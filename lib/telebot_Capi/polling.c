/*
**     Fichero:  telebot_Capi/pooling.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Parte de la libreria telebot_Capi encargada de obtener actualizaciones mediante el método de "long pooling".
*/

/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definiciones de funciones locales. */

/*
**   Parámetros:  message_t * message: Estructura de tipo mensaje para guardar los parametros mas importantes del json recibido.
**                json_t * message_obj: Objeto json completo a procesar.
**                
**     Devuelve:  int: 0 si éxito(el objeto se ha procesado correctamente), -1 en caso de error.
**
**  Descripción:  Función que a partir de un objeto json nos lo formatea en un objeto message_t con nuestros valores a recoger.
*/
// DONE: pensar si es mejor reservar memoria y devolver el mensaje del tirón o que reserve memoria el llamante (esta última opción es como está ahora)
int unpack_json_message(message_t *message, json_t *message_obj){
	
	// DONE: Inicializar ret a -1 (suponer error) y cambiar a 0 en caso de que todo vaya bien
	int ret = 0;
	json_t *aux;
	json_t *aux2;
	
	// TODO: Ver si hay alguna forma de detectar errores o evitar copiar cadenas más grandes del tamaño reservado (a ver si en strcpy y strcat hay algún parámetro "size" o tamaño máximo) PD: NO TE RAYE
	
	// TODO: Poner opcionales las cosas opcionales (rellenar con null). Hay que hacer que el resto de objetos que usen message_t se esperen que haya cosas NULL.
	
	// Desempaquetamos message_id
	message->message_id = json_integer_value(json_object_get(message_obj, "message_id"));
	
	// Desempaquetamos from (si hay, pues es opcional)
	if ((aux = json_object_get(message_obj, "from"))!=NULL){
		
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
	if((aux = json_object_get(message_obj, "text"))!=NULL){ //Comprobamos si no hay error en la recogida de text.
		// TODO: Reservar memo para text
		if((message->text = (char *)malloc(MAX_RESP_TAM)) == NULL){
				printf("Parse: malloc failed\n");
		}
		strcpy(message->text, json_string_value(aux));
		
		if(message->text[0] == '/'){
			sscanf(message->text, "/%s ", message->command);
			// TODO: Analizar los argumentos (scanf(tal, " %s", ...)
		}
		else{
			// TODO: Inicializar todo a NULL y cambiar luego.
			message->command = NULL;
		}
		
	}
	else{
		// TODO: Reservar memo
		message->text = NULL;
		message->command = NULL;
		message->args = NULL;
	}
	  
	return ret;
	
}


/*
**   Parámetros:  Por definición a las funciones pasadas a hilos => void * r_info Informacion de la respuesta dada por el bot 
**               
**                ...
**     Devuelve:  void* Por definición a las funciones pasadas a hilos
**
**  Descripción:  Parsea la respuesta recibida del bot
*/
// TODO: Comentar
void *parser(void *r_info){
	
	//char response[MAX_RESP_TAM];
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
		// Preparamos resp_info para evitar usar el cast en el código
		resp_info = (response_info_t *)r_info;
		
		// Preparamos los semáforos
		mutex_updateNotifiers = resp_info->bot_info->mutex_updateNotifiers;
		
		// TODO: Quitar (es para depuración).
		printf("\nUpdate:\n");
		
		// Realizamos el parse con la librería jansson
		json_root = json_loads(resp_info->response, 0, &error);
		
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
			update.http_info = &(resp_info->bot_info->http_info);
		
			// Analizamos el resultado (lista de objetos update)
			json_result = json_object_get(json_root, "result");
			// TODO: Quitar este printf (debug)
			//printf("JSON: %s", json_string_value(json_result));
			// Para cada objeto del array
			
			for(size_t i = 0; i < json_array_size(json_result); i++){
				
				// Se obtiene el objeto update para la iteración
				json_update = json_array_get(json_result, i);
				
				// Obtenemos update_id
				json_aux = json_object_get(json_update, "update_id");
				update_id = json_integer_value(json_aux);
				//TODO: quitar esta impresión (es debug)
				printf("\tUpdate id: %lld\n", update_id);
				
				
				// Procedemos a procesar el update
				
				// TODO: Poner en los if los distintos objetos de respuesta que se pueden recibir.
				// Si lo que se ha recibido es un mensaje...
				if((json_aux = json_object_get(json_update, "message")) != NULL){
					// Obtenemos el objeto message y lo desempaquetamos
					// TODO: Hay que liberar
					if((update.content = (message_t *)malloc(sizeof(message_t))) == NULL){
						printf("Poll: malloc failed\n");
					}
					unpack_json_message(update.content, json_aux);
					update.type = UPDATE_MESSAGE;
					
				}
				
				// Si lo que se ha recibido es una actualización de encuesta
				else if((json_aux = json_object_get(json_update, "poll")) != NULL){
					// Obtenemos el resultado de la encuesta
					// TODO: Hacer
				  
					update.type = UPDATE_POLL;
					printf("Received poll update\n");
				}
				else{
					update.type = UPDATE_NONE;
					printf("Something received\n");
				}
				
				//TODO: Que deberia de ocurrir en el caso de que se produjera un error?				
				
				
				// Determinar el manejador para el evento
				
				// Se entra en la región compartida
				sem_wait(mutex_updateNotifiers);
				
				// Se busca handle filtrando primero por tipo de update
				
				if ((handle = findUpdateHandler(update, resp_info->bot_info->notifiers)) != NULL){
					printf("> FOUND HANDLER\n");
					// TODO: Cambiar para que no sea puntero a NULL, sino que según lo que devuelva la función se actualize la cola de una manera o de otra (se tomen los mensajes como leidos o no, por ejemplo).
					handle(&update);
				}
				
				// If no handle was found, use default handle
				else{
					printf("> USING DEFAULT HANDLER\n");
					handle = resp_info->bot_info->notifiers[0].handle;
					// TODO: Cambiar para que no sea puntero a NULL, sino que según lo que devuelva la función se actualize la cola de una manera o de otra (se tomen los mensajes como leidos o no, por ejemplo).
					if(handle != NULL){
						// TODO: Cambiar NULL por puntero a la estructura message de respuesta
						handle(&update);
					}
					else{
						// TODO: Don't update queue
						printf("> DONT UPDATE QUEUE\n");
					}
				}
				//TODO: Que deberia de ocurrir en el caso de que se produjera un error?
				// Se sale de la región compartida
				sem_post(mutex_updateNotifiers);
				
				if(update.type == UPDATE_MESSAGE){
					free(update.content);
					if(((message_t *)update.content)->text != NULL){
						free(((message_t *)update.content)->text);
					}
				}
			
			}
			
			// TODO: Quitar este sleep (es debug)
			//sleep(1);
			sprintf(msq_buffer.mtext, "%lld", (update_id != 0)?(update_id + 1):0);
			msgsnd(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, 0);
		
		}
	}
	
	// TODO: ¿Cerrar la cola y liberar otros recursos?
	free(r_info);
	pthread_exit(NULL);
	
}


