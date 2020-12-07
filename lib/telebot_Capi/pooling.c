
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"

void *pool(void *info){
	
	int status;
	printf("hilo\n");
	//bot_https_t *bot_info = (bot_https_t *)info;
	printf("Soy hilo\n");
	
	while(1){
		
		sleep(5);
		printf("Soy pool: %s", ((bot_https_t *)info)->url);
		//status = http_get(&hi, url, response, size);
		
	}
	
}

int tbc_pooling_init(bot_https_t *bot_info){
	
	int error = 0;
	pthread_attr_t attr;
	pthread_t thread;
	
	printf("Initializating pooling\n");
	
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	printf("Creating thread...\n");
	
	if(pthread_create(&thread, &attr, pool, bot_info) != 0)
		error = -1;
	
	printf("Thread created\n");
	
	pthread_attr_destroy(&attr);
	
	printf("Attribute destroyed\n");
	
	return error;
	
}

