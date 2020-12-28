/*
**     Fichero:  telebot_Capi/pooling.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Parte de la libreria telebot_Capi encargada de obtener actualizaciones mediante el método de "long polling".
*/

/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definiciones de funciones locales. */

// Función LOCAL (no declarada en telebot_Capi)
/*
**   Parámetros:  message_t * message: Devuelve el mensaje recibido desempaquetado.
**                json_t * message_obj: Objeto json (del mensaje) completo a procesar.
**                
**     Devuelve:  int: 0 si éxito, -1 en caso de error.
**
**  Descripción:  Extrae del objeto mensaje (json) ciertos campos y los guarda en la estructura apuntada por 'message'.
*/
int unpack_json_message(message_t *message, json_t *message_obj){
	
	int ret = 0;
	json_t *aux;
	json_t *aux2;
	char *st_aux;
	char *st_aux2;
	int tam;
	
	// TODO: Ver si hay alguna forma de detectar errores o evitar copiar cadenas más grandes del tamaño reservado (a ver si en strcpy y strcat hay algún parámetro "size" o tamaño máximo) PD: NO TE RAYE
	
	// TODO: Poner opcionales las cosas opcionales (rellenar con null). Hay que hacer que el resto de objetos que usen message_t se esperen que haya cosas NULL.
	
	// Desempaquetamos message_id
	message->message_id = json_integer_value(json_object_get(message_obj, "message_id"));
	
	// Desempaquetamos from (si hay, pues es opcional)
	if ((aux = json_object_get(message_obj, "from")) != NULL){
		
		message->from.id = json_integer_value(json_object_get(aux, "id"));
		message->from.is_bot = json_boolean_value(json_object_get(aux, "is_bot"));
		strcpy(message->from.first_name, json_string_value(json_object_get(aux, "first_name")));
		
		// Last name is optional
		if((aux2 = json_object_get(aux, "last_name")) != NULL){
			strcpy(message->from.last_name, json_string_value(aux2));
		}
		else{
			message->from.last_name[0] = '\0';
		}
		
		// Language code is optional
		if((aux2 = json_object_get(aux, "language_code")) != NULL){
			strcpy(message->from.language_code, json_string_value(aux2));
		}
		else{
			message->from.language_code[0] = '\0';
		}
		
	}
	else{
		message->from.id = 0;
	}
	
	// Desempaquetamos date
	message->date = json_integer_value(aux);
	
	// Desempaquetamos chat
	aux = json_object_get(message_obj, "chat");
	message->chat.id = json_integer_value(json_object_get(aux, "id"));
	strcpy(message->chat.type, json_string_value(json_object_get(aux, "type")));
	// Desempaquetamos first name (es opcional)
	if((aux = json_object_get(aux, "first_name")) != NULL){
		strcpy(message->chat.first_name, json_string_value(aux));
	}
	else{
		message->chat.first_name[0] = '\0';
	}
	// Desempaquetamos last_name (es opcional)
	if((aux = json_object_get(aux, "last_name")) != NULL){
		strcpy(message->chat.last_name, json_string_value(aux));
	}
	else{
		message->chat.last_name[0] = '\0';
	}
	
	// Desempaquetamos text (es opcional)
	if((aux = json_object_get(message_obj, "text")) != NULL){
		
		st_aux = json_string_value(aux);
		if((message->text = (char *)malloc(strlen(st_aux))) == NULL){
				printf("> Parse: text malloc failed\n");
		}
		strcpy(message->text, json_string_value(aux));
		
		// Si lo recibido es un comando lo almacenamos en command.
		if(message->text[0] == '/'){
			printf("Tiene comando\n");
			st_aux2 = strchr(st_aux, ' ');
			tam = strlen(st_aux) - 1;
			if(st_aux2 != NULL){
				tam = tam - ((&st_aux[tam] - st_aux2) + 1);
			}
			if(tam != 0){
				if((message->command = (char *)malloc(tam)) == NULL){
					printf("> Parse: command malloc failed\n");
				}
				sscanf(message->text, "/%s ", message->command);
			}
			else{
				message->command = NULL;
			}
		}
		else{
			// TODO: Inicializar todo a NULL y cambiar luego.
			message->command = NULL;
		}
		
	}
	else{
		message->text = NULL;
		message->command = NULL;
	}
	  
	return ret;
	
}


