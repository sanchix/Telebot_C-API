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
//#include "https_lib/https.h"
#include "jansson.h"
#include "curl.h"


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
#define COMANDO '/'
#define MAX_COMMAND_TAM 30
#define MAX_POLL_QUESTION_TAM 300
#define MAX_POLL_OPTION_TAM 100
#define MAX_POLL_OPTIONS 10

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
typedef enum{
	UPDATE_NONE = -1,
	UPDATE_ANY = 0,
	UPDATE_MESSAGE = 1,
	UPDATE_POLL = 2,
	// TODO: ¿Crear otra entrada para eventos con comando?
} updatetype_t;

// TODO: Comentar
// TODO: Pensar los tamaños máximos de language_code y type
// TODO: Pensar si es mejor poner punteros en vez de cadenas y que otro reserve la memoria
typedef struct{
	int id;	// if id = 0 don't use content (struct empty)
	int is_bot;
	char first_name[MAX_USER_TAM];
	char last_name[MAX_USER_TAM]; // void if last_name[0] = '\0'
	char language_code[10]; // void if language_code[0] = '\0'
} user_t;

// TODO: Igual que el struct anterior
typedef struct{
	int id;
	char first_name[MAX_USER_TAM]; // void if first_name[0] = '\0'
	char last_name[MAX_USER_TAM]; // void if last_name[0] = '\0'
	char type[20];
} chat_t;

// TODO: Igual que el struct anterior
// TODO: Poner un content type y un content (mensage...) o pensar como manejar cuando el mensaje tiene más campos que no sean text ¿una lista? ¿un array? ¿una lista enlazada?
typedef struct{
	int message_id;
	user_t from; // Si from.id = 0 no se debe utilizar.
	chat_t chat;
	long date;
	char *text; // void if text = NULL
	char *command; // void if command = NULL
} message_t;

typedef struct{
	char text [MAX_POLL_OPTION_TAM] ;
	int opcion_votos;
}option_t;

typedef struct{
	unsigned long long int poll_id;
	char question [MAX_POLL_QUESTION_TAM];
	option_t options [MAX_POLL_OPTIONS];
	int total_votos;       
} poll_update_t;



/*
**		 Campos:  HTTP_INFO hi: Informacion del protocolo HTTPS para la librería de HTTPS.
**				  char url[200]: URL con el token para acceder al bot.
**
**	Descripción: Almacena los datos relativos a la comunicación HTTPS con los servidores de telegram.
 */
typedef struct{
	CURL *curlhandle;
	char url[MAX_URL_TAM];
} http_info_t;


// TODO: Comentar
typedef struct{
	updatetype_t type; // Defined in UPDATE_<type>
	void *content;
	// TODO_ Pensar si esta es la mejor forma de que una función de evento pueda mandar mensajes o si hay otra.
	http_info_t *http_info;
} update_t;


// TODO: Comentar
// TODO: Pensar valores de retorno y parámetros
typedef void (*updateHandle_t)(update_t *);

// TODO: Comentar
// TODO: Añadir más calificadores para los eventos
typedef struct{
	updatetype_t update_type;
	char info[MAX_COMMAND_TAM];	//"" for general messages, disabled ig update_type = UPDATE_ANY or UPDATE_NONE
} event_t;

// TODO: Comentar
typedef struct{
	updateHandle_t handle;
	event_t event; // EVENT_UNASSIGNED = NULL
} update_notifier_t;


typedef struct{
	// TODO: Ver si usar el semáforo así es thread-safe.
	sem_t * mutex_updateNotifiers; //La referencia al semáforo realizada en telebot_init
	update_notifier_t notifiers[MAX_UPDATE_EVENTS];
} notifiers_info_t;

typedef struct{
	http_info_t http_info;
	notifiers_info_t notifiers_info;
} bot_info_t;

typedef struct{
	http_info_t http_info;
	notifiers_info_t *notifiers_info;
} poll_info_t;
	
// TODO: Comentar
typedef struct{
	char *response;
	size_t size;
} http_response_t;

// TODO: Comentar
typedef struct{
	http_response_t http_response;
	poll_info_t *poll_info;
} response_info_t;



/*********************   telebot_Capi.c   *********************/

