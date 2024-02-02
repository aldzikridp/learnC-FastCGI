#include <stdlib.h>
#include <fcgiapp.h>

int main(void){
  FCGX_Init();
  int app_socket = FCGX_OpenSocket("127.0.0.1:2016", 1024);
  FCGX_Request app_request;
  FCGX_InitRequest(&app_request, app_socket, 0);

  while (FCGX_Accept_r(&app_request) >= 0) {
    char *client_addr = FCGX_GetParam("REMOTE_ADDR", app_request.envp);
    FCGX_FPrintF(app_request.out,
                 "Status: 200 OK\r\n"
                 "Content-type: text/plain\r\n\r\n"
                 "%s",client_addr);
    FCGX_Finish_r(&app_request);
    client_addr = NULL;
  }
  return 0;
};
