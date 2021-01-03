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

void imprimeError(char error[]){

	printf("\033[1;31m");
	printf("####################################################\n");
	printf("#   %s \n",error);
	printf("####################################################\n");
	printf("\033[0m");

}

void doEcho(update_t *update){
	
	message_t *message;
	char cid[20];
	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doEcho-##########################\n");
			printf("#   Mensaje recibido de %s %s: %s\n", message->from.first_name, message->from.last_name, message->text);
		}
		else{
			printf("##########################-doEcho-##########################\n");
			printf("#   Mensaje recibido de alguien: %s\n", message->text);
		}
		
		// Se hace eco si se ha recibido texto
		if(message->text != NULL){
			printf("#   Haciendo eco...\n");
			printf("####################################################\n");
			sprintf(cid, "%i", message->chat.id);
			telebot_sendMessage(cid, message->text, update->http_info);
		}
		
	}
	
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-doEcho-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
	
	// TODO: Pensar que poner en el retorno
	
}

void doSurvey (update_t *update){
  
	poll_update_t *survey;

	if (update->type == UPDATE_POLL){
		survey = (poll_update_t *)update->content;
		if (survey->question != NULL && survey->options != NULL){
			printf("##########################-doSurvey-##########################\n");
			printf("#   Pregunta de la encuesta: %s\n\n",survey->question);
			for (int i=0; survey->options[i].text !=NULL;i++){
			  printf ("#\tOpcion %d: %s, Votos: %d\n",i,survey->options[i].text,survey->options[i].opcion_votos);
			}
			printf ("#   Total de votos recogidos: %lld\n\n",survey->total_votos);
			printf ("#   Total de votos recogidos: %lld\n\n",survey->total_votos);
			printf("####################################################\n");
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
			printf("##########################-felicita-##########################\n");
			printf("#   Mensaje recibido de %s %s: %s\n", message->from.first_name, message->from.last_name, message->text);
		}
		else{
			printf("##########################-felicita-##########################\n");
			printf("#   Mensaje recibido de alguien: %s\n", message->text);
		}
		
		// Se hace eco si se ha recibido texto
		if(message->text != NULL){
			printf("#   Felicitando...\n");
			printf("####################################################\n");
			sprintf(cid, "%i", message->chat.id);
			strcat(text, message->from.first_name);
			telebot_sendMessage(cid, text, update->http_info);
		}
		
	}
	
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-felicita-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
}

void doDefault(update_t *update){
	
	message_t *message;
	char cid[20]; //Aqui copiaremos el chat_id del usuario que ha enviado un mensaje.

	int M = 0;
	int N = 5;
	int Z = rand () % (N-M+1) + M;   // Este está entre M y N
	char respuestas[][55] = {"Escribe un /comando","Escribe /help para obtener todos los comandos activos","No te entiendo","Gracias por hablarme","¿Que tal si haces una encuesta?"};

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doDefault-##########################\n");
			printf("#   Mensaje recibido de %s %s: %s\n", message->from.first_name, message->from.last_name, message->text);
		}
		else{
			printf("##########################-doDefault-##########################\n");
			printf("#   Mensaje recibido de alguien: %s\n", message->text);
		}

		// Se le responde si se ha recibido texto
		if(message->text != NULL){
			printf("#   No ha indicado ningún comando...\n");
			printf("####################################################\n");
			sprintf(cid, "%i", message->chat.id);
			telebot_sendMessage(cid, respuestas[Z], update->http_info);
		}
	}
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-doDefault-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
}


