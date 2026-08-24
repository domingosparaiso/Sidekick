#pragma once

#include <functional>

// Name of the browser cookie used to hold the session token
#define COOKIE_NAME "mySessionID"
// Session retention time (1h)
#define COOKIE_TIMEOUT_MS 3600000UL

int checkCookie(SidekickRequest request);
void initCookies();
String setCookie();
void serveLoginPage(SidekickRequest request);
void serveIndexPage(SidekickRequest request);
void authOn(const String &uri, SidekickHTTPMethod method, SidekickHandler handler);
void authOnUpload(const String &uri, SidekickHTTPMethod method, SidekickHandler onComplete, SidekickUploadHandler onChunk);
void loginServerInit();
