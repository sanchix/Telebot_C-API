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

void doSurvey (update_t *update){
  
	poll_update_t *survey;

	if (update->type == UPDATE_MESSAGE){
		survey = (poll_update_t *)update->content;
		if (survey->question != NULL && survey->options != NULL){
			printf("Pregunta de la encuesta: %s\n\n",survey->question);
			for (int i=0; survey->options[i].text !=NULL;i++){
			  printf ("\tOpcion %d: %s, Votos: %d\n",i,survey->options[i].text,survey->options[i].opcion_votos);
			}
			printf ("Total de votos recogidos: %d\n\n",survey->total_votos);
		}
	}

}
void felicita(update_t *update){
	
	message_t *message;
	char cid[20];
	char text[30] = "Feliz navidad ";
	
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
			printf("Felicitando...\n");
			sprintf(cid, "%i", message->chat.id);
			strcat(text, message->from.first_name);
			telebot_sendMessage(cid, text, update->http_info);
		}
		
	}
	
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("Algo recibido: ...\n");
	}
	
}

void doEcho(update_t *update){
	
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
	
}


int main(int argc, char* argv[]){
	
	bot_info_t bot_info;
	event_t event;
	
	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	
	// TODO: Configurar los handle antes
	// Iniciamos las funciones de librería en bot_info
	else if(telebot_init(argv[1], &bot_info) != 0){
			printf("Error al iniciar telebot_Capi\n");
	}
	
	// Si todo va correcto procedemos a realizar pruebas:
	else{
		
		printf("Initialized\n");
		
		char pregunta[]= "¿Funcionara?";
		char *opciones[20]= {"SI","NO","OBERSERVAD",NULL};
		
		//telebot_sendPoll("166103691",pregunta,opciones, &bot_info.http_info);
		telebot_sendPoll("150848014",pregunta,opciones, &bot_info.http_info);
		//telebot_sendMessage("150848014", "Hola", &bot_info.http_info);
		

		// Configuramos el handle imprime:
		// evento -> EVENT_DEFFAULT = Comportamiento por defecto
		// handle -> imprimeMensaje
		event.update_type = UPDATE_ANY;
		if(addUpdateNotifier(doEcho, &event, &bot_info) != 0){
			printf("Fallo al añadir handler doEcho\n");
		}
		
		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "start");
		if(addUpdateNotifier(felicita, &event, &bot_info) != 0){
			printf("Fallo al añadir handler felicita\n");
		}

		event.update_type = UPDATE_POLL;
		if(addUpdateNotifier(doSurvey, &event, &bot_info) != 0){
			printf("Fallo al añadir handler de la encuesta\n");
		}
		// TODO: Comprobar que funcionan los semáforos
		
		// Loop to keep alive main thread
		while(1){
			sleep(1);
		}

	}
	
	return 0; 
	
}