/*
**   Parámetros:  poll_update_t *poll: Estructura de tipo poll para guardar los parametros mas importantes del json recibido.
**                json_t * message_obj: Objeto json completo a procesar.
**                
**     Devuelve:  int: 0 si éxito(el objeto se ha procesado correctamente), -1 en caso de error.
**
**  Descripción:  Función que a partir de un objeto json nos lo formatea en un objeto message_t con nuestros valores a recoger.
*/
// DONE: pensar si es mejor reservar memoria y devolver el mensaje del tirón o que reserve memoria el llamante (esta última opción es como está ahora)
int unpack_json_poll_update(poll_update_t *poll, json_t *message_obj){
	
	// DONE: Inicializar ret a -1 (suponer error) y cambiar a 0 en caso de que todo vaya bien
	int ret = 0;
	json_t *json_opciones;
	json_t *json_opcion;
	
	
	// TODO: Ver si hay alguna forma de detectar errores o evitar copiar cadenas más grandes del tamaño reservado (a ver si en strcpy y strcat hay algún parámetro "size" o tamaño máximo) PD: NO TE RAYE
	
	// TODO: Poner opcionales las cosas opcionales (rellenar con null). Hay que hacer que el resto de objetos que usen message_t se esperen que haya cosas NULL.
	
	// Desempaquetamos id

	//TODO: Poner ifs de correcta recogida y reflejar en ret.
	poll->poll_id = json_integer_value(json_object_get(message_obj, "id"));
	strcpy(poll->question,json_string_value(json_object_get(message_obj, "question")));
	json_opciones = json_object_get (message_obj,"options");
	for(size_t i = 0; i < json_array_size(json_opciones); i++)
	  {
	    json_opcion = json_array_get(json_opciones, i);
	    strcpy(poll->options[i].text, json_string_value(json_object_get(json_opcion,"text")));
	    poll->options[i].opcion_votos = json_integer_value(json_object_get(json_opcion,"voter_count"));
	  }
	
	
	  
	return ret;
	
}

