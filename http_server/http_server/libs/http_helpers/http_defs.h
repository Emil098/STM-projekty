/*
 * http_defs.h
 *
 *  Created on: Feb 18, 2026
 *      Author: emilr
 */

#ifndef HTTP_DEFS_H_
#define HTTP_DEFS_H_

#define PAGE_404 "HTTP/1.0 404 Page not found\r\n" \
                 "Server: EAS\r\n" \
                 "Content-type: text/html\r\n\r\n" \
                 "<!DOCTYPE html><html><body>404 Page not found</body></html>"

#define HTTP_204 "HTTP/1.0 204 No Content\r\n" \
                 "Server: EAS\r\n\r\n"


#define LOGIN_OK_HEAD "HTTP/1.0 302 Found\r\n" \
                      "Server: EAS\r\n" \
                      "Location: /dashboard.shtml\r\n" \
                      "Cache-Control: no-store\r\n" \
                      "Set-Cookie: sid="
#define LOGIN_OK_PAGE "; Max-Age=60; Path=/; SameSite=Strict; HttpOnly\r\n" \
                      "\r\n"

#define LOGOUT_PAGE "HTTP/1.0 200 OK\r\n" \
                    "Server: EAS\r\n" \
                    "Content-type: text/html\r\n" \
                    "Set-Cookie: sid=0; Max-Age=0; SameSite=Strict\r\n\r\n" \
                    "<!DOCTYPE html><html><head><meta http-equiv=\"Refresh\" content=\"0; url=dashboard.shtml\" /></head>" \
                    "<body>Logged out successfully. Redirecting...</body></html>"

#define JSON_HEAD(str)      "HTTP/1.0 200 OK\r\n" \
                            "Server: EAS\r\n" \
                            "Content-type: application/json\r\n" \
                            "Content-Disposition: attachment; filename=\"" \
                            str \
                            "\"\r\n\r\n"

#define JSON_BODY_BEGG "{ "
#define JSON_BODY_END  "}"

#define COOKIE_REDIR_HTM "/cookie_ok.htm\0"


#endif /* HTTP_DEFS_H_ */
