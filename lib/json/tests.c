#include "jsmn.h"
#include <stdio.h>

int main(){
	
	char json_string[] = "{\"ok\":true,\"result\":[{\"update_id\":596179723,\"message\":{\"message_id\":60,\"from\":{\"id\":833182696,\"is_bot\":false,\"first_name\":\"Juan\",\"last_name\":\"Parada\",\"language_code\":\"es\"},\"chat\":{\"id\":833182696,\"first_name\":\"Juan\",\"last_name\":\"Parada\",\"type\":\"private\"},\"date\":1607765471,\"text\":\"Aeeeeee\"}},{\"update_id\":596179724,\"message\":{\"message_id\":61,\"from\":{\"id\":150848014,\"is_bot\":false,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"language_code\":\"en\"},\"chat\":{\"id\":150848014,\"first_name\":\"Javi\",\"last_name\":\"Sanchi\",\"type\":\"private\"},\"date\":1607766596,\"text\":\"Ros co\u00f1o\"}}]}";
	json_parsed_t parsed;
	
	json_parse(json_string, &parsed);
	printf("Bien\n");
	
	return 0;
	
}
