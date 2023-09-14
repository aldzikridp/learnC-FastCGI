#include <fcgiapp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

  printf("Hello.\n");

  FCGX_Init();
  int Socket = FCGX_OpenSocket("127.0.0.1:2016", 1024);
  FCGX_Request Request;
  FCGX_InitRequest(&Request, Socket, 0);

  while (FCGX_Accept_r(&Request) >= 0) {
    printf("Request accepted.\n");

    char *Path = FCGX_GetParam("SCRIPT_NAME", Request.envp);

    char Title[64] = {0};
    char Status[64] = "200 OK";

    if (strcmp(Path, "/") == 0) {
      strcpy(Title, "Home");
    } else if (strcmp(Path, "/about/") == 0) {
      strcpy(Title, "About");
    } else {
      strcpy(Title, "404 Not Found");
      strcpy(Status, "404 Not Found");
    }

    // form handling

    char *Method = FCGX_GetParam("REQUEST_METHOD", Request.envp);

    char Form[512] = {0};

    if (strcmp(Method, "GET") == 0) {
      strcpy(Form,
             "<form method='POST' action=''>"
             "<input id='username' name='username' type='text'></input>"
             "<input id='password' name='password' type='password'></input>"
             "<input type='submit' value='Submit'>"
             "</form>");
    } else if (strcmp(Method, "POST") == 0) {

      // read form data

      int ContentLength = atoi(FCGX_GetParam("CONTENT_LENGTH", Request.envp));

      char *ContentBuffer =
          malloc((ContentLength + 1) * sizeof(*ContentBuffer));

      FCGX_GetStr(ContentBuffer, ContentLength, Request.in);
      ContentBuffer[ContentLength] = '\0';
      strcpy(Form, ContentBuffer);
      free(ContentBuffer);
    }

    FCGX_FPrintF(Request.out,
                 "Status: %s\r\n"
                 "Content-type: text/html\r\n\r\n"
                 "<h1>%s</h1>"
                 "<p>Path: %s</p>"
                 "<div>Form: <br>%s</div>",
                 Status, Title, Path, Form);
    FCGX_Finish_r(&Request);
  }

  return 0;
}
