/*
**     Fichero:  telebot_Capi/pooling.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Parte de la libreria telebot_Capi encargada de obtener actualizaciones mediante el método de "long pooling".
*/

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"

#define MAX_RESP_TAM 4096


void *parser(void *resp){
	
	char response[MAX_RESP_TAM];
	
	strcpy(response, (char *)resp);
	
	printf("Updates: %s\n", response);
	
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  char *tok: Token del BOT.
**				  bot_info_t *bot_info: Puntero a un tipo bot_info_t, que almacenará información para la comunicación https con la api de Telegram para el BOT específico.
**                
**     Devuelve:  int: 0 si la inicializción se ha completado con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería.
*/
void *pool(void *info){
	
	int status;
	char url[250];
	char response[MAX_RESP_TAM];
	useconds_t interval = 1000000;
	pthread_attr_t attr;
	pthread_t thread;
	//key_t clave;
	
	// Prepare URL to get updates
	bot_info_t *bot_info = (bot_info_t *)info;
	strcpy(url, bot_info->url); 
	strcat(url, "/getUpdates");
	
	// Prepare variables to create parser threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// Prepare message queue to communicate with parser threads
	//clave = ftok(" //////ME HE QUEDADO AQUI//////
	//msgget();
	
	while(1){
		
		usleep(interval);
		status = http_get(&(bot_info->hi), url, response, MAX_RESP_TAM);
		
		if(status != 200){
			printf("Failed to update data\n");
		}
		else{
			if(pthread_create(&thread, &attr, parser, response) != 0){
				printf("Failed to create parse thread\n");
				break;
			}
			//wait_to_msg(&offset)
		}
		
	}
	
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  bot_info_t *bot_info: Puntero a una variable bot_info_t con información HTTPS del bot.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso
**
**  Descripción:  Inicializa la función de pooling.
*/
int tbc_pooling_init(bot_info_t *bot_info){
	
	int ret = 0;	// Valor de retorno
	pthread_attr_t attr;
	pthread_t thread;
	
	// Se crea un hilo que se encargara del pooling (void *pool())
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &attr, pool, bot_info) != 0)
		ret = -1;
	
	pthread_attr_destroy(&attr);
	
	return ret;
	
}