// TODO: Comentar
void *poll(void *info){
	
	// TODO: Ordenar
	int status;
	int error = 0;
	char url[MAX_URL_TAM];
	char fullurl[MAX_URL_TAM];
	char *offset = "0";
	char response[MAX_RESP_TAM];
	useconds_t interval = 1000;
	pthread_attr_t attr;
	pthread_t thread;
	key_t clave;
	int msgqueue_id;
	struct msgbuf msq_buffer;
	response_info_t *resp_info;
	bot_info_t *bot_info;
	
	// Prepare URL to get updates
	bot_info = (bot_info_t *)info;
	strcpy(url, bot_info->http_info.url); 
	strcat(url, "/getUpdates?offset=");
	// Prepare first url to get updates (offset=0)
	strcpy(fullurl, url);
	strcat(fullurl, offset);
	
	// Prepare variables to create parser threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// Prepare message queue to communicate with parser threads
	clave = ftok(".",'p');
	// TODO: Poner este printf en un IFDEF o algo, es util para el usuario final. Tambien se podría crear una función para obtener el token.
	//printf("clave: %i\n", clave);
	// TODO: Hay que hacer una función para cerrar los hilos y parar el pooling (cerrar también las colas)
	if((msgqueue_id = msgget(clave,IPC_CREAT|0660)) == -1){
		printf("> Pooling thread: error al iniciar la cola de mensajes: %s\n", strerror(errno));
		error = 1;
	}
	// Se vacía la cola.
	else{
		while(msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSQ_TYPE, IPC_NOWAIT) != -1){}
	}
	
	// Siempre que no haya error iteramos
	while(!error){
		
		// Esperamos un tiempo para no sobrecargar la red
		// TODO: Cambiar por esperar x segundos entre iteración, no siempre x segundos en esta linea
		// TODO: Variar la frecuencia de pooling dinámicamente
		usleep(interval);
		
		// TODO: Quitar esta impresión (es para debugging)
		printf("\n Request to: %s\n", fullurl);
		
		// Realizamos la petición HTTP
		status = http_get(&(bot_info->http_info.hi), fullurl, response, MAX_RESP_TAM);
		// Analizamos la respuesta
		// TODO: ¿Poner que tras x intentos fallidos se cierre el servidor o se notifique al usuario?
		if(status != 200){
			printf("Failed to update data: %i, %s\n", status, response);
		}
		
		// TODO: Hay que reservar memoria para resp_info
		else{
			
			// Creamos el hilo que se encargará de parsear con los parámetros correspondientes
			// TODO: Hacer que no se reserve siempre para el máximo, sino que sea variable (hay que reservar tamaño para la estructura y el string todo en el mismo malloc (para que sea más rápido) y luego organizar la memoria a nuestra bola (hay que hacer que el punero a char de la estructura apunte a la zona del malloc que hemos dejado para el string.
			if((resp_info = (response_info_t *)malloc(sizeof(response_info_t))) == NULL){
				printf("Poll: malloc failed\n");
			}
			strcpy(resp_info->response, response);
			resp_info->bot_info = bot_info;
			
			if(pthread_create(&thread, &attr, parser, resp_info) != 0){
				printf("Failed to create parse thread\n");
				error = 1;
			}
			else{
			
				// Esperamos a que el parser nos indique el offset del último mensaje y lo almacenamos en la siguiente URL.
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
	
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  http_info_t *http_info: Puntero a una variable http_info_t con información HTTPS del bot.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso
**
**  Descripción:  Inicializa la función de polling.
*/
int polling_init(bot_info_t *bot_info){
	
	int ret = 0;	// Valor de retorno
	pthread_attr_t attr;
	pthread_t thread;
	
	// Se crea el hilo que se encargara del pooling
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &attr, poll, bot_info) != 0){
		ret = -1;
	}
	pthread_attr_destroy(&attr);
	
	return ret;
	
}

