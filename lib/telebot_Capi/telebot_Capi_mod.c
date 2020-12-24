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
static size_t curl_callback(void *data, size_t size, size_t nmemb, void *userp){
	printf("Algo\n");
	size_t realsize = size * nmemb;
	http_response_t *response = (http_response_t *)userp;
 
	char *ptr = realloc(response->response, response->size + realsize + 1);
	if(ptr == NULL)
		return 0;  /* out of memory! */
 
	response->response = ptr;
	memcpy(&(response->response[response->size]), data, realsize);
	response->size += realsize;
	response->response[response->size] = 0;
 
	return realsize;
}

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
void telebot_close(int a){
	
	pid_t pid;
	
	printf("Cerrando telebot_Capi\n");
	if ( sem_unlink("mutex_updateNotifiers")!=0 ){
		printf("Ha habido un problema al cerrar el semaforo mutex_updateNotifiers");
	}
	isRunning(1);
	printf("Bot cerrado correctamente\n");
	
	sleep(5);
	curl_global_cleanup();
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
	curl_version_info_data *data;
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
	bot_info->notifiers_info.mutex_updateNotifiers = mutex_updateNotifiers;

	// Inicializamos la librería curl
	curl_global_init(CURL_GLOBAL_ALL);
	
	// Se inicializa el handle para este hilo (principal - envíos)
	bot_info->http_info.curlhandle = curl_easy_init();
	if(bot_info->http_info.curlhandle == NULL){
		printf("telebot_Capi: Error initializing curl_easy\n");
	}
	// Se configura:
	// Estas opciones son recomendables
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_SSL_VERIFYHOST, 0L);
	
	// Opcion importante para multithread
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_NOSIGNAL, 1L);
	
	// El manejador se ajusta también con una opción
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_WRITEFUNCTION, curl_callback);
	
	// Se comprueba que tenemos SSL y se configuran opciones.
	data = curl_version_info(CURLVERSION_NOW);
	if(data->ssl_version != NULL){
		printf("Utilizando SSL version: %s\n", data->ssl_version);
	}else{
		printf("Telebot_init: Error, no se diposne de SSL\n");
	}
	
	// Se forma la URL de acceso a la API para el BOT y se almacena en http_info.
	// TODO: ¿comprobar si no desborda? ¿existe un parámetro en strcpy y strcat para verlo?
	strcpy(bot_info->http_info.url,API_URL);
	strcat(bot_info->http_info.url,token);
	
	// Inicializamos los handlers (para el "por defecto" -> dejar en la cola, los demás vacíos)
	initUpdateNotifiers(bot_info->notifiers_info.notifiers);
	
	
	// Inicializamos la funcion de polling.
	//if(polling_init(bot_info) != 0){
	//	printf("telebot_Capi: Error initializing polling thread\n");
	//}
    ret = 0;
	
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
int telebot_getMe(http_response_t *http_response, http_info_t *http_info){
	
	// TODO: Cambiar char *response a la estructura de respuesta.
	// TODO: Pensar si el espacio para la respuesta la debe reservar el que llama a la función o la función en sí (en cuyo caso devolvería la estructura en el return, o un puntero a ella.

	int ret = -1;
	char url[MAX_URL_TAM];
	char method[10] = "/getMe";
	CURLcode res;
	
	// Formamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);
	
	// Realizamos la petición
	//status = http_get(&(http_info->hi), url, response, size);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)http_response);
	http_response->response = NULL;
	http_response->size = 0;
	res = curl_easy_perform(http_info->curlhandle);
	if(res == CURLE_OK){
		ret = 0;
	}
	else{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
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
	
	http_response_t http_response; //Valor devuelto por el método de la API sendMessage.
	int ret = -1;
	CURLcode res;
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
	//status = http_post(&(http_info->hi), url, data, respuesta, MAX_RESP_TAM);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)&http_response);
	http_response.response = NULL;
	http_response.size = 0;
	res = curl_easy_perform(http_info->curlhandle);
	printf("Send message: %s\n", data);
	if(res == CURLE_OK){

		// Realizamos el parse con la librería jansson para extraer el texto que se ha enviado y comprobar 
		json_error_t error;	
		json_t *root;
		json_t *ok;
		json_t *result;
		json_t *texto;

		root = json_loads(http_response.response, 0, &error);
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
	else{
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	
	curl_easy_setopt(http_info->curlhandle, CURLOPT_HTTPGET, 1L); 
	
	return ret;
	
}

int telebot_sendPoll( char *chat_id,char *question,char **options, http_info_t *http_info){
	
	http_response_t http_response; //Valor devuelto por el método de la API sendPoll.
	int ret = -1;
	CURLcode res;
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
	printf("Send encuesta\n");
	//status = http_post(&(http_info->hi), url, data, respuesta, MAX_RESP_TAM);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)&http_response);
	http_response.response = NULL;
	http_response.size = 0;
	printf("Aqui\n");
	res = curl_easy_perform(http_info->curlhandle);
	printf("Encuesta send\n");

	if(res == CURLE_OK){

		// TODO: Comprobar
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
