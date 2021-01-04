/*
**     Fichero:  telebot_Capi/pooling.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  04/enero/2021
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
		if((message->text = (char *)malloc(sizeof(char)*strlen(st_aux))) == NULL){
			printf("\033[1;31m");
			printf("####################################################\n");
			printf("Parse>unpack_json_message: text malloc failed\n");
			printf("####################################################\n");
			printf("\033[0m");
				
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
				if((message->command = (char *)malloc(tam*sizeof(char))) == NULL){
					printf("\033[1;31m");
					printf("####################################################\n");
					printf("Parse>unpack_json_message: command malloc failed\n");
					printf("####################################################\n");
					printf("\033[0m");
				}
				sscanf(message->text, "/%s ", message->command);
			}
			else{
				message->command = NULL;
			}
		}
		else{
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
int unpack_json_poll_update(poll_update_t *poll, json_t *message_obj){
	
	int ret = 0;
	json_t *json_opciones;
	json_t *json_opcion;
	int tam_preg;
	
	// Desempaquetamos id
	poll->poll_id = json_integer_value(json_object_get(message_obj, "id"));
	tam_preg = strlen(json_string_value(json_object_get(message_obj, "question")));
	if ((poll->question = (char *)malloc(tam_preg*sizeof(char))) != NULL){
	
		strcpy(poll->question,json_string_value(json_object_get(message_obj, "question")));
		json_opciones = json_object_get (message_obj,"options");
		
		if ((poll->options = (option_t *)malloc(sizeof(option_t)*(json_array_size(json_opciones) + 1))) != NULL){
		  
		  	for(size_t i = 0; i < json_array_size(json_opciones); i++){
			    
				json_opcion = json_array_get(json_opciones, i);
				int tam_text = strlen (json_string_value(json_object_get(json_opcion,"text")));
				
				if ((poll->options[i].text = (char *)malloc(sizeof(char)*tam_text))!=NULL){
					strcpy(poll->options[i].text, json_string_value(json_object_get(json_opcion,"text")));
					poll->options[i].opcion_votos = json_integer_value(json_object_get(json_opcion,"voter_count"));
				}else{
					printf("> Parse: options[%lu].text malloc failed\n",i);
				}
				
			}
			poll->options[json_array_size(json_opciones)].text = NULL;
		  
		} else{
		  	printf("> Parse: options malloc failed\n");	  				    	  
		}
		
		poll->total_votos = json_integer_value(json_object_get(message_obj, "total_voter_count"));

	}
	else{
		printf("> Parse: question malloc failed\n");	
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
	long long int update_id = 0;
	struct msgbuf msq_buffer;
	updateHandle_t handle;
	response_info_t *resp_info;
	update_t update;

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
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("Parser thread>*tbc_parser: error al acceder a la cola de mensajes: %s\n", strerror(errno));
		printf("####################################################\n");
		printf("\033[0m");
	}
	
	else{
		// Peparar resp_info (evita usar el cast, comididad)
		resp_info = (response_info_t *)info;
		
		printf("##########################-tbc_parser-##########################\n");
		printf("#   Update:\n");
		
		// Realizar el parse con la librería jansson
		json_root = json_loads(resp_info->http_response.response, 0, &error);
		
		// Obtenemos el valor de ok
		json_ok = json_object_get(json_root, "ok");

		if(json_is_false(json_ok)){
			printf("\033[1;31m");
			printf("*tbc_parser: Message received not ok: %s (error code: %lld)\n",json_string_value(json_object_get(json_root,"description")),json_integer_value(json_object_get(json_root,"error_code")));
			printf("\033[0m");
		 
		}
		
		// Si todo va bien
		else{
			printf("#\tok\n");
		
			update.http_info = &(resp_info->poll_info->http_info);
		
			// Analizamos el resultado (lista de objetos update)
			json_result = json_object_get(json_root, "result");

			// Para cada objeto del array
			for(size_t i = 0; i < json_array_size(json_result); i++){
				
				// Se obtiene el objeto update
				json_update = json_array_get(json_result, i);
				
				// Obtenemos update_id
				json_aux = json_object_get(json_update, "update_id");
				update_id = json_integer_value(json_aux);
				printf("#\tUpdate id: %lld\n", update_id);
				
				/* --- Procesar el update --- */
				// Si lo que se ha recibido es un mensaje...
				if((json_aux = json_object_get(json_update, "message")) != NULL){
					// Obtenemos el objeto message y lo desempaquetamos
					update.type = UPDATE_MESSAGE;
					if((update.content = (message_t *)malloc(sizeof(message_t))) == NULL){
						printf("\033[1;31m");
						printf("####################################################\n");
						printf("Poll>*tbc_parser: malloc failed\n");
						printf("####################################################\n");
						printf("\033[0m");
						
					}
					
					unpack_json_message(update.content, json_aux);
					
				}
				
				// Si lo que se ha recibido es una actualización de encuesta...
				else if((json_aux = json_object_get(json_update, "poll")) != NULL){
			       	// Obtenemos el resultado de la encuesta
				 	if((update.content = (poll_update_t *)malloc(sizeof(poll_update_t)))==NULL){
						printf("\033[1;31m");
						printf("####################################################\n");
						printf("Poll>*tbc_parser: poll update malloc failed\n");
						printf("####################################################\n");
						printf("\033[0m");
					}

					unpack_json_poll_update(update.content,json_aux);
					update.type = UPDATE_POLL;
					printf("#   Received poll update\n");
					printf("####################################################\n");
				}
				else{
					update.type = UPDATE_UNKNOWN;
					printf("#   Something received\n");
					printf("####################################################\n");
				}

				// Se obtiene el handle a utilizar
				handle = findUpdateHandler(&update, resp_info->poll_info->notifiers_info);
							
				handle(&update);

				// Se liberan recursos
				if(update.type == UPDATE_MESSAGE){
					if(((message_t *)update.content)->text != NULL){
						free(((message_t *)update.content)->text);
					}
					if(((message_t *)update.content)->command != NULL){
						free(((message_t *)update.content)->command);
					}
					free(update.content);
					
				} else if (update.type == UPDATE_POLL) {

				  	if(((poll_update_t *)update.content)->question != NULL){
						free(((poll_update_t *)update.content)->question);
					}
					for (int i=0; ((poll_update_t *)update.content)->options[i].text != NULL; i++){
						free(((poll_update_t *)update.content)->options[i].text);					  
					}
					
					if(((poll_update_t *)update.content)->options != NULL){
						free(((poll_update_t *)update.content)->options);
					}
					free(update.content);
				}
			
			}
			
			sprintf(msq_buffer.mtext, "%lld", (update_id != 0)?(update_id + 1):0);
			msgsnd(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, 0);
		
		}
	}
	
	free(info);
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  void *info: Puntero a bot_info_t con información de la librería.
**
**  Descripción:  Realiza la función de polling, llamando a tbc_parser() cuando hay nuevas actualizaciones.
*/
void *tbc_poll(void *info){
	
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

	if((poll_info.http_info.curlhandle = curl_easy_duphandle(((bot_info_t *)info)->http_info.curlhandle)) == NULL){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("polling()>*tbc_poll: Failed to duplicate curl handle\n");
		printf("####################################################\n");
		printf("\033[0m");	
		
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
	if((msgqueue_id = msgget(clave,IPC_CREAT|0660)) == -1){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("Pooling thread>*tbc_poll: error al iniciar la cola de mensajes: %s\n", strerror(errno));
		printf("####################################################\n");
		printf("\033[0m");	
		isRunning(1, NULL);
	}
	// Se vacía la cola si ya tenía datos
	else{
		while(msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSQ_TYPE, IPC_NOWAIT) != -1){}
	}
	
	// Siempre que no haya error iteramos
	while(isRunning(0, NULL)){
		
		printf("##########################-tbc_poll-##########################\n");
		printf("#   Request to: %s\n", fullurl);
		
		// Reservar memoria para el objeto de respuesta
		if((resp_info = (response_info_t *)malloc(sizeof(response_info_t))) == NULL){
			printf("\033[1;31m");
			printf("####################################################\n");
			printf("Poll>*tbc_poll: malloc failed\n");
			printf("####################################################\n");
			printf("\033[0m");
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
		if(res != CURLE_OK){
			printf("\033[1;31m");
			printf("####################################################\n");
			printf("*tbc_poll: curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			printf("####################################################\n");
			printf("\033[0m");
		}
		else{
			
			// Crear hilo para parsear con los parámetros correspondientes
			if(pthread_create(&thread, &attr, tbc_parser, resp_info) != 0){
				printf("\033[1;31m");
				printf("####################################################\n");
				printf("*tbc_poll: Failed to create parse thread\n");
				printf("####################################################\n");
				printf("\033[0m");
				isRunning(1, NULL);
			}
			else{
			
				// Esperar a que parser indique el offset del último mensaje (se almacena en la siguiente URL).
				msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSQ_TYPE, 0);
				
				// Si hay que actualizar lo hacemos
				if(msq_buffer.mtext[0] != '0'){
					offset = msq_buffer.mtext;
					strcpy(fullurl, url);
					strcat(fullurl, offset);
				}
				
			}
			
		}
		
	}
	
	// Se borra la cola
	if(msgctl(msgqueue_id, IPC_RMID, NULL) != 0){
		printf("\033[1;31m");
		printf("####################################################\n");
		printf("*tbc_poll: Error al cerrar la cola de mensajes\n");
		printf("####################################################\n");
		printf("\033[0m");
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

