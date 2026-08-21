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

int checkCookie() {
    for(int i=0;i<MAX_SESSIONS;i++) {
        if(cookie_storage[i]->timeout != 0 && cookie_storage[i]->timeout < millis()) {
            cookie_storage[i]->timeout = 0;
        }
    }
    if (server.hasHeader("Cookie")) {
        String cookieHeader = server.header("Cookie");
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

// Create a new session, store its hash and send it to the browser as a cookie (1h retention)
void setCookie() {
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
  server.sendHeader("Set-Cookie", String(COOKIE_NAME) + "=" + hash + "; Max-Age=3600; Path=/; HttpOnly");
}

// Serve the login page: prefer a custom login.html from storage, fallback to the hardcoded minimal page
void serveLoginPage() {
  if(LittleFS.exists("/login.html")) {
    File loginFile = LittleFS.open("/login.html", "r");
    if(loginFile) {
      server.streamFile(loginFile, "text/html");
      loginFile.close();
      return;
    }
  }
  server.send(200, "text/html", loginHtmlFallback);
}

// Serve the main application page (index.html) from storage
void serveIndexPage() {
  String indexHtmlFS = "<html><body><h1>File 'index.html' not found.</h1><hr><a href=/fs>Upload Files</a></body></html>";
  if(LittleFS.exists("/index.html")) {
    File storage = LittleFS.open("/index.html", "r");
    if(storage) {
      indexHtmlFS = storage.readString();
      storage.close();
    }
  }
  server.send(200, "text/html", indexHtmlFS);
}

// Register a protected endpoint: only calls the real handler when a valid session cookie is present,
// otherwise serves the login page
void authOn(const String &uri, HTTPMethod method, std::function<void(void)> handler) {
  server.on(uri, method, [handler]() {
    if(checkCookie()) {
      handler();
    } else {
      serveLoginPage();
    }
  });
}

// Validate credentials against CFG.data.userName/password, open a session and redirect to "/"
void handleLoginSubmit() {
  String userName = server.arg("userName");
  String password = server.arg("password");
  if(userName.equals(String(CFG.data.userName)) && password.equals(String(CFG.data.password))) {
    setCookie();
    server.sendHeader("Location", "/");
  } else {
    server.sendHeader("Location", "/login?error=1");
  }
  server.send(302, "text/plain", "");
}

// Register the public login endpoints (must remain accessible without a cookie)
void loginServerInit() {
  server.on("/login", HTTP_GET, serveLoginPage);
  server.on("/login", HTTP_POST, handleLoginSubmit);
}
