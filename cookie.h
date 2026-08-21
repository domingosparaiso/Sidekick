#pragma once

#include <functional>

// Name of the browser cookie used to hold the session token
#define COOKIE_NAME "mySessionID"
// Session retention time (1h)
#define COOKIE_TIMEOUT_MS 3600000UL

int checkCookie();
void initCookies();
void setCookie();
void serveLoginPage();
void serveIndexPage();
void authOn(const String &uri, HTTPMethod method, std::function<void(void)> handler);
void loginServerInit();
