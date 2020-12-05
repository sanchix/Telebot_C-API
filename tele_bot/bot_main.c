
#include <stdio.h>
#include "lib/telebot_Capi.h"


int main(int argc, char* argv[]){
	
	char info[200] = NULL
	
	if(argc != 2){
		printf("Error...");
	}
	else if(telebot_init() != 0){
		printf("Error al iniciar https");
		
	}else{
	
		info = telebot_getMe(argv[1]);
		
		printf("Info: %s", info);

	}
	
	return 0;
	
}