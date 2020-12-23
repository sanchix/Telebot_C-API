/*
**     Fichero:  testbot/testbot.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Implementación de un BOT para la aplicación Telegram(R) utilizando la librería "telebot_Capi" con el objetivo de probar la funcionalidad de esta última.
*/

#include <stdio.h>
#include <unistd.h>
#include "telebot_Capi/telebot_Capi.h"


int doEcho(update_t *update){
	
	message_t *message;
	char cid[20];
	
	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("Mensaje recibido de %s %s: %s\n", message->from.first_name, message->from.last_name, message->text);
		}
		else{
			printf("Mensaje recibido de alguien: %s\n", message->text);
		}
		
		// Se hace eco si se ha recibido texto
		if(message->text != NULL){
			printf("Haciendo eco...\n");
			sprintf(cid, "%i", message->chat.id);
			telebot_sendMessage(cid, message->text, update->http_info);
		}
		
	}
	
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("Algo recibido: ...\n");
	}
	
	// TODO: Pensar que poner en el retorno
	return POLL_DROP;
	
}


int main(int argc, char* argv[]){
	
	bot_info_t bot_info;
	event_t event;
	
	telebot_close();
	
	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	
	// Iniciamos las funciones de librería en bot_info
	else if(telebot_init(argv[1], &bot_info) != 0){
			printf("Error al iniciar https\n");
	}
	
	// Si todo va correcto procedemos a realizar pruebas:
	else{
		
		printf("Initialized\n");
		
		char pregunta[]= "¿Funcionara?";
		char *opciones[20]= {"SI","NO","OBERSERVAD",NULL};
	
		telebot_sendPoll("166103691",pregunta,opciones, &bot_info.http_info);


		// Configuramos el handle imprime:
		// evento -> EVENT_DEFFAULT = Comportamiento por defecto
		// handle -> imprimeMensaje
		event.update_type = UPDATE_ANY;
		strcpy(event.command, "");
		addUpdateNotifier(doEcho, event, &bot_info);
		
		// TODO: Comprobar que funcionan los semáforos
		
		// Loop to keep alive main thread
		while(1){
			sleep(15);
		}

		
		if (telebot_close() == -1){
			printf("Ha habido un problema al cerrar el bot");
		}

	}
	
	return 0; 
	
}