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

/* Definición de constantes */ 


/* Tipos definidos por el usuario */


/************************************************************/
/* Definiciones de funciones  */


// TODO: Comentar
void *doNothing(update_t *p){/*do nothing absolutely*/ return NULL; }


/*
**   Parámetros:  <tipo1> <parm1> <Descripción>
**                <tipo2> <parm2> <Descripción>
**                ...
**     Devuelve:  <tipo> <Descripción>
**
**  Descripción:  <Descripción>
*/
// Comentar: Esta función no tiene porqué bloquear los eventos (aún no se ha ejecutado el hilo de polling)
void initUpdateEvents(event_t *updateEvents){
	
	// TODO: Set first update handler to NULL (don't change offset on HTTP request), and others to "doNothing()"
	updateEvents[0].handler = NULL;
	updateEvents[0].condition = CONDITION_DEFFAULT;
	for(int i = 1; i < MAX_UPDATE_EVENTS; i++){
		updateEvents[i].handler = doNothing;
		updateEvents[i].condition = CONDITION_UNASSIGNED;
	}
	
}


// TODO: Comentar: Esta función debe bloquear los eventos (hay que poner un mutex)
// TODO: Cambiar el parametro newEvent por algo más userfriendly? Probablemente con hacer event_t más userfrienly sea suficiente...
int addUpdateEvent(poll_info_t *poll_info, event_t newEvent){
	
	int found = 0;
	sem_t * mutex_updateEvents = poll_info->mutex_updateEvents;

	// TODO: Poner exclusión mutua
	//bajar
	sem_wait(mutex_updateEvents);

	if(newEvent.condition == CONDITION_DEFFAULT){
		poll_info->updateEvents[0].handler = newEvent.handler;
	}
	else{
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// Cuando encontremos uno vacío lo usamos
			if(poll_info->updateEvents[i].condition == CONDITION_UNASSIGNED){
				poll_info->updateEvents[i].handler = newEvent.handler;
				poll_info->updateEvents[i].condition = newEvent.condition;
				found = 1;
			}
		}
	}
	// subir
	sem_post(mutex_updateEvents);
	
	return found-1;
	
}


// TODO: Comentar
int removeUpdateEvent(poll_info_t *poll_info, event_t event){
	
	int found = 0;
	sem_t * mutex_updateEvents = poll_info->mutex_updateEvents;
	
	// TODO: Poner exclusión mutua
	// TODO: ¿Cambiar la exclusión mutua para que solo la garantice cuando moficica? Realmente no tiene mucha importancia porque modificar es un hecho anecdótico.
	//bajar
	sem_wait(mutex_updateEvents);
	if(event.condition == CONDITION_DEFFAULT){
		// TODO: Cambiar NULL por una función especial "DEFAULT"
		poll_info->updateEvents[0].handler = NULL;
	}
	for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
		// TODO: Cambiar este "found" a algo que diga si son iguales (¿por una comparación hash?, ¿elemento a elemento?)
		if(found){
			poll_info->updateEvents[i].handler = doNothing;
			poll_info->updateEvents[i].condition = CONDITION_UNASSIGNED;
			found = 1;
		}
	}
	// subir
	sem_post(mutex_updateEvents);
	
	return found-1;
	
}