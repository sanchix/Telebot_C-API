
#include <stdio.h>
#include "../src/telebot_Capi.h"


int main(int argc, char* argv[]){
	
	char *info = NULL;
	
	if(argc != 2){
		printf("Error...");
	}
	else if(telebot_init(argv[1]) != 0){
		printf("Error al iniciar https");
		
	}else{
	
		info = telebot_getMe();
		
		printf("Info: %s", info);

	}
	
	return 0;
	
}