/*
**   Parámetros:  int stop: Indica con un 1 que se debe parar la aplicación.
**                
**     Devuelve:  int: 0 si la aplicación debe parar, 1 e.o.c.
**
**  Descripción:  Indica al llamante si la aplicación debe parar o seguir con su ejecución. El parámetro permite modificar el estado.
*/
int isRunning(int stop, bot_info_t **bi);


/*
**   Parámetros:  int sig: Se ignora.
**
**  Descripción:  Manda la terminación de los hilos internos a la librería y recursos reservados en la inicialización de la misma (semáforo mutex_updateNotifiers, curl, ...). El resto de hilos deben librerar sus recursos antes de cerrase.
*/
void telebot_close(int a);


/*
**   Parámetros:  char *token: Token del BOT.
**				  bot_info_t *bot_info: Devuelve información necesaria para el funcionamiento de la libería.
**                
**     Devuelve:  0 si éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería.
*/
int telebot_init(char *token, bot_info_t *bot_info);


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
int telebot_getMe(http_response_t *http_response, http_info_t *http_info);


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

int telebot_sendPoll( char *chat_id,char *question,char **options, http_info_t *http_info);



/*********************     pooling.c      *********************/

/*
**   Parámetros:  bot_info_t *bot_info: Información de la librería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error
**
**  Descripción:  Inicializa la función de polling.
*/
int tbc_polling_init(bot_info_t *bot_info);



/*********************     events.c      *********************/

/*
**   Parámetros:  update_t *p: Se ignora
**
**  Descripción:  Ignora un update
*/
void ignoreUpdate(update_t *p);


// TODO: Pensar, en cuanto a los update events, si se va a poder tener más de un handle para un mismo evento.
// Memoria: Hemos quitado holdUpdate porque no tiene sentido almacenar updates individualmente (vienen en objetos). Como mucho tendría sentido al inicializar, pero para eso hacemos que el usuario configure los eventos de antemano y punto.


/*
**   Parámetros:  update_notifier_t *notifiers: array notifier_t a inicializar
**                
**     Devuelve:  void
**
**  Descripción:  Inicializa los eventos. Se establece el handle del evento por defecto (notifiers[0]) a ignoreUpdate. El resto de eventos se configuran como UPDATE_NONE y con el sin manejador. La función no lleva semáforo pues no tiene porqué garantizar exclusión mutua en los eventos (aún no se ha ejecutado el hilo de polling)
*/
void initUpdateNotifiers(update_notifier_t *notifiers);


/*
**   Parámetros:  updateHandle_t handle: manejador para el evento
**                event_t *event: evento
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar un hueco o el nuevo evento tiene tipo UPDATE_NONE).
**
**  Descripción:  Añade 'event' en los notifiers de bot_info. Si 'event' tiene tipo UPDATE_ANY se guarda el handle en el evento por defecto (efecto igual a modifyUpdateNotifier()); en otro caso se almacena en el primer notifier cuyo evento sea de tipo UPDATE_NONE, notificando error si no es posible.
*/
int addUpdateNotifier(updateHandle_t handle, event_t *event, bot_info_t *bot_info);


/*
**   Parámetros:  updateHandle_t handle: nuevo manejador para el evento.
**                event_t *event: nombre del evento a modificar
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar el evento). 
**
**  Descripción:  Edita el manejador de 'event'.
*/
int modifyUpdateNotifier(updateHandle_t handle, event_t *event, bot_info_t *bot_info);


/*
**   Parámetros:  event_t *event: evento a eliminar
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar el evento).
**
**  Descripción:  Borra el manejador del notifier identificado por 'event' y cambia el evento a UPDATE_NONE. Si el tipo de 'event' es UPDATE_ANY se cambia el manejador del evento por defecto a ignoreUpdate.
*/
int removeUpdateNotifier(event_t *event, bot_info_t *bot_info);


/*
**   Parámetros:  event_t event: nombre del evento que estamos buscando
**				  bot_info_t *bot_info 	
**				  updateHandle_t *handle: ṕuntero a la direccion de memoria en la que hay que dejar el handler
**                
**     Devuelve:  int Devuelve 0 en caso de exit, -1 en caso de que no se haya podido encontrar. 
**
**  Descripción:  Comprobamos si existe un evento. 
*/
updateHandle_t findUpdateHandler(update_t *update, update_notifier_t *notifiers);


#endif




