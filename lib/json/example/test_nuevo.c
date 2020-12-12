
#include "../json.c"


int main (){
  char *json ="{\"message_id\":16,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607340555,\"text\":\"Parada claro\"}";

  json_parsed_t parsed;

  if (json_parse(json,&parsed)==0){
    int valor=json_primeFromObj("date",parsed);
    if (valor!=-1){
      printf("date:%d\n",valor);
    }
    else
      printf("Error en la busqueda de la clave.\n");
  }else{
    printf ("Error en parse.\n");
  }

 

  //analize_all(json);
  
  
  return 0;
}
