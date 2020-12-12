/*
**     Fichero:  telebot_Capi/pooling.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Parte de la libreria telebot_Capi encargada de obtener actualizaciones mediante el método de "long pooling".
*/

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include "https_lib/https.h"
#include "telebot_Capi.h"

#include <errno.h>

#define MAX_RESP_TAM 4096
#define MAX_OFFSET_TAM 20
#define OFFSET_MSG_TYPE 1


struct msgbuf{
	long mtype;
	char mtext[MAX_OFFSET_TAM];
};


// ADD a method 
void *parser(void *resp){
	
	char response[MAX_RESP_TAM];
	key_t clave;
	int msgqueue_id;
	struct msgbuf msq_buffer;
	
	clave = ftok(".", 'p');  //Equivalent to 1882193940
	msq_buffer.mtype = OFFSET_MSG_TYPE;
	if((msgqueue_id = msgget(clave, 0660)) == -1){
		printf("> Parser thread: error al acceder a la cola de mensajes: %s\n", strerror(errno));
	}
	else{
		
		strcpy(response, (char *)resp);
		printf("Updates: %s\n", response);
		
		/*	--------  CODE  --------  */
		
		//Function content in json.h
		analizar_objeto(jsmn_init;jsmn_parse) -> tokens
		
		//Buscamos el valor de ok y result)
		valido = json_primeFromObj(key="ok",objeto=tokens)
		lista_updates = json_listFromObj(key="result",objeto=tokens)
		
		//Fast last_update_id search
		
		for(i = 0; i < list_size(lista_updates); i++){
			update = json_objFromList(index=i,lista=lista_updates)
			
			update_id = json_primeFromObj(key="update_id",object=update)
			send2pool(update_id)
			
			struct message = json_smsgFromObj(key="message",objet=update)
			sent2client(struct message)
			
		}
		
		/*	--------  CODE  --------  */
		
		/*
		sleep(1);
		strcpy(msq_buffer.mtext, "-1");
		msgsnd(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, 0);
		*/
		
	}
	
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
void *pool(void *info){
	
	int status;
	int error = 0;
	char url[250];
	char fullurl[250];
	char *offset = "0";
	char response[MAX_RESP_TAM];
	useconds_t interval = 1000000;
	pthread_attr_t attr;
	pthread_t thread;
	key_t clave;
	int msgqueue_id;
	struct msgbuf msq_buffer;
	
	// Prepare URL to get updates
	bot_info_t *bot_info = (bot_info_t *)info;
	strcpy(url, bot_info->url); 
	strcat(url, "/getUpdates?offset=");
	
	// Prepare variables to create parser threads
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	// Prepare message queue to communicate with parser threads
	clave = ftok(".",'p');
	if((msgqueue_id = msgget(clave,IPC_CREAT|IPC_EXCL|0660)) == -1){
		printf("> Pooling thread: error al iniciar la cola de mensajes: %s\n", strerror(errno));
		error = 1;
	}
	
	while(!error){
		
		// TODO: Cambiar por esperar x segundos entre iteración, no siempre x segundos en esta linea
		// TODO: Variar la frecuencia de pooling dinámicamente
		usleep(interval);
		strcpy(fullurl, url);
		strcat(fullurl, offset);
		printf("URL: %s\n", fullurl);
		status = http_get(&(bot_info->hi), fullurl, response, MAX_RESP_TAM);
		
		if(status != 200){
			printf("Failed to update data\n");
		}
		else{
			if(pthread_create(&thread, &attr, parser, response) != 0){
				printf("Failed to create parse thread\n");
				error = 1;
				break;
			}
			
			// Esperamos a que el parser nos indique el offset del último mensaje.
			msgrcv(msgqueue_id, &msq_buffer, MAX_OFFSET_TAM, OFFSET_MSG_TYPE, 0);
			offset = msq_buffer.mtext;
			
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
int tbc_pooling_init(bot_info_t *bot_info){
	
	int ret = 0;	// Valor de retorno
	pthread_attr_t attr;
	pthread_t thread;
	
	// Se crea un hilo que se encargara del pooling (void *pool())
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if(pthread_create(&thread, &attr, pool, bot_info) != 0)
		ret = -1;
	
	pthread_attr_destroy(&attr);
	
	return ret;
	
}