void doHelp(update_t *update){
	
	message_t *message;
	char cid[20]; //Aqui copiaremos el chat_id del usuario que ha enviado un mensaje.
	char ayuda[] = "/help\n/feliz\n/starteco\n/stopeco\n/join\n/delete";
	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doHelp-##########################\n");
			printf("#   %s %s ha solicitado ayuda.\n", message->from.first_name, message->from.last_name);
			printf("####################################################\n");
		}
		else{
			printf("##########################-doHelp-##########################\n");
			printf("#   Alguien ha solicitado ayuda\n");
			printf("####################################################\n");
		}
		
		// Se le responde si se ha recibido texto
		if(message->text != NULL){		
			sprintf(cid, "%i", message->chat.id);
			telebot_sendMessage(cid, ayuda, update->http_info);
		}
	}
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-doHelp-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
}


int findSubscripcion(int chat_id){
	char cid[20];
	FILE *fichero;
	fichero = fopen("subscripciones.txt","r"); //abre el fichero y se posiciona al principio
	int i = 0;
	int x = -1;
	while (fscanf(fichero,"%s\n",cid)!= EOF){
		if (strcmp(cid,chat_id)==0){
		x = i;
		}
		//printf("chat_id:%s\n",cid);
	}
	fclose(fichero);
	return x;
}


void doJoin(update_t *update){
	
	message_t *message;
	char subscripcion[3*64];
	FILE *fichero;

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doJoin-##########################\n");
			printf("#   %s %s ha solicitado subscribirse a las encuestas.\n", message->from.first_name, message->from.last_name);


			if (findSubscripcion(message->from.id)>0){
				printf("#   Ya está subscrito.\n", message->from.first_name, message->from.last_name);
				printf("####################################################\n");
			}else{
				printf("####################################################\n");
				//Se almacenan los datos en el fichero:
				sprintf(subscripcion,"%d\n",message->from.id);
				fichero = fopen("subscripciones.txt","a"); //abre el fichero y se posiciona al final
				if (fputs(subscripcion,fichero)<0){
					imprimeError("TESTBOT: Error al escribir una subscripción en el fichero.");
				}
				fclose(fichero);
				}			
		}
		else{
			printf("##########################-doHelp-##########################\n");
			printf("#   Alguien ha solicitado subscribirse a las encuestas.\n");
			printf("####################################################\n");
		}
		
	}
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-doJoin-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
}



void doDelete(update_t *update){
	
	message_t *message;
	char cid[20]; //Aqui copiaremos el chat_id del usuario que ha enviado un mensaje.
	int fichero;
	char *datos;

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doJoin-##########################\n");
			printf("#   %s %s ha solicitado borrar su subscripción a las encuestas.\n", message->from.first_name, message->from.last_name);
			printf("####################################################\n");
			
			if (findSubscripcion(message->from.id)>0){
				printf("#   No está subscrito.\n", message->from.first_name, message->from.last_name);
				printf("####################################################\n");
			}else{
				//Se almacenan los datos en el fichero:			
				if(fichero=open("subscripciones.txt",O_RDWR)<0){
					imprimeError("TESTBOT: No se ha podido abrir el fichero orgien.");
				}else{

					/* Averigua la longitud del fichero origen */
         			if(fstat(fichero, &bstat)<0){
            			perror("Error en fstat en el fichero");
         			}else{
			            if ((datos=(char *)mmap((caddr_t) 0, bstat.st_size, PROT_READ, MAP_SHARED, fichero, 0)) == MAP_FAILED){
            			   perror("Error en la proyeccion del fichero origen");
            			}else{

			                /* Bucle de búsqueda */
				            int j = 0;
				            int i = 0;
				            for (i=0; i<bstat.st_size; i++){
				                if (datos[i] == argv[1][0])
				                    j++;
				            }
				            printf("El caracter %s se repite %d veces en el fichero %s\n",argv[1],j,argv[2]);
			                
			                /* Se eliminan la proyeccion */
			                if (munmap(datos, bstat.st_size)){
			                	perror("Error al eliminar la proyeccion");
			                }

            			}
         			}
         			close(fichero);
				}				
				while (fscanf(fichero,"chat_id:%s\n",cid)!= EOF){
					printf("chat_id:%s\n",cid);
				}
			}
		}
		else{
			printf("##########################-doHelp-##########################\n");
			printf("#   Alguien ha solicitado subscribirse a las encuestas.\n");
			printf("####################################################\n");
		}
		
	}
	// Si lo recibido no es un mensaje no hacemos nada
	else{
		printf("##########################-doJoin-##########################\n");
		printf("#   Algo recibido: ...\n");
		printf("####################################################\n");
	}
}



