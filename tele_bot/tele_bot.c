
#include <stdio.h>
#include "telebot_Capi/telebot_Capi.h"


int main(int argc, char* argv[]){
	
	char info[4096] = {};
	
	if(argc != 2){
		printf("Error...\n");
	}
	else if(telebot_init(argv[1]) != 0){
		printf("Error al iniciar https\n");
		
	}else{
	
		telebot_getMe(info, sizeof(info));
		printf("Info: %s\n", info);

	}
	
	return 0;
	
}