/*
**   Parámetros:  void *r_info: Puntero a estructura response_info_t con información de la respuesta http.
**
**  Descripción:  Analiza la respuesta recibida por http.
*/
void *tbc_parser(void *info){
	
	key_t clave;
	int msgqueue_id;
	// TODO: Dimensionar update_id
	long long int update_id = 0;
	struct msgbuf msq_buffer;
	updateHandle_t handle;
	response_info_t *resp_info;
	update_t update;
	sem_t *mutex_updateNotifiers;

	json_t *json_root;
	json_error_t error;
	json_t *json_ok;
	json_t *json_result;
	json_t *json_update;
	json_t *json_aux;
	
	// Creamos la cola de mensajes para comunicarnos con pool
	clave = ftok(".", 'p');
	msq_buffer.mtype = OFFSET_MSQ_TYPE;
	if((msgqueue_id = msgget(clave, 0660)) == -1){
		printf("> Parser thread: error al acceder a la cola de mensajes: %s\n", strerror(errno));
	}
	
	else{
		// Peparar resp_info (evita usar el cast, comididad)
		resp_info = (response_info_t *)info;
		// Preparar los semáforos
		mutex_updateNotifiers = resp_info->poll_info->notifiers_info->mutex_updateNotifiers;
		
		// TODO: Quitar (es para depuración).
		printf("\nUpdate:\n");
		
		// Realizar el parse con la librería jansson
		json_root = json_loads(resp_info->http_response.response, 0, &error);
		
		// Obtenemos el valor de ok
		// TODO: Hacer algo con esto, que notifique a alguien si no es ok o haga algo de provecho más que imprimirlo.
		json_ok = json_object_get(json_root, "ok");

		//TODO: Probar si esto funciona bien
		if(json_is_false(json_ok)){
			printf("Message received not ok: %s (error code: %lld)\n",json_string_value(json_object_get(json_root,"description")),json_integer_value(json_object_get(json_root,"error_code")));
		 
		}
		
		// Si todo va bien
		else{
			// TODO: Quitar, es de debug
			printf("\tok\n");
		
			// TODO: Implementar la búsqueda rápida del último update_id para comunicárselo a pool. Hacer mediciones de tiempo para ver si realmente merece la pena.
			update.http_info = &(resp_info->poll_info->http_info);
		
			// Analizamos el resultado (lista de objetos update)
			json_result = json_object_get(json_root, "result");
			// TODO: Quitar este printf (debug)
			//printf("JSON: %s", json_string_value(json_result));
			// Para cada objeto del array
			
			for(size_t i = 0; i < json_array_size(json_result); i++){
				
				// Se obtiene el objeto update
				json_update = json_array_get(json_result, i);
				
				// Obtenemos update_id
				json_aux = json_object_get(json_update, "update_id");
				update_id = json_integer_value(json_aux);
				//TODO: quitar esta impresión (es debug)
				printf("\tUpdate id: %lld\n", update_id);
				
				
				/* --- Procesar el update --- */
				// TODO: Poner en los if los distintos objetos de respuesta que se pueden recibir.
				// Si lo que se ha recibido es un mensaje...
				if((json_aux = json_object_get(json_update, "message")) != NULL){
					// Obtenemos el objeto message y lo desempaquetamos
					update.type = UPDATE_MESSAGE;
					// TODO: Hay que liberar
					//TODO: Si no se asigna correctamente la memoria rip
					if((update.content = (message_t *)malloc(sizeof(message_t))) == NULL){
						printf("Poll: malloc failed\n");
					}
					
					unpack_json_message(update.content, json_aux);
					
				}
				
				// Si lo que se ha recibido es una actualización de encuesta...
				else if((json_aux = json_object_get(json_update, "poll")) != NULL){
			       	// Obtenemos el resultado de la encuesta
				  	// TODO: Hacer
				 	if((update.content = (poll_update_t *)malloc(sizeof(poll_update_t)))==NULL){
						printf("Poll: poll update malloc failed\n");
					}
				  
					update.type = UPDATE_POLL;
					printf("Received poll update\n");
				}
				else{
					update.type = UPDATE_NONE;
					printf("Something received\n");
				}
				
				//TODO: Que deberia de ocurrir en el caso de que se produjera un error?				
				
				// Se entra en la región compartida
				sem_wait(mutex_updateNotifiers);
				
				// Se obtiene el handle a utilizar
				handle = findUpdateHandler(&update, resp_info->poll_info->notifiers_info->notifiers);
				handle(&update);
				
				//TODO: Que deberia de ocurrir en el caso de que se produjera un error?
				// Se sale de la región compartida
				sem_post(mutex_updateNotifiers);
				
				// Se liberan recursos
				// TODO: Liberar recursos en caso de encuesta y otros mensajes
				if(update.type == UPDATE_MESSAGE){
					if(((message_t *)update.content)->text != NULL){
						free(((message_t *)update.content)->text);
					}
					if(((message_t *)update.content)->command != NULL){
						free(((message_t *)update.content)->command);
					}
					free(update.content);
				}
			
			}
			
			sprintf(msq_buffer.mtext, "%lld", (update_id != 0)?(update_id + 1):0);
			msgsnd(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, 0);
		
		}
	}
	
	// TODO: ¿Liberar otros recursos?
	free(info);
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  void *info: Puntero a bot_info_t con información de la librería.
**
**  Descripción:  Realiza la función de polling, llamando a tbc_parser() cuando hay nuevas actualizaciones.
*/
void *tbc_poll(void *info){
	
	// TODO: hacer que cierre tambien la cola...
	// TODO: Ordenar
	CURLcode res;
	char url[MAX_URL_TAM];
	char fullurl[MAX_URL_TAM];
	char *offset = "0";
	pthread_attr_t attr;
	pthread_t thread;
	key_t clave;
	int msgqueue_id;
	struct msgbuf msq_buffer;
	response_info_t *resp_info;
	poll_info_t poll_info;
	
	/* --- Prepare HTTP info --- */
	// Prepare poll_info object from bot_info object (in 'info')
	strcpy(poll_info.http_info.url, ((bot_info_t *)info)->http_info.url);
	poll_info.notifiers_info = &((bot_info_t *)info)->notifiers_info;
	// TODO: Ponerle el else o algo de eso
	if((poll_info.http_info.curlhandle = curl_easy_duphandle(((bot_info_t *)info)->http_info.curlhandle)) == NULL){
		printf("polling(): Failed to duplicate curl handle\n");
	}
	
	// Prepare URL to get updates
	strcpy(url, poll_info.http_info.url); 
	strcat(url, "/getUpdates?offset=");
	// Prepare first url to get updates (offset=0)
	strcpy(fullurl, url);
	strcat(fullurl, offset);
	
	/* --- Prepare parser thread info --- */
	// Prepare variables to create parser threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// Prepare message queue to communicate with parser threads
	clave = ftok(".",'p');
	// TODO: Poner este printf en un IFDEF o algo, es util para el usuario final. Tambien se podría crear una función para obtener el token.
	//printf("clave de poll: %i\n", clave);
	if((msgqueue_id = msgget(clave,IPC_CREAT|0660)) == -1){
		printf("> Pooling thread: error al iniciar la cola de mensajes: %s\n", strerror(errno));
		isRunning(1, NULL);
	}
	// Se vacía la cola si ya tenía datos
	else{
		while(msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSQ_TYPE, IPC_NOWAIT) != -1){}
	}
	
	// Siempre que no haya error iteramos
	while(isRunning(0, NULL)){
		
		// TODO: Quitar esta impresión (es para debugging)
		printf("\n Request to: %s\n", fullurl);
		
		// Reservar memoria para el objeto de respuesta
		if((resp_info = (response_info_t *)malloc(sizeof(response_info_t))) == NULL){
			printf("Poll: malloc failed\n");
		}
		// Guardar poll_info
		resp_info->poll_info = &poll_info;
		
		// Realizar la petición HTTP
		curl_easy_setopt(poll_info.http_info.curlhandle, CURLOPT_URL, fullurl);
		curl_easy_setopt(poll_info.http_info.curlhandle, CURLOPT_WRITEDATA, (void *)&resp_info->http_response);
		resp_info->http_response.response = NULL;
		resp_info->http_response.size = 0;
		res = curl_easy_perform(poll_info.http_info.curlhandle);
		
		// Analizar la respuesta
		// TODO: ¿Poner que tras x intentos fallidos se cierre el servidor o se notifique al usuario?
		if(res != CURLE_OK){
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		else{
			
			// Crear hilo para parsear con los parámetros correspondientes
			if(pthread_create(&thread, &attr, tbc_parser, resp_info) != 0){
				printf("Failed to create parse thread\n");
				isRunning(1, NULL);
			}
			else{
			
				// Esperar a que parser indique el offset del último mensaje (se almacena en la siguiente URL).
				// TODO: Hacer algo en la función de parse para que en caso de error devuelva el último offset leido o 0 (el offset 0 devuelve todos los updates si no recuerdo mal, pero habría que comprobarlo en la API de telegram). Podría devolver 0 en caso de que no haya updates y -1 en caso de error al parsear (puede haber varios reintentos).
				msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSQ_TYPE, 0);
				
				// Si hay que actualizar lo hacemos
				// TODO: Hacer que el intercambio de mensajes sea en formato int, no en caracteres
				if(msq_buffer.mtext[0] != '0'){
					offset = msq_buffer.mtext;
					strcpy(fullurl, url);
					strcat(fullurl, offset);
				}
				
			}
			
		}
		
	}
	
	//TODO: Añadir que mande una señal al proceso principal para que se entere del fallo.
	// Se borra la cola
	if(msgctl(msgqueue_id, IPC_RMID, NULL) != 0){
		printf("Error al cerrar la cola de mensajes\n");
	}
	// Se borra el manejador
	curl_easy_cleanup(poll_info.http_info.curlhandle);
	printf("Exiting pooling thread\n");
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  bot_info_t *bot_info: Información de la librería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error
**
**  Descripción:  Inicializa la función de polling.
*/
int tbc_polling_init(bot_info_t *bot_info){
	
	int ret = 0;	// Valor de retorno
	pthread_attr_t attr;
	pthread_t thread;
	
	// Se crea el hilo que se encargara del pooling
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &attr, tbc_poll, bot_info) != 0){
		ret = -1;
	}
	pthread_attr_destroy(&attr);
	
	return ret;
	
}

