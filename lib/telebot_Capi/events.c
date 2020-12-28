/*
**     Fichero:  telebot_Capi/events.c
**       Group:  Grupo 8
**		Author:  Juan Parada Claro, Javier Ros Raposo y Javier Sanchidrián Boza
**       Fecha:  07/dec/2020
**
** Descripcion:  Definición de funciones relativas a eventos necesarias para la comunicación de actualizaciones al usuario de la librería.
*/

/* Includes del sistema */ 


/* Includes de la aplicacion */
#include "telebot_Capi.h"



/************************************************************/
/* Definiciones de funciones  */

// Función LOCAL (no declarada en telebot_Capi)
/*
**   Parámetros:  update_t *p: Se ignora
**
**  Descripción:  Ignora un update
*/
void ignoreUpdate(update_t *p){ ; }


// Función LOCAL (no declarada en telebot_Capi)
/*
**   Parámetros:  event_t e1: Evento a comparar
**                event_t e2: Evento a comparar
**                
**     Devuelve:  int: Resultado de la comparación (0 si iguales)
**
**  Descripción:  Compara dos eventos e indica si son iguales o no.
*/
int compareEvents(event_t *e1, event_t *e2){
	
	int ret = 1;
	
	// Si los campos coinciden devolvemos 0
	if(e1->update_type == e2->update_type && strcmp(e1->info, e2->info) == 0){
		ret = 0;
	}
	
	return ret;
	
}


// Función LOCAL (no declarada en telebot_Capi)
/*
**   Parámetros:  event_t *dst: Puntero al evento destino.
**                event_t *org: Puntero al evento a copiar (origen).
**                
**     Devuelve:  void
**
**  Descripción:  Copia los datos del evento org en dst
*/
// TODO: Poner valor de retorno (errores)
void copyEvent(event_t *dst, event_t *org){
	
	dst->update_type = org->update_type;
	strcpy(dst->info, org->info);
	
}


/*
**   Parámetros:  update_notifier_t *notifiers: array notifier_t a inicializar
**                
**     Devuelve:  void
**
**  Descripción:  Inicializa los eventos. Se establece el handle del evento por defecto (notifiers[0]) a ignoreUpdate. El resto de eventos se configuran como UPDATE_NONE y con el sin manejador. La función no lleva semáforo pues no tiene porqué garantizar exclusión mutua en los eventos (aún no se ha ejecutado el hilo de polling)
*/
void initUpdateNotifiers(update_notifier_t *notifiers){
	
	// Configurar handle del evento por defecto a ignoreUpdate.
	notifiers[0].event.update_type = UPDATE_ANY;
	notifiers[0].handle = ignoreUpdate;
	
	// Configurar el resto de eventos como UPDATE_NONE
	for(int i = 1; i < MAX_UPDATE_EVENTS; i++){
		notifiers[i].event.update_type = UPDATE_NONE;
		notifiers[i].handle = NULL;
	}
	
}


/*
**   Parámetros:  updateHandle_t handle: manejador para el evento
**                event_t *event: evento
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar un hueco o el nuevo evento tiene tipo UPDATE_NONE).
**
**  Descripción:  Añade 'event' en los notifiers de bot_info. Si 'event' tiene tipo UPDATE_ANY se guarda el handle en el evento por defecto; en otro caso se almacena en el primer notifier cuyo evento sea de tipo UPDATE_NONE, notificando error si no es posible.
*/
int addUpdateNotifier(updateHandle_t handle, event_t *event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparar el semáforo (solo se usa para las modificaciones, el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->notifiers_info.mutex_updateNotifiers;

	// Comprobación
	if(handle != NULL){
		
		// Si el evento es por defecto almacenamos el nuevo handle.
		// TODO: Se podria implementar modify porque estamos haciendo una modificacion...
		if(event->update_type == UPDATE_ANY){
			sem_wait(mutex_updateNotifiers);
			bot_info->notifiers_info.notifiers[0].handle = handle;
			sem_post(mutex_updateNotifiers);
			found = 1;
		}
		
		// Si no es el por defecto:
		else if(event->update_type != UPDATE_NONE){
			
			// se busca un hueco (caracterizado por UPDATE_NONE)
			for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
				// y cuando se encuentre se edita.
				if(bot_info->notifiers_info.notifiers[i].event.update_type == UPDATE_NONE){
					sem_wait(mutex_updateNotifiers);
					bot_info->notifiers_info.notifiers[i].handle = handle;
					copyEvent(&(bot_info->notifiers_info.notifiers[i].event), event);
					sem_post(mutex_updateNotifiers);
					found = 1;
				}
			}
		}
	}
	
	return found-1;
	
}


