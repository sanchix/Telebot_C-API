/*
**     Fichero:  lib/telebot_Capi/telebot_Capi.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  04/enero/2021
**
** Descripcion:  Código principal de la librería "telebot_Capi". Contiene las definiciones de las funciones que podrá utilizar el usuario de la librería.
*/

/* Includes del sistema */
#include <signal.h>

/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definición de funciones locales. */

// Función LOCAL (no declarada en telebot_Capi)
/*
**   Parámetros:  void *data: Chunk de datos recibidos para una cierta petición.
**                size_t size: Tamaño en unidades del chunk.
**				  size_t nmemb: Tamaño de unidad.
**				  void *userp: Puntero a la estructura donde almacenar la información.
**                
**     Devuelve:  static size_t: 
**
**  Descripción:  Almacena la respuesta de la petición en la estructura http_response_t apuntada por userp.
*/
static size_t tbc_http_callback(void *data, size_t size, size_t nmemb, void *userp){
	
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



/*
**   Parámetros:  int stop: Indica con un 1 que se debe parar la aplicación.
**                
**     Devuelve:  int: 0 si la aplicación debe parar, 1 e.o.c.
**
**  Descripción:  Indica al llamante si la aplicación debe parar o seguir con su ejecución. El parámetro permite modificar el estado.
*/
int isRunning(int stop, bot_info_t **bi){
	static int running = 0;
	static bot_info_t *bot_info = NULL;
	if(stop == 1){
		running = 1;
	}else if(stop == 2){
		running = 0;
	}
	// Si no esta establecido bot_info y *bi tiene algo se establece.
	if(bot_info == NULL && *bi != NULL){
		bot_info = *bi;
	}
	// SI bot_info está establecido y **bi no es null se devuelve
	else if(bot_info != NULL && bi != NULL){
		*bi = bot_info;
	}
	
	return running;
}



/*
**   Parámetros:  int sig: Se ignora.
**
**  Descripción:  Manda la terminación de los hilos internos a la librería y recursos reservados en la inicialización de la misma (semáforo mutex_updateNotifiers, curl, ...). El resto de hilos deben librerar sus recursos antes de cerrase.
*/
void telebot_close(int sig){
	
	pid_t pid;
	bot_info_t *bot_info;
	
	printf("\033[1;33m");
	printf("\nCerrando telebot_Capi\n");
	printf("\033[0m");
	
	
	// Indicamos a todos los hilos que se paren
	isRunning(2, &bot_info);
	sleep(4);
	
	// 		Liberar recursos:
	// Semáforo mutex_updateNotifiers
	if ( sem_unlink("mutex_updateNotifiers")!=0 ){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_close: Ha habido un problema al cerrar el semaforo mutex_updateNotifiers");
		printf("####################################################\n");
		printf("\033[0m");		
	}
	// Cerrar librería curl (recursos del hilo principal)
	curl_easy_cleanup(bot_info->http_info.curlhandle);
	curl_global_cleanup();
	sleep(1);
	
	
	printf("\033[1;33m");
	printf("Bot cerrado correctamente\n");
	printf("\033[0m");
	printf("\n");
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
	struct curl_slist *headers = NULL;
	int ret = 0;
	
	
	
	printf("\033[1;33m");
	printf("Initializing telebot_Capi\n");
	printf("\033[0m");
	
	/* --- Inicializar funciones generales --- */
	// Se arma la señal para la terminación
	term.sa_handler= telebot_close;
	term.sa_flags = 0;
	sigemptyset(&term.sa_mask);
	if ( sigaction(SIGINT, &term, NULL) != 0 ){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_init: Fallo al armar la señal \n");
		printf("####################################################\n");
		printf("\033[0m");
		ret = -1;
	}
	
	// Inicializar la librería curl
	curl_global_init(CURL_GLOBAL_ALL);
	
	isRunning(1, &bot_info);
	
	
	/* --- Inicializar notifiers --- */
	// Crear semáforo mutex_updateNotifiers.
	//Comprobamos que no esté creado el semáforo, y si lo está, lo eliminamos para crearlo de nuevo(para que no haya bloqueos donde no debe).
	if (sem_open("mutex_updateNotifiers",0) != SEM_FAILED){
		sem_unlink("mutex_updateNotifiers");
	}
    if((mutex_updateNotifiers = sem_open(
	"mutex_updateNotifiers", O_CREAT,0600,1)) == SEM_FAILED){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_init: Fallo al abrir el semáforo\n");
		printf("####################################################\n");
		printf("\033[0m");
		ret = -1;
	}
	bot_info->notifiers_info.mutex_updateNotifiers = mutex_updateNotifiers;

	// Inicializar los handlers
	initUpdateNotifiers(bot_info->notifiers_info.notifiers);


	/* --- Inicializar http_info --- */	
	// Inicializar y configurar handle para este hilo (principal - envíos)
	if((bot_info->http_info.curlhandle = curl_easy_init()) == NULL){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_Capi: Error initializing curl_easy\n");
		printf("####################################################\n");
		printf("\033[0m");
		ret = -1;
	}
	// Estas opciones son recomendables
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_SSL_VERIFYHOST, 0L);
	// Opcion importante para multithread
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_NOSIGNAL, 1L);
	// Configurar manejador a tbc_http_callback
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_WRITEFUNCTION, tbc_http_callback);
	// Configurar las cabeceras para enviar json
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(bot_info->http_info.curlhandle, CURLOPT_HTTPHEADER, headers);
	// Se comprueba el SSL
	data = curl_version_info(CURLVERSION_NOW);
	if(data->ssl_version != NULL){
		printf("\033[1;33m");
		printf("Utilizando SSL version: %s\n", data->ssl_version);
		printf("\033[0m");
	}else{
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_init: Error, no se diposne de SSL\n");
		printf("####################################################\n");
		printf("\033[0m");
		ret = -1;
	}
	
	// Formar URL de acceso a la API para el BOT (en http_info)
	strcpy(bot_info->http_info.url,API_URL);
	strcat(bot_info->http_info.url,token);
	
	
	/* --- Inicializar hilos internos --- */
	// Inicializar la funcion de polling.
	if(tbc_polling_init(bot_info) != 0){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_Capi: Error initializing polling thread\n");
		printf("####################################################\n");
		printf("\033[0m");
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
int telebot_getMe(http_response_t *http_response, http_info_t *http_info){

	int ret = -1;
	char url[MAX_URL_TAM];
	char method[10] = "/getMe";
	CURLcode res;
	
	// Formamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);
	
	// Realizamos la petición
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)http_response);
	http_response->response = NULL;
	http_response->size = 0;
	res = curl_easy_perform(http_info->curlhandle);
	if(res == CURLE_OK){
		ret = 0;
	}
	else{
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_getMe: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		printf("####################################################\n");
		printf("\033[0m");
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
		/*
		int unpack_message(message_t *message, json_t *message_obj);
		*/
	
	return ret;
	
}


