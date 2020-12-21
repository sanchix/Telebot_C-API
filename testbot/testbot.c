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

void *imprimeMensaje(update_t *update){
	message_t *message;
	char cid[20];
	if(update->type == UPDATE_MESSAGE){
		message = (message_t *)update->content;
		printf("Mensaje recibido de %s: %s\n", message->from.first_name, message->text);
		printf("Haciendo eco...\n");
		sprintf(cid, "%i", message->chat.id);
		telebot_sendMessage(cid, message->text, update->bot_info);
	}
	else{
		printf("Algo recibido: ...\n");
	}
	return NULL;
}

int main(int argc, char* argv[]){
	
	char info[4096] = {};
	poll_info_t poll_info;
	event_t imprime;
	
	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	else if(telebot_init(argv[1], &poll_info) != 0){
			printf("Error al iniciar https\n");
	}else{

		char texto[] = "Hola";
		char id[] = "150848014";
		sleep(2);
		printf("Initialized\n");
		imprime.condition = CONDITION_DEFFAULT;
		imprime.handler = imprimeMensaje;
		addUpdateEvent(poll_info.updateEvents, imprime);
		sleep(2);
		
		//telebot_getMe(info, sizeof(info), &bot_info);
		//printf("Enviando un mensaje:\n");
		//telebot_sendMessage(id,texto, &bot_info);
		
		while(1){
			sleep(15);
		}

		int x = telebot_close();
		if (x == -1){
			printf("Ha habido un problema al cerrar el bot");
		}

	}
	
	return 0; 
	
}