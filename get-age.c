#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcgiapp.h>
#include <json-c/json.h>

char *get_post_request_str(FCGX_Request app_request){
  int content_length = atoi(FCGX_GetParam("CONTENT_LENGTH", app_request.envp));
  printf("content length: %d\n", content_length);
  char *content_buffer = malloc((content_length + 1) * sizeof(*content_buffer));
  FCGX_GetStr(content_buffer, content_length, app_request.in);
  content_buffer[content_length] = '\0';

  return content_buffer;
};

char *get_fullname(json_object *json_req){
  json_object *tmp_name = json_object_object_get(json_req, "name");
  json_object *tmp_lastname = json_object_object_get(json_req, "lastname");

  char *name = strdup(json_object_get_string(tmp_name));
  char *lastname = strdup(json_object_get_string(tmp_lastname));
  char *fullname = malloc(strlen(name) + strlen(lastname) + 1);

  strcat(fullname, name);
  strcat(fullname, " ");
  strcat(fullname, lastname);
  free(name);
  free(lastname);

  return fullname;
};

int calculate_age(int birthYear, int birthMonth, int birthDay){
  time_t currentTime;
  struct tm *localTime;
  int currentYear, currentMonth, currentDay, age;

  currentTime = time(NULL);
  localTime = localtime(&currentTime);

  currentYear = localTime->tm_year + 1900;
  currentMonth = localTime->tm_mon + 1;
  currentDay = localTime->tm_mday;

  age = currentYear - birthYear;
  if(currentMonth < birthDay || (currentMonth == birthMonth && currentDay < birthDay)){
    age--;
  }
  return age;
};


int main(void) {
  FCGX_Init();
  int app_socket = FCGX_OpenSocket("127.0.0.1:2016", 1024);
  FCGX_Request app_request;
  FCGX_InitRequest(&app_request, app_socket, 0);

  while (FCGX_Accept_r(&app_request) >= 0) {
    printf("Incoming request\n");

    char *request_method = FCGX_GetParam("REQUEST_METHOD", app_request.envp);
    char *client_addr = FCGX_GetParam("REMOTE_ADDR", app_request.envp);

    printf("Client IP: %s\n",client_addr);

    if (strcmp(request_method, "POST") == 0) {

      char *content_buffer = get_post_request_str(app_request);

      json_object *json_req  = json_tokener_parse(content_buffer);

      char *fullname = get_fullname(json_req);

      json_object *tmp_year = json_object_object_get(json_req, "birthyear");

      int birthYear = json_object_get_int(tmp_year);

      json_object *tmp_month = json_object_object_get(json_req, "birthmonth");

      int birthMonth = json_object_get_int(tmp_month);

      json_object *tmp_day = json_object_object_get(json_req, "birthday");

      int birthDay = json_object_get_int(tmp_day);

      int age = calculate_age(birthYear, birthMonth, birthDay);

      printf("Your fullname is: %s\n", fullname);
      printf("Your birth year is: %d\n", birthYear);
      printf("Your birth month is: %d\n", birthMonth);
      printf("Your birth day is: %d\n", birthDay);
      printf("Your age is: %d\n", age);
      printf("The json object to string:\n\n%s\n", json_object_to_json_string(json_req));

      char *app_status = "200 OK";

      FCGX_FPrintF(app_request.out,
                   "Status: %s\r\n"
                   "Content-type: application/json\r\n\r\n"
                   "%s",app_status, content_buffer);
      FCGX_Finish_r(&app_request);
      free(content_buffer);
      content_buffer = NULL;
      free(fullname);
      fullname = NULL;
      json_object_put(json_req);
    }
  }
  return 0;
}
