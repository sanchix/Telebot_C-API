/*
**     Fichero:  telebot_Capi/telebot_Capi.h
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Declaraciones de las funciones, tipos y constantes que podrá utilizar el usuario de la librería.
*/

#ifndef <FICHERO>_H
#define <FICHERO>_H


/* -------- Includes de la aplicacion -------- */
#include "https_lib/https.h"

/* Definición de constantes */ 
#define API_URL "https://api.telegram.org/bot"

/* -------- Tipos definidos por el usuario -------- */
/*
**		 Campos:  HTTP_INFO hi: Informacion del protocolo HTTPS para la librería de HTTPS.
**				  char url[200]: URL con el token para acceder al bot.
**
**	Descripción: Almacena los datos relativos a la comunicación HTTPS con los servidores de telegram.
 */
typedef struct{
	HTTP_INFO hi;
	char url[200];
} bot_info_t;

/* -------- Declaración de funciones exportadas -------- */
/*
**   Parámetros:  char tok[50]: Token para el acceso al bot.
**                bot_info_t *bot_info: Puntero a una estructura bot_info_t que almacena información del bot relativa a la comunicación con Telegram.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería telebot_Capi.
*/
int telebot_init(char tok[50], bot_https_t *bot_info);


/*
**   Parámetros:  bot_info_t *bot_info: Puntero a una variable bot_info_t con información HTTPS del bot.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso
**
**  Descripción:  Inicializa la función de pooling.
*/
int tbc_pooling_init(bot_https_t *bot_info);


#endif


int telebot_getMe(char *response, int size, bot_https_t *bot_info);

