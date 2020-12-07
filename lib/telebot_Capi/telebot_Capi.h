
#include "https_lib/https.h"

/*
 *	Here we define all the functions that could be used outside the library.
 */

typedef struct{
	HTTP_INFO hi;
	char url[200];
} bot_https_t;

int telebot_init(char tok[50], bot_https_t *bot_info);
int telebot_getMe(char *response, int size, bot_https_t *bot_info);

int tbc_pooling_init(bot_https_t *bot_info);