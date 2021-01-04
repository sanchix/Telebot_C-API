/*
**     Fichero:  testbot/testbot.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  04/enero/2021
**
** Descripcion:  Implementación de un BOT para la aplicación Telegram(R) utilizando la librería "telebot_Capi" con el objetivo de probar la funcionalidad de esta última.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> /* struct stat y fstat */
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
			//printf ("#   Total de votos recogidos: %lld\n\n",survey->total_votos);
			printf ("#   Total de votos recogidos: %lld\n\n",survey->total_votos);
			printf("####################################################\n");
		}
	}
}

void doEncuesta (update_t *update){
	
	message_t *message;
	char cid[20];
	char pregunta[MAX_POLL_QUESTION_TAM];
	char **options;
	int offset = strlen("/encuesta ");
	json_t *json_root;
	json_t *json_options;
	json_t *json_option;
	json_error_t error;

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		
		// Se hace eco si se ha recibido texto
		if(message->text != NULL){
			
			json_root = json_loads(message->text + offset, 0, &error);
			strcpy(pregunta , json_string_value (json_object_get(json_root,"pregunta")));
			json_options = json_object_get(json_root,"opciones");
			if ((options = (char **)malloc(sizeof(char*)*(int)(json_array_size(json_options)+1))) !=NULL){
				for(size_t i = 0; i < json_array_size(json_options); i++){
					json_option = json_array_get(json_options,i);
					if ((options[(int)i] = (char *)malloc(sizeof(char)*strlen(json_string_value(json_option))))!=NULL){				
						strcpy(options[(int)i],json_string_value(json_option));
					}else
						printf("Error en la reserva de la opcion %d\n",(int)i);
				}
			  
				options [json_array_size(json_options)] = NULL;

				FILE *fichero;
				fichero = fopen("suscripciones.txt","r"); //abre el fichero y se posiciona al principio

				while (fscanf(fichero,"%s\n",cid)!= EOF){
					telebot_sendPoll(cid,pregunta, options, update->http_info);
				}
				fclose(fichero);
				
				for(size_t i = 0; i < json_array_size(json_options); i++){
					free(options[(int)i]);
				}
				free (options);
			}
			else
				printf("Error en la reserva de opciones\n");
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


int findSubscripcion(char *chat_id){
	char cid[20];
	FILE *fichero;
	fichero = fopen("suscripciones.txt","r"); //abre el fichero y se posiciona al principio
	int i = 0;
	int x = -1;

	while (fscanf(fichero,"%s\n",cid)!= EOF){
		if (strcmp(cid,chat_id) == 0){
		x = i;
		}
		i++;
	}
	fclose(fichero);
	return x;
}


void doJoin(update_t *update){
	
	char cid[20]; //Aqui copiaremos el chat_id del usuario que ha enviado un mensaje.
	message_t *message;
	char subscripcion[3*64];
	FILE *fichero;

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		sprintf(cid,"%d",message->from.id);
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doJoin-##########################\n");
			printf("#   %s %s ha solicitado subscribirse a las encuestas.\n", message->from.first_name, message->from.last_name);


			if (findSubscripcion(cid) != -1){
				printf("#   Ya está subscrito.\n");
				printf("####################################################\n");
			}else{
				printf("####################################################\n");
				//Se almacenan los datos en el fichero:
				sprintf(subscripcion,"%d\n",message->from.id);
				fichero = fopen("suscripciones.txt","a"); //abre el fichero y se posiciona al final
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
	char cid[20]; 
	struct stat bstat;
	char *datos;
 	int fichero;
 	FILE *fichero2;

	// Solo vamos a hacer cosas con los mensajes
	if(update->type == UPDATE_MESSAGE){
		
		message = (message_t *)update->content;
		sprintf(cid,"%d",message->from.id);
		
		// Se imprime el mensaje
		if(message->from.id != 0){
			printf("##########################-doDelete-##########################\n");
			printf("#   %s %s ha solicitado borrar su subscripción a las encuestas.\n", message->from.first_name, message->from.last_name);
			printf("####################################################\n");
			
			if (findSubscripcion(cid) == -1){
				printf("#   No está suscrito.\n");
				printf("####################################################\n");
			}else{

			  	/* Abrir el fichero recibido como argumento */
  				if ((fichero=open("suscripciones.txt", O_RDONLY))<0){
    				imprimeError("No puede abrirse el fichero origen");
    			}else if (fstat(fichero, &bstat)<0){
                    imprimeError("Error en fstat en el fichero");
                }else{
					datos = (char *)malloc(sizeof(char)*bstat.st_size);
					
					read(fichero,datos,sizeof(char)*bstat.st_size);

					close(fichero);
	                
	                /* Bucle de búsqueda */
				    int len;
				    int newlen;
				    int offset = 0;
				    char aux[100];
				    char* found = NULL;
				    int iteracion = 0;
				    
				    len = strlen(datos);
					printf("Datos (%i):%s\n\n", len, datos);
					
					printf("Datos[len-1]: %i\n", datos[len-1]);
					printf("Datos[len]: %i\n", datos[len]);
					printf("Datos[len+1]: %i\n", datos[len+1]);

				    while(offset < len && !found){

				        sscanf(datos+offset, "%s\n", aux);
				        
				        if(strcmp(aux, cid) == 0){
				            found = datos + offset;
				        }
				        
				        offset += strlen(aux) + 1;
				        iteracion++;

				    }
					
				    printf("found:%s\n",found );
				    printf("after:%s\n",found+strlen(aux)+1);
				    strcpy(found, found+strlen(aux)+1);
				    printf("data: %s\n", datos);

					newlen = len-strlen(aux)-1;
					fichero2 = fopen("suscripciones.txt","w"); //abre el fichero y se posiciona al final
					fputs(datos,fichero2);
	     			fclose(fichero2);
     			}
			}
		}else{
			printf("##########################-doHelp-##########################\n");
			printf("#   Alguien ha solicitado subscribirse a las encuestas.\n");
			printf("####################################################\n");
		}
		
	}else{
	// Si lo recibido no es un mensaje no hacemos nada
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
	
	// Iniciamos las funciones de librería en bot_info
	else if(telebot_init(argv[1], &bot_info) != 0){
			imprimeError("TESTBOT: Error al iniciar telebot_Capi");
	}
	
	// Si todo va correcto procedemos a realizar pruebas:
	else{
		
		printf("\033[1;33m");
		printf("TESTBOT: Initialized\n");
		printf("\033[0m");
		
		telebot_sendMessage(jcube, "Haciendo una prueba", &bot_info.http_info);		
		//telebot_sendPoll(ros,pregunta,opciones, &bot_info.http_info);
		/*
		telebot_sendMessage(jcube, "Haciendo una prueba", &bot_info.http_info);		
		telebot_sendPoll(jcube,pregunta,opciones, &bot_info.http_info);
		*/


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

		event.update_type = UPDATE_MESSAGE;
		strcpy(event.info, "encuesta");
		if(addUpdateNotifier(doEncuesta, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler doEncuesta");
		}
		event.update_type = UPDATE_POLL;
		strcpy(event.info, "");
		if(addUpdateNotifier(doSurvey, &event, &bot_info) != 0){
			imprimeError("TESTBOT: Fallo al añadir handler de la encuesta");
		}
		
		// Loop to keep alive main thread
		while(1){
			sleep(1);
		}

	}
	
	return 0; 
	
}
