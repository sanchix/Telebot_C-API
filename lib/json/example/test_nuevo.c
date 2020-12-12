
#include "../json.c"


int main (){
  char *json = "{\"ok\":true,\"result\":[{\"update_id\":596179679,\"message\":{\"message_id\":16,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340555,\"text\":\"Parada claro\"}},{\"update_id\":596179680,\"message\":{\"message_id\":17,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340661,\"text\":\"Ho\"}}]}";

  /**json_parsed_t parsed;

  if (json_parse(json,&parsed)==0){
    int valor=json_primeFromObj("ok",parsed);
    if (valor!=-1){
      printf("ok:%d\n",valor);
    }
  }else{
    printf ("Error en parse.\n");
  }

  **/

  analize_all(json);
  
  
  return 0;
}
