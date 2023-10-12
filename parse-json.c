#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcgiapp.h>
#include <json-c/json.h>

char *get_post_request_str(FCGX_Request app_request){
  int content_length = atoi(FCGX_GetParam("CONTENT_LENGTH", app_request.envp));
  char *content_buffer = malloc((content_length + 1) * sizeof(*content_buffer));
  FCGX_GetStr(content_buffer, content_length, app_request.in);
  content_buffer[content_length] = '\0';

  return content_buffer;
};

int main(void) {
  FCGX_Init();
  int app_socket = FCGX_OpenSocket("127.0.0.1:2016", 1024);
  FCGX_Request app_request;
  FCGX_InitRequest(&app_request, app_socket, 0);

  while (FCGX_Accept_r(&app_request) >= 0) {
    printf("Incoming request\n");

    char *request_method = FCGX_GetParam("REQUEST_METHOD", app_request.envp);

    if (strcmp(request_method, "POST") == 0) {

      char *content_buffer = get_post_request_str(app_request);

      json_object *json_req  = json_tokener_parse(content_buffer);

      json_object *json_tmp_name = json_object_object_get(json_req, "name");
      json_object *json_tmp_lastname = json_object_object_get(json_req, "lastname");

      int name_length = json_object_get_string_len(json_tmp_name);
      int lastname_length = json_object_get_string_len(json_tmp_lastname);
      int fullname_length = name_length+lastname_length+2; // 2 more space for space and terminator

      char *name = strdup(json_object_get_string(json_tmp_name));
      char *lastname = strdup(json_object_get_string(json_tmp_lastname));
      char *fullname = malloc(fullname_length * sizeof(*fullname));
      strcat(fullname, name);
      strcat(fullname, " ");
      strcat(fullname, lastname);

      free(name);
      free(lastname);

      printf("Your fullname is: %s\n", fullname);
      printf("The json object to string:\n\n%s\n", json_object_to_json_string(json_req));

      char *app_status = "200 OK";

      FCGX_FPrintF(app_request.out,
                   "Status: %s\r\n"
                   "Content-type: application/json\r\n\r\n"
                   "%s",app_status, content_buffer);
      FCGX_Finish_r(&app_request);
      free(content_buffer);
      free(fullname);
      json_object_put(json_req);
    }
  }
  return 0;
}
