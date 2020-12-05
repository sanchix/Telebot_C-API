
#include <string.h>
#include "lib/https.h"
#include "telebot_Capi.h"

#define API_URL "https://api.telegram.org/bot"

char token[50] = NULL;
HTTP_INFO hi = NULL;

void telebot_init(char tok[50]){
	
	token = strcpy(token,tok);
	http_init(&hi, TRUE);
	
}

char *telebot_getMe(){
	
	char url[100] = API_URL;
	char method[10] = "getMe";
	char response[4096] = NULL;
	int ret = 0;
	
	strcat(url, token);
	strcat(url, method);
	
	ret = http_get(&hi, url, response, sizeof(response));
	
	return response;
	
}