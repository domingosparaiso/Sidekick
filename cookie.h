#pragma once

#include <functional>

// Max of concurrent users
#define MAX_SESSIONS 10
// Name of the browser cookie used to hold the session token
#define COOKIE_NAME "mySessionID"
// Session retention time (5 min)
#define COOKIE_TIMEOUT_MS 300000UL

int checkCookie(SidekickRequest request);
void initCookies();
String setCookie();
void serveLoginPage(SidekickRequest request);
void serveIndexPage(SidekickRequest request);
void authOn(const String &uri, SidekickHTTPMethod method, SidekickHandler handler);
void authOnUpload(const String &uri, SidekickHTTPMethod method, SidekickHandler onComplete, SidekickUploadHandler onChunk);
void loginServerInit();
