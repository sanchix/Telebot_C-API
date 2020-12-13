/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.h
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Declaraciones de las funciones, tipos y constantes que podrá utilizar el usuario de la librería.
*/

#ifndef TELEBOT_CAPI_H
#define TELEBOT_CAPI_H

/* Includes del sistema */ 
#include <string.h>

/* Includes de la aplicacion */
#include "pooling.c"
#include "https_lib/https.h"
#include "jansson.h"


/* Definición de constantes */ 
#define API_URL "https://api.telegram.org/bot"


/* Tipos definidos por el usuario */
/*
**		 Campos:  HTTP_INFO hi: Informacion del protocolo HTTPS para la librería de HTTPS.
**				  char url[200]: URL con el token para acceder al bot.
**
**	Descripción: Almacena los datos relativos a la comunicación HTTPS con los servidores de telegram.
 */
typedef struct{
	HTTP_INFO hi;
	// TODO: Pensar el tamaño máximo de la URL y definirlo en una constante a parte.
	char url[200];
} bot_info_t;


// TODO: Comentar
// TODO: Pensar los tamaños máximos de las cadenas (buscar en telegram si hay tamaños máximos)
// TODO: Pensar si es mejor poner punteros en vez de cadenas y que otro reserve la memoria
typedef struct{
	int id;
	int is_bot;
	char first_name[25];
	char last_name[50];
	char language_code[5];
} from_t;

// TODO: Igual que el struct anterior
typedef struct{
	int id;
	char first_name[25];
	char last_name[50];
	char type[20];
} chat_t;

// TODO: Igual que el struct anterior
typedef struct{
	int message_id;
	from_t from;
	chat_t chat;
	long date;
	char text[1000];
} message_t;



/************************************************************/
/* Declaración de funciones exportadas. */

/*
**   Parámetros:  char tok[50]: Token para el acceso al bot.
**                bot_info_t *bot_info: Puntero a una estructura bot_info_t que almacena información del bot relativa a la comunicación con Telegram.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería telebot_Capi.
*/
int telebot_init(char tok[50], bot_info_t *bot_info);


/*
**   Parámetros:  char *response: Valor devuelto por el método de la API getMe.
**                int size: Tamaño de la respuesta.
**				  bot_info_t *bot_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición a la API de telegram con el método getMe, devolviendo la respuesta en *response.
*/
//TODO: Comentar
int telebot_getMe(char *response, int size, bot_info_t *bot_info);


/*
**   Parámetros:  char *response: Valor devuelto por el método de la API getMe.
**                int size: Tamaño de la respuesta.
**				  bot_info_t *bot_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición a la API de telegram con el método getMe, devolviendo la respuesta en *response.
*/
// TODO: comentar: devuelve 0 si bien, -1 si error
int unpack_message(message_t *message, json_t *message_obj);



#endif




