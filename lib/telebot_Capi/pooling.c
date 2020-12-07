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
#include <stdio.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"


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
	useconds_t interval = 1000000;
	
	bot_info_t *bot_info = (bot_info_t *)info;
	
	while(1){
		
		usleep(interval);
		printf("Soy pool: %s\n", bot_info->url);
		//status = http_get(&hi, url, response, size);
		
	}
	
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

