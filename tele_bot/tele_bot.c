/*
**     Fichero:  tele_bot/tele_bot.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Implementación de un BOT para la aplicación Telegram(R) utilizando la librería "telebot_Capi" con el objetivo de probar la funcionalidad de esta última.
*/

#include <stdio.h>
#include <unistd.h>
#include "telebot_Capi/telebot_Capi.h"



int main(int argc, char* argv[]){
	
	char info[4096] = {};
	bot_https_t bot_info;
	
	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	else if(telebot_init(argv[1], &bot_info) != 0){
		printf("Error al iniciar https\n");
		
	}else{
		
		sleep(2);
		printf("Initialized\n");
		telebot_getMe(info, sizeof(info), &bot_info);
		while(1){
			sleep(1);
			printf("Info: %s\n", info);
		}

	}
	
	return 0; 
	
}