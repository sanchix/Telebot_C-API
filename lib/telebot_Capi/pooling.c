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
/* Declaración de funciones locales. */

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
// TODO: poner comprobaciones de error
// TODO: pensar si es mejor reservar memoria y devolver el mensaje del tirón o que reserve memoria el llamante (esta última opción es como está ahora)
int unpack_json_message(message_t *message, json_t *message_obj){
	
	// TODO: Inicializar ret a -1 (suponer error) y cambiar a 0 en caso de que todo valla bien
	int ret = 0;
	json_t *aux;
	
	// TODO: Ver si hay alguna forma de detectar errores o evitar copiar cadenas más grandes del tamaño reservado (a ver si en strcpy y strcat hay algún parámetro "size" o tamaño máximo)
	
	// Desempaquetamos message_id
	message->message_id = json_integer_value(json_object_get(message_obj, "message_id"));
	
	// Desempaquetamos from
	aux = json_object_get(message_obj, "from");
	message->from.id = json_integer_value(json_object_get(aux, "id"));
	message->from.is_bot = json_boolean_value(json_object_get(aux, "is_bot"));
	strcpy(message->from.first_name, json_string_value(json_object_get(aux, "first_name")));
	strcpy(message->from.last_name, json_string_value(json_object_get(aux, "last_name")));
	strcpy(message->from.language_code, json_string_value(json_object_get(aux, "language_code")));
	
	// Desempaquetamos chat
	aux = json_object_get(message_obj, "chat");
	message->chat.id = json_integer_value(json_object_get(aux, "id"));
	strcpy(message->chat.first_name, json_string_value(json_object_get(aux, "first_name")));
	strcpy(message->chat.last_name, json_string_value(json_object_get(aux, "last_name")));
	strcpy(message->chat.type, json_string_value(json_object_get(aux, "type")));
	
	// Desempaquetamos date
	message->date = json_integer_value(json_object_get(message_obj, "date"));
	
	// Desempaquetamos text
	strcpy(message->text, json_string_value(json_object_get(message_obj, "text")));
	
	return ret;
	
}


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
void *parser(void *r_info){
	
	char response[MAX_RESP_TAM];
	key_t clave;
	int msgqueue_id;
	int update_id;
	struct msgbuf msq_buffer;
	message_t message;
	int found;
	updateHandler_t handler;
	response_info_t *resp_info;
	update_t update;
	
	json_t *json_root;
	json_error_t error;
	json_t *json_ok;
	json_t *json_result;
	json_t *json_update;
	json_t *json_aux;
	
	// Creamos la cola de mensajes para comunicarnos con pool
	clave = ftok("/home/dit/Documentos/telebot", 'p'); 
	//printf("%i", clave);
	msq_buffer.mtype = OFFSET_MSG_TYPE;
	if((msgqueue_id = msgget(clave, 0660)) == -1){
		printf("> Parser thread: error al acceder a la cola de mensajes: %s\n", strerror(errno));
	}
	
	else{
		// TODO: Pensar si hace falta almacenar en otra variable la respuesta. Quizás sea mejor pasarla de otra forma, por ejemplo que el pool haga un malloc y aquí hagamos un free cuando terminemos u otro sistema...
		resp_info = (response_info_t *)r_info;
		strcpy(response, resp_info->response);
		
		// TODO: Quitar (es para depuración)
		printf("\nUpdate:\n");
		
		// Realizamos el parse con la librería jansson
		json_root = json_loads(response, 0, &error);
		
		// Obtenemos el valor de ok
		// TODO: Hacer algo con esto, que notifique a alguien si no es ok o haga algo de provecho más que imprimirlo
		json_ok = json_object_get(json_root, "ok");
		if(json_is_true(json_ok)){
			printf("ok\n");
		}
		else{
			printf("Message received not ok\n");
		}
		
		// TODO: Implementar la búsqueda rápida del último update_id para comunicárselo a pool. Hacer mediciones de tiempo para ver si realmente merece la pena.
		
		// Analizamos el resultado (lista de objetos update)
		json_result = json_object_get(json_root, "result");
		for(size_t i = 0; i < json_array_size(json_result); i++){
			
			// Se obtiene el objeto update
			json_update = json_array_get(json_result, i);
			
			// Obtenemos update_id
			json_aux = json_object_get(json_update, "update_id");
			update_id = json_integer_value(json_aux);
			//TODO: quitar esta impresión (es debug)
			printf("Update id: %i\n", update_id);
			
			// Obtenemos el objeto message y lo desempaquetamos
			// TODO: Pensar quien tiene que reservar memoria para este objeto y como vamos a pasarlo.
			// TODO: Poner en los if los distintos objetos de respuesta que se pueden recibir.
			// TODO: Poner un if (no tiene porque ser un mensaje)
			json_aux = json_object_get(json_update, "message");
			unpack_json_message(&message, json_aux);
			
			// TODO: Pasar el mensaje al usuario (notificarle) en vez de imprimirlo. Pensar si se va a hacer por paso de mensajes o mediante un puntero a una función que esta función (parser) debe ejecutar. En este caso, esa función se encontrará en el fichero del bot que haya programado el usuario de la librería y llegará hasta aquí mediante parámetros (es algo parecido a lo que se hace en python con los eventos).
			printf("From: %s %s\n", message.from.first_name, message.from.last_name);
			printf("Text: %s\n", message.text);
			
			// TODO: Analizar eventos y llamar a la función
			// TODO: Poner exclusión mutua
			// bajar mutex
			found = 0;
			for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
				if(resp_info->poll_info->updateEvents[i].condition != CONDITION_UNASSIGNED){
					printf("> FOUND HANDLER\n");
					handler = resp_info->poll_info->updateEvents[i].handler;
					// TODO: Cambiar para que no sea puntero a NULL, sino que según lo que devuelva la función se actualize la cola de una manera o de otra (se tomen los mensajes como leidos o no, por ejemplo).
					if(handler != NULL){
						// TODO: Cambiar NULL por puntero a la estructura message de respuesta
						update.type = UPDATE_MESSAGE;
						update.content = (void *)(&message);
						update.bot_info = &(resp_info->poll_info->bot_info);
						handler(&update);
					}
					else{
						// TODO: Don't update queue
						printf("> DONT UPDATE QUEUE\n");
					}
				}
			}
			if(!found){
				printf("> USING DEFAULT HANDLER\n");
				handler = resp_info->poll_info->updateEvents[0].handler;
				// TODO: Cambiar para que no sea puntero a NULL, sino que según lo que devuelva la función se actualize la cola de una manera o de otra (se tomen los mensajes como leidos o no, por ejemplo).
					if(handler != NULL){
						// TODO: Cambiar NULL por puntero a la estructura message de respuesta
						update.type = UPDATE_MESSAGE;
						update.content = (void *)(&message);
						update.bot_info = &(resp_info->poll_info->bot_info);
						handler(&update);
					}
					else{
						// TODO: Don't update queue
						printf("> DONT UPDATE QUEUE\n");
					}
			}
			// subir mutex
			
		}
		
		// TODO: Quitar este sleep (es debug)
		sleep(1);
		
		// TODO: mandar aquí el último update_id. Realmente esto lo quitaremos cuando hagamos la búsqueda anticipada (antes del bucle)
		strcpy(msq_buffer.mtext, "-1");
		msgsnd(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, 0);
		
	}
	
	// TODO: ¿Cerrar la cola y liberar otros recursos?
	free(r_info);
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  char *tok: Token del BOT.
**				  bot_info_t *bot_info: Puntero a un tipo bot_info_t, que almacenará información para la comunicación https con la api de Telegram para el BOT específico.
**                
**     Devuelve:  int: 0 si la inicializción se ha completado con éxito, -1 en caso de error.
**
**  Descripción:  Inicializa las funciones de la librería.
*/
// TODO: Comentar
// TODO: Cambiar por poll
void *pool(void *info){
	
	int status;
	int error = 0;
	char url[MAX_URL_TAM];
	char fullurl[MAX_URL_TAM];
	char *offset = "0";
	char response[MAX_RESP_TAM];
	useconds_t interval = 1000000;
	pthread_attr_t attr;
	pthread_t thread;
	key_t clave;
	int msgqueue_id;
	struct msgbuf msq_buffer;
	response_info_t *resp_info;
	
	// Prepare URL to get updates
	poll_info_t *poll_info = (poll_info_t *)info;
	strcpy(url, poll_info->bot_info.url); 
	strcat(url, "/getUpdates?offset=");
	strcpy(fullurl, url);
	strcat(fullurl, offset);
	
	// Prepare variables to create parser threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// Prepare message queue to communicate with parser threads
	clave = ftok("/home/dit/Documentos/telebot",'p');
	// TODO: Poner este printf en un IFDEF o algo, es util para el usuario final. Tambien se podría crear una función para obtener el token.
	// TODO: Hay que hacer una función para cerrar los hilos y parar el pooling (cerrar también las colas)
	printf("clave: %i\n", clave);
	if((msgqueue_id = msgget(clave,IPC_CREAT|IPC_EXCL|0660)) == -1){
		printf("> Pooling thread: error al iniciar la cola de mensajes: %s\n", strerror(errno));
		error = 1;
	}
	
	// Siempre que no haya error iteramos
	while(!error){
		
		// Esperamos un tiempo para no sobrecargar la red
		// TODO: Cambiar por esperar x segundos entre iteración, no siempre x segundos en esta linea
		// TODO: Variar la frecuencia de pooling dinámicamente
		usleep(interval);
		
		// TODO: Quitar esta impresión (es para debugging)
		printf("URL: %s\n", fullurl);
		
		// Realizamos la petición HTTP
		status = http_get(&(poll_info->bot_info.hi), fullurl, response, MAX_RESP_TAM);
		
		// Analizamos la respuesta
		// TODO: ¿Poner que tras x intentos fallidos se cierre el servidor o se notifique al usuario?
		if(status != 200){
			printf("Failed to update data\n");
		}
		
		// TODO: Hay que reservar memoria para resp_info
		else{
			
			// Creamos el hilo que se encargará de parsear con los parámetros correspondientes
			// TODO: Hacer que no se reserver siempre para el máximo, sino que sea variable (hay que reservar tamaño para la estructura y el string todo en el mismo malloc (para que sea más rápido) y luego organizar la memoria a nuestra bola (hay que hacer que el punero a char de la estructura apunte a la zona del malloc que hemos dejado para el string.
			if((resp_info = (response_info_t *)malloc(sizeof(response_info_t))) == NULL){
				printf("Poll: malloc failed\n");
			}
			strcpy(resp_info->response, response);
			resp_info->poll_info = poll_info;
			
			if(pthread_create(&thread, &attr, parser, resp_info) != 0){
				printf("Failed to create parse thread\n");
				error = 1;
			}
			else{
			
				// Esperamos a que el parser nos indique el offset del último mensaje y lo almacenamos en la siguiente URL.
				// TODO: Hacer algo en la función de parse para que en caso de error devuelva el último offset o 0 (el offset 0 devuelve todos los examenes si no recuerdo mal, pero habría que comprobarlo en la API de telegram).
				msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSG_TYPE, 0);
				offset = msq_buffer.mtext;
				strcpy(fullurl, url);
				strcat(fullurl, offset);
				
			}
			
		}
		
	}
	
	//TODO: Añadir que mande una señal al proceso principal para que se entere del fallo.
	
	pthread_exit(NULL);
	
}


/*
**   Parámetros:  bot_info_t *bot_info: Puntero a una variable bot_info_t con información HTTPS del bot.
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso
**
**  Descripción:  Inicializa la función de pooling.
*/
int tbc_pooling_init(poll_info_t *poll_info){
	
	// TODO: Hacer que el valor de retorno suponga error (-1) y se ponga a 0 si todo va bien
	int ret = 0;	// Valor de retorno
	pthread_attr_t attr;
	pthread_t thread;
	
	// Se crea el hilo que se encargara del pooling
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &attr, pool, poll_info) != 0)
		ret = -1;
	pthread_attr_destroy(&attr);
	
	return ret;
	
}

