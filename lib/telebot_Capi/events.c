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
int holdUpdate(update_t *p){ return UPDATE_HOLD; }


/*
**   Parámetros:  <tipo1> <parm1> <Descripción>
**                <tipo2> <parm2> <Descripción>
**                ...
**     Devuelve:  <tipo> <Descripción>
**
**  Descripción:  <Descripción>
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


// TODO: Comentar
// Comentarios: Esta función debe bloquear los eventos (hay que poner un mutex)
int addUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semáforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;

	// Si el evento es por defecto almacenamos el nuevo handle.
	if(event == EVENT_DEFFAULT){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = handle;
		sem_post(mutex_updateNotifiers);
	}
	
	// Si no es el por defecto...
	else{
		// se busca un hueco (caracterizado por EVENT_UNASSIGNED)
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se edita.
			if(bot_info->notifiers[i].event == EVENT_UNASSIGNED){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = handle;
				// TODO: Si cambiamos event a una estructura o algo hay que cambiar esta asignación a una copia de memoria o a un puntero.
				bot_info->notifiers[i].event = event;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	// TODO: Comentar: Devuelve -1 en caso de que no se haya podido encontrar un hueco.
	return found-1;
	
}


// TODO: Comentar
// Comentarios: Esta función debe bloquear los eventos (hay que poner un mutex)
int modifyUpdateNotifier(updateHandle_t handle, event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semáforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;

	// Si el evento es por defecto almacenamos el nuevo handle.
	if(event == EVENT_DEFFAULT){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = handle;
		sem_post(mutex_updateNotifiers);
	}
	
	// Si no es el por defecto...
	else{
		// se busca el evento
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se edita.
			// TODO: Cambiar este "found" por la búsqueda del evento. Antes hay que pensar si se van a poder tener varios handle para un mismo evento y como se va a hacer ¿se tendrán varios objetos update_event_t con el mismo event_t? ¿En los objetos update_event_t puede haber una lista de hanles para cada evento?
			if(found){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = handle;
				// TODO: Si cambiamos event a una estructura o algo hay que cambiar esta asignación a una copia de memoria o a un puntero.
				bot_info->notifiers[i].event = event;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	// TODO: Comentar: Devuelve -1 en caso de que no se haya podido encontrar un hueco.
	return found-1;
	
}


// TODO: Comentar
int removeUpdateNotifier(event_t event, bot_info_t *bot_info){
	
	int found = 0;
	sem_t * mutex_updateNotifiers;

	// Preparamos el semáforo (solo se va a subir y bajar cuando se hagan modificaciones, pues el resto de funciones de la librería se limitan a leer y es más eficiente hacerlo así pues no bloqueamos a los lectores durante la búsqueda).
	mutex_updateNotifiers = bot_info->mutex_updateNotifiers;
	
	// Si el evento a borrar es el por defecto reestablecemos su handle inicial.
	if(event == EVENT_DEFFAULT){
		sem_wait(mutex_updateNotifiers);
		bot_info->notifiers[0].handle = holdUpdate;
		sem_post(mutex_updateNotifiers);
	}
	
	// Si no es el por defecto...
	else{
		// se el evento en particular...
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// y cuando se encuentre se edita.
			// TODO: Cambiar este "found" a algo que diga si son iguales (¿por una comparación hash?, ¿elemento a elemento?)
			if(found){
				sem_wait(mutex_updateNotifiers);
				bot_info->notifiers[i].handle = holdUpdate;
				bot_info->notifiers[i].event = EVENT_UNASSIGNED;
				sem_post(mutex_updateNotifiers);
				found = 1;
			}
		}
	}
	
	// TODO: Comentar: Devuelve -1 en caso de que no se haya podido borrar el evento (no existía).
	return found-1;
	
}