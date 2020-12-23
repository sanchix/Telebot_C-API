/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.h
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Declaraciones de las funciones, tipos y constantes que podrá utilizar el usuario de la librería, así como las declaraciones de las funciones, tipos y constantes necesarias para el código contenido en pooling.c
*/

#ifndef TELEBOT_CAPI_H
#define TELEBOT_CAPI_H

/* Includes del sistema */ 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h> 
#include <sys/msg.h>
#include <sys/ipc.h>
// TODO: Poner este siguiente include para debug (ifdef debug)
#include <errno.h>

/* Includes de la aplicacion */
#include "https_lib/https.h"
#include "jansson.h"


/* Definición de constantes */ 
// TODO: Ordenar por ficheros (esto y el resto de cosas)
#define API_URL "https://api.telegram.org/bot"
#define MAX_RESP_TAM 4096
#define MAX_POST_TAM 4096
#define MAX_OFFSET_TAM 20
#define OFFSET_MSQ_TYPE 1
#define MAX_URL_TAM 200
#define MAX_USER_TAM 64 // Maximo tamaño para nombre y apellido https://tecnonucleous.com/2019/07/10/los-limites-de-telegram/
#define MAX_UPDATE_EVENTS 20
#define EVENT_UNASSIGNED "null"
#define EVENT_DEFFAULT "deffault"
#define UPDATE_MESSAGE 1
#define UPDATE_DROP 0
#define UPDATE_HOLD 1


/* Tipos definidos por el usuario */
/*
**		 Campos:  HTTP_INFO hi: Informacion del protocolo HTTPS para la librería de HTTPS.
**				  char url[200]: URL con el token para acceder al bot.
**
**	Descripción: Almacena los datos relativos a la comunicación HTTPS con los servidores de telegram.
 */
struct msgbuf{
	long mtype;
	char mtext[MAX_OFFSET_TAM];
};


// TODO: Comentar
// TODO: Pensar los tamaños máximos de language_code y type
// TODO: Pensar si es mejor poner punteros en vez de cadenas y que otro reserve la memoria
typedef struct{
	int id;
	int is_bot;
	char first_name[MAX_USER_TAM];
	char last_name[MAX_USER_TAM];
	char language_code[5];
} from_t;

// TODO: Igual que el struct anterior
typedef struct{
	int id;
	char first_name[MAX_USER_TAM];
	char last_name[MAX_USER_TAM];
	char type[20];
} chat_t;

// TODO: Igual que el struct anterior
typedef struct{
	int message_id;
	from_t from;
	chat_t chat;
	long date;
	char text[MAX_POST_TAM];
} message_t;


/*
**		 Campos:  HTTP_INFO hi: Informacion del protocolo HTTPS para la librería de HTTPS.
**				  char url[200]: URL con el token para acceder al bot.
**
**	Descripción: Almacena los datos relativos a la comunicación HTTPS con los servidores de telegram.
 */
typedef struct{
	HTTP_INFO hi;
	char url[MAX_URL_TAM];
} http_info_t;


// TODO: Comentar
typedef struct{
	int type; // Defined in UPDATE_<type>
	void *content;
	// TODO_ Pensar si esta es la mejor forma de que una función de evento pueda mandar mensajes o si hay otra.
	http_info_t *http_info;
} update_t;


// TODO: Comentar
// TODO: Pensar valores de retorno y parámetros
typedef int (*updateHandle_t)(update_t *);

typedef char* event_t;

// TODO: Comentar
typedef struct{
	updateHandle_t handle;
	// TODO: Add possible events
	event_t event; // EVENT_UNASSIGNED = none
} update_notifier_t;




typedef struct{
	http_info_t http_info;
	update_notifier_t notifiers[MAX_UPDATE_EVENTS];
	sem_t * mutex_updateNotifiers; //La referencia al semáforo realizada en telebot_init
} bot_info_t;


// TODO: Comentar
typedef struct{
	char response[MAX_RESP_TAM];
	bot_info_t *bot_info;
} response_info_t;



/*********************   telebot_Capi.c   *********************/

/*
**   Parámetros:  char tok[50]: Token para el acceso al bot.
**                http_info_t *http_info: Puntero a una estructura http_info_t que almacena información del bot relativa 
**				  a la comunicación con Telegram.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería telebot_Capi.
*/
int telebot_init(char *token, bot_info_t *bot_info);


/*
**   Parámetros:  
**				  
**                
**     Devuelve:  0 si la clausura se ha completado con éxito, -1 en caso de error.
**
**  Descripción:  Cierra semáforos.
*/
int telebot_close();


/*
**   Parámetros:  char *response: Valor devuelto por el método de la API getMe.
**                int size: Tamaño de la respuesta.
**				  http_info_t *http_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición a la API de telegram con el método getMe, devolviendo la respuesta en *response.
*/
//TODO: Comentar
int telebot_getMe(char *response, int size, http_info_t *http_info);


/*
**   Parámetros:  char *chat_id: Id del chat al que mandar la petición.
**				  char *text: Texto a enviar en el mensaje.
**				  http_info_t *http_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición de enviar un mensaje a la API de telegram con el método sendMessage, devolviendo 
**				  la respuesta en *response.
*/
int telebot_sendMessage( char *chat_id,char *text, http_info_t *http_info);



/*********************     pooling.c      *********************/

/*
**   Parámetros:  <tipo1> <parm1> <Descripción>
**                <tipo2> <parm2> <Descripción>
**                ...
**     Devuelve:  <tipo> <Descripción>
**
**  Descripción:  <Descripción>
*/
// TODO: Comentar
int unpack_json_message(message_t *message, json_t *message_obj);


/* Declaración de funciones locales. Para cada función: */
/*
**   Parámetros:  <tipo1> <parm1> <Descripción>
**                <tipo2> <parm2> <Descripción>
**                ...
**     Devuelve:  <tipo> <Descripción>
**
**  Descripción:  <Descripción>
*/
// TODO: Comentar
void *parser(void *resp_info);


// TODO: Comentar
void *poll(void *info);
	

/*
**   Parámetros:  http_info_t *http_info: Puntero a una variable http_info_t con información HTTPS del bot.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso
**
**  Descripción:  Inicializa la función de polling.
*/
int polling_init(bot_info_t *bot_info);



/*********************     events.c      *********************/

// TODO: Comentar
int ignoreUpdate(update_t *p);

// TODO: Comentar
int holdUpdate(update_t *p);

// TODO: Pensar, en cuanto a los update events, si se va a poder tener más de un handle para un mismo evento.

// TODO: Comentar
void initUpdateNotifiers(update_notifier_t *notifiers);


// TODO: Comentar
int addUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info);

// TODO: Comentar
int modifyUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info);

// TODO: Comentar
int removeUpdateNotifier(event_t event, bot_info_t *bot_info);


#endif