/*
**   Parámetros:  updateHandle_t handle: nuevo manejador para el evento.
**                event_t *event: nombre del evento a modificar
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar el evento). 
**
**  Descripción:  Edita el manejador de 'event'.
*/
int modifyUpdateNotifier(updateHandle_t handle, event_t *event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparar el semáforo (solo se usa para las modificaciones, el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->notifiers_info.mutex_updateNotifiers;

	// Comprobación
	if(handle != NULL){
		
		// Si el evento es por defecto almacenamos el nuevo handle.
		if(event->update_type == UPDATE_ANY){
			sem_wait(mutex_updateNotifiers);
			bot_info->notifiers_info.notifiers[0].handle = handle;
			sem_post(mutex_updateNotifiers);
			found = 1;
		}
		
		// Si no es el por defecto...
		else if(event->update_type != UPDATE_NONE){
			
			// se busca el evento
			for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
				// y cuando se encuentre se edita.
				// TODO: Pensar si se van a poder tener varios handle para un mismo evento y como se va a hacer ¿se tendrán varios objetos update_event_t con el mismo event_t? ¿En los objetos update_event_t puede haber una lista de hanles para cada evento?
				if(compareEvents(event, &(bot_info->notifiers_info.notifiers[i].event)) == 0){
					sem_wait(mutex_updateNotifiers);
					bot_info->notifiers_info.notifiers[i].handle = handle;
					sem_post(mutex_updateNotifiers);
					found = 1;
				}
			}
		}
	}
		
	return found-1;
	
}


/*
**   Parámetros:  event_t *event: evento a eliminar
**				  bot_info_t *bot_info: información de la libería
**                
**     Devuelve:  int: 0 si se completa con éxito, -1 en caso de error (no se ha podido encontrar el evento).
**
**  Descripción:  Borra el manejador del notifier identificado por 'event' y cambia el evento a UPDATE_NONE. Si el tipo de 'event' es UPDATE_ANY se cambia el manejador del evento por defecto a ignoreUpdate.
*/
int removeUpdateNotifier(event_t *event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparar el semáforo (solo se usa para las modificaciones, el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->notifiers_info.mutex_updateNotifiers;
	
	// Si el evento a borrar es el por defecto reestablecemos su handle inicial.
	if(event->update_type == UPDATE_ANY){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers_info.notifiers[0].handle = ignoreUpdate;
		sem_post(mutex_updateNotifiers);
		found = 1;
	}
	
	// Si no es el por defecto...
	else{
		// se busca el evento en particular...
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se borra.
			if(compareEvents(event, &(bot_info->notifiers_info.notifiers[i].event)) == 0){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers_info.notifiers[i].event.update_type = UPDATE_NONE;
				bot_info->notifiers_info.notifiers[i].handle = NULL;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	return found-1;
	
}


/*
**   Parámetros:  update_t *update: información sobre un update
**				  update_notifier_t *notifiers: Lista de notifiers
**                
**     Devuelve:  updateHandle_t: handle del evento que coincida o NULL si no se encuentra
**
**  Descripción:  Busca el evento de 'notifiers' que mejor coincida con 'update' y devuelve su manejador. En caso de no encontrarlo devuelve el handle del evento por defecto.
*/
updateHandle_t findUpdateHandler(update_t *update,notifiers_info_t *notifiers_info){
	update_notifier_t *notifiers = notifiers_info->notifiers;
	updateHandle_t handle = NULL;
	updateHandle_t handle_def = NULL; // Manejador por defecto para un cierto tipo (sin información)
<<<<<<< HEAD
	sem_t *mutex_updateNotifiers;
	// Preparar los semáforos
	mutex_updateNotifiers = notifiers_info->mutex_updateNotifiers;
=======
	char * aux = NULL;
>>>>>>> 2e3b56d05833f490e5b2bbef650eb3e11e26e32c
	
	// Se entra en la región compartida
	sem_wait(mutex_updateNotifiers);

	// se comienza buscando entre eventos asignados.
	for(int i = 1; (i < MAX_UPDATE_EVENTS) && (handle == NULL); i++){
		// Si coindice el tipo...
		if(notifiers[i].event.update_type == update->type){
			// y es mensaje...
			if(update->type == UPDATE_MESSAGE){
				// si el evento tiene comando filtramos y asignamos.
				// TODO: Cambiar notifiers[i].event.info != NULL por strcmp (no vale null, vale "")
				if(notifiers[i].event.info[0] != '\0'){
					printf("####################################################\n");
					printf("Encontrado: %s\n", notifiers[i].event.info);
					printf("Comando: %s\n", ((message_t *)update->content)->command);
					printf("####################################################\n");
					if(((message_t *)update->content)->command != NULL && strcmp(notifiers[i].event.info, ((message_t *)update->content)->command) == 0){
						handle = notifiers[i].handle;
					}
				}
				// y si no tiene asignamos.
				else{
					handle_def = notifiers[i].handle;
				}
			} else if (update->type == UPDATE_POLL) {
			  	//Si el evento tiene algo dentro de info y es de tipo encuesta, lo de dentro será la id de la encuesta.
				if(notifiers[i].event.info[0] != '\0'){
				 	 
				 	printf ("Respuesta de encuesta recibida cuyo id: %s",notifiers[i].event.info);
					
					sprintf (aux,"%llu",((poll_update_t*)update->content)->poll_id);
					if (((poll_update_t*)update->content)->poll_id != 0 && strcmp(aux,notifiers[i].event.info) == 0 ){
						handle = notifiers[i].handle;
					}
				}
				else{
					handle_def = notifiers[i].handle;
				}
				
			}
			// TODO: Poner else if para poll u otros tipos
		}
		
	}
	
	// Se asigna evento por defecto si no se ha encontrado
	if(handle == NULL){
		if(handle_def != NULL){
			handle = handle_def;
		}
		else{
			handle = notifiers[0].handle;
		}
	}
	
	// Se sale de la región compartida
	sem_post(mutex_updateNotifiers);

	return handle;
	
}
