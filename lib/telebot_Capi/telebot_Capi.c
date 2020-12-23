/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Código principal de la librería "telebot_Capi". Contiene las definiciones de las funciones que podrá utilizar el usuario de la librería.
*/

/* Includes del sistema */
#include <signal.h>

/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definición de funciones locales. */

// TODO: Comentar
int isRunning(int stop){
	static int running = 1;
	if(stop){
		running--;
	}
	return running;
}

/*
**   Parámetros:  
**				  
**                
**     Devuelve:  0 si la clausura se ha completado con éxito, -1 en caso de error.
**
**  Descripción:  Cierra semáforos.
*/
// TODO: hacer que cierre tambien la cola...
void telebot_close(void){
	
	pid_t pid;
	
	printf("Cerrando telebot_Capi\n");
	if ( sem_unlink("mutex_updateNotifiers")!=0 ){
		printf("Ha habido un problema al cerrar el semaforo mutex_updateNotifiers");
	}
	isRunning(1);
	printf("Bot cerrado correctamente\n");
	
	sleep(5);
	pid = getpid();
	kill(pid, SIGKILL);
	
}


/*
**   Parámetros:  char *token: Token del BOT.
**				  bot_info_t *bot_info: Devuelve información necesaria para el funcionamiento de la libería.
**                
**     Devuelve:  0 si éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería.
*/
int telebot_init(char *token, bot_info_t *bot_info){
	
	struct sigaction term;
	sem_t * mutex_updateNotifiers;
	int ret = -1;
	
	printf("Initializing telebot_Capi\n");
	
	// Se arma la señal para la terminación
	term.sa_handler= telebot_close;
	term.sa_flags = 0;
	sigemptyset(&term.sa_mask);
	sigaction(SIGINT, &term, NULL);
	
	//Inicializamos los semáforos
    if((mutex_updateNotifiers = sem_open(
	"mutex_updateNotifiers", O_CREAT,0600,1)) == SEM_FAILED){
		printf("Telebot_init: Fallo al abrir el semáforo\n");
	}
	bot_info->mutex_updateNotifiers = mutex_updateNotifiers;

	// Inicializamos la librería https_lib
	// TODO: Comprobar que la inicialización es correcta
	http_init(&(bot_info->http_info.hi), TRUE);
	
	// Se forma la URL de acceso a la API para el BOT y se almacena en http_info.
	// TODO: ¿comprobar si no desborda? ¿existe un parámetro en strcpy y strcat para verlo?
	strcpy(bot_info->http_info.url,API_URL);
	strcat(bot_info->http_info.url,token);
	
	// Inicializamos los handlers (para el "por defecto" -> dejar en la cola, los demás vacíos)
	initUpdateNotifiers(bot_info->notifiers);
	
	// Inicializamos la funcion de polling.
	if(polling_init(bot_info) != 0){
		printf("telebot_Capi: Error initializing polling thread\n");
	}
	else{
		ret = 0;
	}
	
	return ret;
	
}


/*
**   Parámetros:  char *response: Valor devuelto por el método de la API getMe.
**                int size: Tamaño de la respuesta.
**				  http_info_t *http_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición a la API de telegram con el método getMe, devolviendo la respuesta en *response.
*/
int telebot_getMe(char *response, int size, http_info_t *http_info){
	
	// TODO: Cambiar char *response a la estructura de respuesta.
	// TODO: Pensar si el espacio para la respuesta la debe reservar el que llama a la función o la función en sí (en cuyo caso devolvería la estructura en el return, o un puntero a ella.

	int ret = -1;
	int status = 0;
	char url[MAX_URL_TAM];
	char method[10] = "/getMe";
	
	// Formamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);
	
	// Realizamos la petición
	status = http_get(&(http_info->hi), url, response, size);
	if(status == 200){
		ret = 0;
	}
	
	// TODO: Analizar la respuesta y guardarla en la estructura respuesta (previamente hay que crearla, ver los TODOs del principio de esta función)
	// TODO: En telebot_Capi.h estaba esto:
		/*
		**   Parámetros:  char *response: Valor devuelto por el método de la API getMe.
		**                int size: Tamaño de la respuesta.
		**				  http_info_t *http_info: Creado en telebot_init()
		**                
		**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
		**
		**  Descripción:  Realiza una petición a la API de telegram con el método getMe, devolviendo la respuesta en *response.
		*/
		// TODO: comentar: devuelve 0 si bien, -1 si error
		/*
		int unpack_message(message_t *message, json_t *message_obj);
		*/
	
	return ret;
	
}


