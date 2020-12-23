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


// TODO: Comentar
int ignoreUpdate(update_t *p){ return UPDATE_DROP; }

// TODO: Comentar
// TODO: Hacer que polling aproveche el retorno de la función.
// Se guardan las actualizaciones y no se borran. Será el handler por defecto para el evento por defecto.
int holdUpdate(update_t *p){ return UPDATE_HOLD; }


/*
**   Parámetros:  update_notifier_t *notifiers: 
**                
**                
**     Devuelve:  void
**
**  Descripción:  Esta funcion inicializa los eventos como NULL, dejandolos listos para que sean asignados. Además establece el evento por defecto. 
*/
// TODO: Comentar: Esta función no tiene porqué bloquear los eventos (aún no se ha ejecutado el hilo de polling)
void initUpdateNotifiers(update_notifier_t *notifiers){
	
	// TODO: Set first update handle to NULL (don't change offset on HTTP request), and others to "doNothing()"
	notifiers[0].handle = holdUpdate;
	notifiers[0].event = EVENT_DEFFAULT;
	for(int i = 1; i < MAX_UPDATE_EVENTS; i++){
		notifiers[i].handle = ignoreUpdate;
		notifiers[i].event = EVENT_UNASSIGNED;
	}
	
}

/*
**   Parámetros:  updateHandle_t handle: es la accion que se va a realizar cuando se detecte el evento que estamos añadiendo
**                event_t event: nombre del nuevo evento que estamos añadiendo / si es el EVENT_DEFFAULT se le asigna a el.
**				  bot_info_t *bot_info 	
**                
**     Devuelve:  int Devuelve 0 en caso de exit, -1 en caso de que no se haya podido encontrar un hueco. 
**
**  Descripción:  Al pasar el nuevo evento, es asignado como default si se le indica, o como un nuevo evento en uno de los espacios disponibles de los MAX_UPDATE_EVENTS.  
*/
// TODO: Comentar
// Comentarios: Esta función debe bloquear los eventos (hay que poner un mutex)
int addUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semáforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;

	// Si el evento es por defecto almacenamos el nuevo handle.
	//TODO: Se podria implementar modify porque estamos haciendo una modificacion....
	if( strcmp(event,EVENT_DEFFAULT) == 0 ){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = handle;
		sem_post(mutex_updateNotifiers);
		found = 1;
	}
	
	// Si no es el por defecto:
	else{
		// se busca un hueco (caracterizado por EVENT_UNASSIGNED)
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se edita.
			if( strcmp(bot_info->notifiers[i].event,EVENT_UNASSIGNED) == 0 ){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = handle;
				// TODO: Si cambiamos event a una estructura o algo hay que cambiar esta asignación a una copia de memoria o a un puntero.
				bot_info->notifiers[i].event = event;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	return found-1;
	
}

/*
**   Parámetros:  updateHandle_t handle: es la nueva accion que se va a realizar cuando se detecte el evento que estamos modificando.
**                event_t event: nombre del evento que estamos modificando.
**				  bot_info_t *bot_info 	
**                
**     Devuelve:  int Devuelve 0 en caso de exit, -1 en caso de que no se haya podido encontrar el evento a modificar. 
**
**  Descripción:  Al pasar el nuevo handler, es asignado al evento indicado que debe existir.   
*/
// TODO: Comentar
// Comentarios: Esta función debe bloquear los eventos (hay que poner un mutex)
int modifyUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semaforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;

	// Si el evento es por defecto almacenamos el nuevo handle.
	if( strcmp(event,EVENT_DEFFAULT) == 0 ){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = handle;
		sem_post(mutex_updateNotifiers);
		found = 1;
	}
	
	// Si no es el por defecto...
	else{
		// se busca el evento
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se edita.
			
			// TODO: Hay que pensar si se van a poder tener varios handle para un mismo evento y como se va a hacer ¿se tendrán varios objetos update_event_t con el mismo event_t? ¿En los objetos update_event_t puede haber una lista de hanles para cada evento?
			if( strcmp(event,bot_info->notifiers[i].event) == 0 ){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = handle;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	return found-1;
	
}

/*
**   Parámetros:  event_t event: nombre del evento que estamos eliminando.
**				  bot_info_t *bot_info 	
**                
**     Devuelve:  int Devuelve 0 en caso de exit, -1 en caso de que no se haya podido eliminar el evento (no existia). 
**
**  Descripción:  Si se quiere borrar el evento EVENT_DEFFAULT se e establece el handle por defecto, si es cualquier otro, se le asigna EVENT_UNASSIGNED y holdUpdate como handler por defecto.
*/
// TODO: Comentar
int removeUpdateNotifier(event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semáforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer y es más eficiente hacerlo así pues no bloqueamos a los lectores durante la búsqueda).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;
	
	// Si el evento a borrar es el por defecto reestablecemos su handle inicial.
	if( strcmp(event,EVENT_DEFFAULT) == 0 ){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = holdUpdate;
		sem_post(mutex_updateNotifiers);
		found = 1;
	}
	
	// Si no es el por defecto...
	else{
		// se busca el evento en particular...
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se borra.
			// TODO: Cambiar este "found" a algo que diga si son iguales (¿por una comparación hash?, ¿elemento a elemento?)
			// DONE: Se compara el evento que se quiere eliminar con la lista.
			if( strcmp(event,bot_info->notifiers[i].event) == 0 ){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = holdUpdate;
				bot_info->notifiers[i].event = EVENT_UNASSIGNED;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	return found-1;
	
}