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



int main(int argc, char* argv[]){
	
	char info[4096] = {};
	bot_info_t bot_info;
	
	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	else if(telebot_init(argv[1], &bot_info) != 0){
			printf("Error al iniciar https\n");
	}else{
<<<<<<< HEAD
	
=======
		char texto[] = "Hola";
		char id[] = "150848014";
		sleep(2);
>>>>>>> 51fbc0e4ca5a1cc4364732e761077dde9a3673df
		printf("Initialized\n");
		sleep(2);
		
		telebot_getMe(info, sizeof(info), &bot_info);
<<<<<<< HEAD
		
		while(1){
			sleep(10);
			//printf("Info: %s\n", info);
		}
	
=======
		printf("Enviando un mensaje:\n");
		telebot_sendMessage(id,texto, &bot_info);
		sleep(15);

>>>>>>> 51fbc0e4ca5a1cc4364732e761077dde9a3673df
	}
	
	return 0; 
	
}