int main(int argc, char* argv[]){
	
	bot_info_t bot_info;
	event_t event;
	
	char ros[] = "166103691";
	char juan[] = "833182696";
	char jcube[] = "-415881867";
	
	char pregunta[] = "¿Funcionara?";
	char *opciones[20] = {"SI","NO","OBERSERVAD",NULL};

	if(argc != 2){
		printf("Wrong number of arguments, usage: %s [token]\n", argv[0]);
	}
	
	// TODO: Configurar los handle antes
	// Iniciamos las funciones de librería en bot_info
	else if(telebot_init(argv[1], &bot_info) != 0){
			imprimeError("TESTBOT: Error al iniciar telebot_Capi");
	}
	
	// Si todo va correcto procedemos a realizar pruebas:
	else{
		
		printf("\033[1;33m");
		printf("TESTBOT: Initialized\n");
		printf("\033[0m");
		
<<<<<<< HEAD
<<<<<<< HEAD
		char ros[] = "166103691";
		char juan[] = "150848014";

		
		char pregunta[] = "¿Funcionara?";
		char *opciones[20] = {"SI","NO","OBERSERVAD",NULL};
		
		telebot_sendMessage(ros, "Haciendo una prueba", &bot_info.http_info);		
		telebot_sendPoll(ros,pregunta,opciones, &bot_info.http_info);
		
=======
		telebot_sendMessage(jcube, "Haciendo una prueba", &bot_info.http_info);		
		telebot_sendPoll(jcube,pregunta,opciones, &bot_info.http_info);
>>>>>>> e7f136ba5637af135044b09a076e466eb6e562f6
=======
		telebot_sendMessage(jcube, "Haciendo una prueba", &bot_info.http_info);		
		telebot_sendPoll(jcube,pregunta,opciones, &bot_info.http_info);
>>>>>>> e7f136ba5637af135044b09a076e466eb6e562f6

		// Configuramos el handle imprime:
		// evento -> EVENT_DEFFAULT = Comportamiento por defecto
		// handle -> imprimeMensaje
		event.update_type = UPDATE_ANY;
		if(addUpdateNotifier(doDefault, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler doDefault");
		}
		
		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "feliz");
		if(addUpdateNotifier(felicita, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler felicita");
		}

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "help");
		if(addUpdateNotifier(doHelp, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler doHelp");
		}

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "join");
		if(addUpdateNotifier(doJoin, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler doJoin");
		}

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "delete");
		if(addUpdateNotifier(doDelete, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler doDelete");
		}

		void starteco(){
			event.update_type = UPDATE_ANY;
			if(addUpdateNotifier(doEcho, &event, &bot_info) != 0){
				imprimeError("TESTBOT: Fallo al añadir handler doEcho");
			}
		}

		void stopeco(){

			event.update_type = UPDATE_ANY;
			if(addUpdateNotifier(doDefault, &event, &bot_info) != 0){
				imprimeError("TESTBOT: Fallo al añadir handler doDefault");
			}
		}

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "starteco");
		if(addUpdateNotifier(starteco, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler starteco");
		}

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "stopeco");
		if(addUpdateNotifier(stopeco, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler stopeco");
		}

		event.update_type = UPDATE_POLL;
		strcpy(event.info, "");
		if(addUpdateNotifier(doSurvey, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler de la encuesta");
		}
		// TODO: Comprobar que funcionan los semáforos
		
		// Loop to keep alive main thread
		while(1){
			sleep(1);
		}

	}
	
	return 0; 
	
}
