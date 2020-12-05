
#include <string.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"

#define API_URL "https://api.telegram.org/bot"

char token[50] = {};
HTTP_INFO hi;

int telebot_init(char tok[50]){
	
	strcpy(token,tok);
	http_init(&hi, TRUE);
	
	return 0;
	
}

int telebot_getMe(char *response, int size){
	
	int ret = -1;
	int status = 0;
	char url[100] = API_URL;
	char method[10] = "/getMe";
	
	strcat(url, token);
	strcat(url, method);
	printf("%s\n",url);
	status = http_get(&hi, url, response, size);
	if(status == 200){
		ret = 0;
	}
	
	return ret;
	
}