/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Código principal de la librería "telebot_Capi". Contiene las definiciones de las funciones que podrá utilizar el usuario de la librería.
*/

/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definición de funciones locales. */

/*
**   Parámetros:  char *token: Token del BOT.
**				  http_info_t *http_info: Devuelve información de la comunicación https con Telegram para el BOT específico.
**                
**     Devuelve:  0 si la inicializción se ha completado con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería.
*/
int telebot_init(char *token, bot_info_t *bot_info){
	
	int ret = -1;
	printf("Initializing telebot_Capi\n");

	//Inicializamos los semáforos
	sem_t * mutex_updateEvents;
    mutex_updateEvents = sem_open("mutex_updateEvents", O_CREAT,0600,1);
	bot_info->mutex_updateEvents = mutex_updateEvents;

	// Inicializamos la librería https_lib
	// TODO: Comprobar que la inicialización es correcta
	http_init(&(bot_info->http_info.hi), TRUE);
	
	// Formamos la URL de acceso al BOT y la almacenamos en http_info.
	// TODO: ¿comprobar si no desborda? ¿existe un parámetro en strcpy y strcat para verlo?
	strcpy(bot_info->http_info.url,API_URL);
	strcat(bot_info->http_info.url,token);
	
	// Inicializamos los handlers (para el "por defecto" -> dejar en la cola, los demás vacíos)
	initUpdateNotifiers(bot_info->notifiers);
	
	// Inicializamos la funcion de pooling.
	if(tbc_pooling_init(bot_info) != 0){
		printf("telebot_Capi: Error initializing pooling thread\n");
	}
	else{
		ret = 0;
	}
	
	return ret;
	
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
int telebot_close(){
	
	int ret = 0;
	printf("Cerrando telebot_Capi\n");
	if ( sem_unlink("mutex_updateEvents")!=0 ){
		printf("Ha habido un problema al cerrar el semaforo mutex_updateEvents");
		ret = -1;
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
