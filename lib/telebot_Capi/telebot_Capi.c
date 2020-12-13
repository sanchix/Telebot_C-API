/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Código principal de la librería "telebot_Capi". Contiene las definiciones de las funciones que podrá utilizar el usuario de la librería.
*/

#include <string.h>
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
int telebot_init(char *tok, bot_info_t *bot_info){
	
	int ret = 0;
	printf("Initializing telebot_Capi\n");
	
	// Inicializamos la librería https_lib
	http_init(&(bot_info->hi), TRUE);
	
	// Formamos la URL de acceso al BOT y la almacenamos en bot_info.
	strcpy(bot_info->url,API_URL);
	strcat(bot_info->url,tok);
	
	// Inicializamos la funcion de pooling.
	if(tbc_pooling_init(bot_info) != 0){
		printf("telebot_Capi: Error initializing pooling thread\n");
		ret = -1;
	}
	
	return ret;
	
}


/*
**   Parámetros:  char *response: Valor devuelto
**                <tipo2> <parm2> <Descripción>
**                ...
**     Devuelve:  <tipo> <Descripción>
**
**  Descripción:  <Descripción>
*/
//  #TODO: Cambiar char *response a la estructura de respuesta.
int telebot_getMe(char *response, int size, bot_info_t *bot_info){
	
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


int telebot_sendMessage(char *response, int size, bot_info_t *bot_info,char chat_id,char text){
	
	int ret = -1;
	int status = 0;
	char url[200];
	char method[15] = "/sendMessage";
	//char data[100] = "{\"chat_id\":150848014,\"text\":\"Hola\"}";
	char data[100];//Maximo tamaño permitido por telegram es 4096 creo

	sprintf(data,"{\"chat_id\":%s,\"text\":\"%s\"}",chat_id,text);
	
	strcpy(url, bot_info->url);
	strcat(url, method);
	status = http_post(&(bot_info->hi), url, data, response, size);
	//printf("status: %d \n",status);
	printf("%s \n",response);
	if(status == 200){
		ret = 0;
	}
	
	return ret;
	
}