/*
**   Parámetros:  char *chat_id: Id del chat al que mandar la petición a telegram para que se realice el envío del mensaje.
**				  char *text: Texto a enviar en el mensaje.
**				  http_info_t *http_info: Creado en telebot_init()
**                
**     Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición de enviar un mensaje a la API de telegram con el método sendMessage, devolviendo la respuesta en *response.
*/
int telebot_sendMessage( char *chat_id,char *text, http_info_t *http_info){
	
	http_response_t http_response; //Valor devuelto por el método de la API sendMessage.
	int ret = -1;
	CURLcode res;
	char url[MAX_URL_TAM];
	char method[15] = "/sendMessage";
	char data[MAX_POST_TAM];
	json_error_t error;	
	json_t *root;
	json_t *ok;
	json_t *result;
	json_t *texto;

	// Generamos la url con el método correspondiente
	strcpy(url, http_info->url);
	strcat(url, method);
	
	// Generamos la cadena JSON
	sprintf(data,"{\"chat_id\":\"%s\",\"text\":\"%s\"}",chat_id,text);

	// Realizamos la petición con POST

	curl_easy_setopt(http_info->curlhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)&http_response);
	http_response.response = NULL;
	http_response.size = 0;
	res = curl_easy_perform(http_info->curlhandle);

	if(res == CURLE_OK){

		// Realizamos el parse con la librería jansson para extraer el texto que se ha enviado y comprobar 
	       
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
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("telebot_sendMessage: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		printf("####################################################\n");
		printf("\033[0m");
	}
	
	curl_easy_setopt(http_info->curlhandle, CURLOPT_HTTPGET, 1L); 
	
	return ret;
	
}


/*
**	Parámetros:	  char *chat_id: Id del chat al que mandar la petición a telegram para que se realice el envío del mensaje.
**				  char *question: Texto sobre la encuesta.
**				  char **options: Lista con el texto de las opciones. 
**				  http_info_t *http_info: Creado en telebot_init()
**                
**	   Devuelve:  0 si la petición finaliza correctamente, -1 en caso de error.
**
**  Descripción:  Realiza una petición de enviar un mensaje a la API de telegram con el método sendPoll, devolviendo la respuesta en *response.
*/
int telebot_sendPoll(char *chat_id, char *question, char **options, http_info_t *http_info){
	
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
	strcpy(parsedoptions,opcion);
	for (int i = 1 ; options[i] != NULL ; i++){
		sprintf(opcion,",\"%s\"",options[i]);
		strcat(parsedoptions,opcion);
	}
	
	sprintf(data,"{\"chat_id\":%s,\"question\":\"%s\",\"options\":[%s]}",chat_id,question,parsedoptions);

	// Realizamos la petición con POST
	printf("##########################-telebot_sendPoll-##########################\n");
	printf("#   Enviando encuesta\n");
	printf("#   Encuesta send: %s\n", data);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_POSTFIELDS, data);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_URL, url);
	curl_easy_setopt(http_info->curlhandle, CURLOPT_WRITEDATA, (void *)&http_response);
	http_response.response = NULL;
	http_response.size = 0;
	res = curl_easy_perform(http_info->curlhandle);
	printf("Encuesta send: %s\n", http_response.response);
	printf("####################################################\n");
	if(res == CURLE_OK){
		ret = 0;
	}
	return ret;
	
}