/*
**   Parámetros:  char *chat_id: Id del chat al que mandar la petición.
**				  char *text: Texto a enviar en el mensaje.
**				  http_info_t *http_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición de enviar un mensaje a la API de telegram con el método sendMessage, devolviendo la respuesta en *response.
*/
int telebot_sendMessage( char *chat_id,char *text, http_info_t *http_info){

	// TODO: Cambiar el parámetro chat_id y añadir los parámetros necesarios para mandar un mensaje (comprobar que es lo que se debe mandar en el método sendMessage en la API de Telegram). 
	// ¿Hacer que se pasen los parámetros mediante una estructura (podría ser útil para reenvíar mensajes recibidos)? ¿Hacer dos funciones una con parámetros separados y otra con la estructura?
	
	char respuesta[MAX_RESP_TAM]; //Valor devuelto por el método de la API sendMessage.
	int ret = -1;
	int status = 0;
	char url[MAX_URL_TAM];
	char method[15] = "/sendMessage";
	char data[MAX_POST_TAM];

	// Generamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);
	
	// Generamos la cadena JSON
	// TODO: ¿Hacer una función para que haga esto?
	sprintf(data,"{\"chat_id\":%s,\"text\":\"%s\"}",chat_id,text);

	// Realizamos la petición con POST
	status = http_post(&(http_info->hi), url, data, respuesta, MAX_RESP_TAM);
	printf("Send message: %s\n", data);

	if(status == 200){

		// Realizamos el parse con la librería jansson para extraer el texto que se ha enviado y comprobar 
		json_error_t error;	
		json_t *root;
		json_t *ok;
		json_t *result;
		json_t *texto;

		root = json_loads(respuesta, 0, &error);
		// Obtenemos el valor de ok
		ok = json_object_get(root, "ok");
		if(json_is_true(ok)){
			result = json_object_get(root, "result");		
			// Se obtiene el objeto from
			texto = json_object_get(result, "text");
			if (strcmp(json_string_value(texto),text)==0){
				ret = 0;
			}
		}

	}
	return ret;
	
}

int telebot_sendPoll( char *chat_id,char *question,char **options, http_info_t *http_info){
	
	char respuesta[MAX_RESP_TAM]; //Valor devuelto por el método de la API sendPoll.
	int ret = -1;
	int status = 0;
	char url[MAX_URL_TAM];
	char data[15*MAX_POLL_OPTION_TAM];
	char method[15] = "/sendPoll";
	char parsedoptions[10*MAX_POLL_OPTION_TAM];
	char opcion[MAX_POLL_OPTION_TAM];

	// Generamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);

	// Generamos la cadena JSON
	sprintf(opcion,"\"%s\"",options[0]);
	strcat(parsedoptions,opcion);
	for (int i = 1 ; options[i] != NULL ; i++){
		sprintf(opcion,",\"%s\"",options[i]);
		strcat(parsedoptions,opcion);
	}
	
	sprintf(data,"{\"chat_id\":%s,\"question\":\"%s\",\"options\":[%s]}",chat_id,question,parsedoptions);

	// Realizamos la petición con POST
	status = http_post(&(http_info->hi), url, data, respuesta, MAX_RESP_TAM);
	printf("Send message: %s\n", data);

	if(status == 200){

		/*
		// Realizamos el parse con la librería jansson para extraer el texto que se ha enviado y comprobar 
		json_error_t error;	
		json_t *root;
		json_t *ok;
		json_t *result;
		json_t *texto;

		root = json_loads(respuesta, 0, &error);
		// Obtenemos el valor de ok
		ok = json_object_get(root, "ok");
		if(json_is_true(ok)){
			result = json_object_get(root, "result");		
			// Se obtiene el objeto from
			texto = json_object_get(result, "text");
			if (strcmp(json_string_value(texto),text)==0){
				ret = 0;
			}
		}
		*/
		ret = 0;
	}
	return ret;
	
}
