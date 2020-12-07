/*
**     Fichero:  tele_bot/tele_bot.c
**       Autor:  Grupo 8
**       Fecha:  <fecha de última edición>
**
** Descripcion:  <Descripción sucinta del contenido del fichero>
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