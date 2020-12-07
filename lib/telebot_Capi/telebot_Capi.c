
#include <string.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"

#define API_URL "https://api.telegram.org/bot"


int telebot_init(char *tok, bot_https_t *bot_info){
	
	printf("Initializing telebot_Capi\n");
	strcpy(bot_info->url,API_URL);
	strcat(bot_info->url,tok);
	http_init(&(bot_info->hi), TRUE);
	if(tbc_pooling_init(bot_info) != 0)
		printf("Error initializing pooling thread\n");
	
	printf("End initializing\n");
	
	return 0;
	
}

int telebot_getMe(char *response, int size, bot_https_t *bot_info){
	
	int ret = -1;
	int status = 0;
	char url[200];
	char method[10] = "/getMe";
	
	strcpy(url, bot_info->url);
	strcat(url, method);
	status = http_get(&(bot_info->hi), url, response, size);
	if(status == 200){
		ret = 0;
	}
	
	return ret;
	
}