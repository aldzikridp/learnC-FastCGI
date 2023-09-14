#include <stdio.h>
#include <fcgiapp.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
  FCGX_Init();
  int app_socket = FCGX_OpenSocket("127.0.0.1:2016", 1024);
  FCGX_Request app_request;
  FCGX_InitRequest(&app_request, app_socket, 0);

  while (FCGX_Accept_r(&app_request) >= 0) {
    printf("Incoming request\n");

    char *request_method = FCGX_GetParam("REQUEST_METHOD", app_request.envp);

    if (strcmp(request_method, "POST") == 0) {

      int content_length = atoi(FCGX_GetParam("CONTENT_LENGTH", app_request.envp));
      char *content_buffer = malloc((content_length + 1) * sizeof(*content_buffer));

      FCGX_GetStr(content_buffer, content_length, app_request.in);
      content_buffer[content_length] = '\0';

      char *app_status = "200 OK";

      FCGX_FPrintF(app_request.out,
                   "Status: %s\r\n"
                   "Content-type: application/json\r\n\r\n"
                   "%s",app_status, content_buffer);
      FCGX_Finish_r(&app_request);
      free(content_buffer);
    }
  }
  return 0;
}
