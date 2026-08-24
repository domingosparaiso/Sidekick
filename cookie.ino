#include <LittleFS.h>
#include "wificonfig.h"
#include "cookie.h"

#ifdef ESP32
  #include "mbedtls/md.h"
#endif
#ifdef ESP8266
  #include <user_interface.h>
  #include <bearssl/bearssl.h>
#endif

#define MAX_SESSIONS 10

class cookieItem {
  public:
    String id;
    unsigned long timeout;
};

cookieItem* cookie_storage[MAX_SESSIONS];

// Minimal hardcoded login page, used only when no login.html exists in storage
const char *loginHtmlFallback = "<!DOCTYPE html><html><body><h3>Login</h3>"
  "<form method='POST' action='/login'>"
  "<input name='userName' placeholder='User'><br />"
  "<input name='password' type='password' placeholder='Password'><br />"
  "<input type='submit' value='Enter'>"
  "</form></body></html>";

void initCookies() {
    for(int i=0;i<MAX_SESSIONS;i++) {
        cookie_storage[i] = new cookieItem();
        cookie_storage[i]->timeout = 0;
    }
}

int checkCookie(SidekickRequest request) {
    for(int i=0;i<MAX_SESSIONS;i++) {
        if(cookie_storage[i]->timeout != 0 && cookie_storage[i]->timeout < millis()) {
            cookie_storage[i]->timeout = 0;
        }
    }
    if (req_hasHeader(request, "Cookie")) {
        String cookieHeader = req_header(request, "Cookie");
        int index = cookieHeader.indexOf("=");
        if (cookieHeader.substring(0,index).equals(COOKIE_NAME)) {
            String hashCookie = cookieHeader.substring(index + 1);
            for(int i=0;i<MAX_SESSIONS;i++) {
                if(cookie_storage[i]->timeout != 0 && hashCookie.equals(cookie_storage[i]->id)) {
                    return(1);
                }
            }
        }
    }
    return(0);
}

String generateRandomHash() {
  uint8_t randomBytes[32];
  uint8_t hashResult[32];

  #ifdef ESP32
    esp_fill_random(randomBytes, sizeof(randomBytes));

    mbedtls_md_context_t ctx;
    mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

    mbedtls_md_init(&ctx);
    mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
    mbedtls_md_starts(&ctx);
    mbedtls_md_update(&ctx, randomBytes, sizeof(randomBytes));
    mbedtls_md_finish(&ctx, hashResult);
    mbedtls_md_free(&ctx);
  #endif

  #ifdef ESP8266
    os_get_random(randomBytes, sizeof(randomBytes));

    br_sha256_context ctx;
    br_sha256_init(&ctx);
    br_sha256_update(&ctx, randomBytes, sizeof(randomBytes));
    br_sha256_out(&ctx, hashResult);
  #endif

  String hashString = "";
  for (int i = 0; i < 32; i++) {
    if (hashResult[i] < 16) {
      hashString += "0";
    }
    hashString += String(hashResult[i], HEX);
  }

  return hashString;
}

// Create a new session and return the "Set-Cookie" header value to send it to the browser (1h retention)
String setCookie() {
  String hash = generateRandomHash();

  // reuse a free slot, or replace the oldest session if the table is full
  int slot = 0;
  unsigned long oldest = cookie_storage[0]->timeout;
  for(int i=0;i<MAX_SESSIONS;i++) {
    if(cookie_storage[i]->timeout == 0) {
      slot = i;
      oldest = 0;
      break;
    }
    if(cookie_storage[i]->timeout < oldest) {
      oldest = cookie_storage[i]->timeout;
      slot = i;
    }
  }

  cookie_storage[slot]->id = hash;
  cookie_storage[slot]->timeout = millis() + COOKIE_TIMEOUT_MS;
  return String(COOKIE_NAME) + "=" + hash + "; Max-Age=3600; Path=/; HttpOnly";
}

// Serve the login page: prefer a custom login.html from storage, fallback to the hardcoded minimal page
void serveLoginPage(SidekickRequest request) {
  if(LittleFS.exists("/login.html")) {
    req_sendFile(request, "/login.html", "text/html");
    return;
  }
  req_send(request, 200, "text/html", loginHtmlFallback);
}

// Serve the main application page (index.html) from storage
void serveIndexPage(SidekickRequest request) {
  if(LittleFS.exists("/index.html")) {
    req_sendFile(request, "/index.html", "text/html");
    return;
  }
  req_send(request, 200, "text/html", "<html><body><h1>File 'index.html' not found.</h1><hr><a href=/fs>Upload Files</a></body></html>");
}

// Register a protected endpoint: only calls the real handler when a valid session cookie is present,
// otherwise serves the login page
void authOn(const String &uri, SidekickHTTPMethod method, SidekickHandler handler) {
  req_on(uri, method, [handler](SidekickRequest request) {
    if(checkCookie(request)) {
      handler(request);
    } else {
      serveLoginPage(request);
    }
  });
}

// Same as authOn(), but for an upload endpoint (protects the completion handler only,
// same as the original per-endpoint behavior; upload chunk handlers check the cookie themselves)
void authOnUpload(const String &uri, SidekickHTTPMethod method, SidekickHandler onComplete, SidekickUploadHandler onChunk) {
  req_onUpload(uri, method, [onComplete](SidekickRequest request) {
    if(checkCookie(request)) {
      onComplete(request);
    } else {
      serveLoginPage(request);
    }
  }, onChunk);
}

// Validate credentials against CFG.data.userName/password, open a session and redirect to "/"
void handleLoginSubmit(SidekickRequest request) {
  String userName = req_arg(request, "userName");
  String password = req_arg(request, "password");
  ReqHeader headers[2];
  int n = 0;
  String location = "/login?error=1";
  if(userName.equals(String(CFG.data.userName)) && password.equals(String(CFG.data.password))) {
    headers[n++] = { "Set-Cookie", setCookie() };
    location = "/";
  }
  headers[n++] = { "Location", location };
  req_send(request, 302, "text/plain", "", headers, n);
}

// Register the public login endpoints (must remain accessible without a cookie)
void loginServerInit() {
  req_on("/login", HTTP_GET, serveLoginPage);
  req_on("/login", HTTP_POST, handleLoginSubmit);
}
