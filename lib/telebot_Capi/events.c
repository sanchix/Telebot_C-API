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
void initUpdateNotifiers(update_notifier_t *notifiers){
	
	// TODO: Set first update handle to NULL (don't change offset on HTTP request), and others to "doNothing()"
	notifiers[0].handle = NULL;
	notifiers[0].event = EVENT_DEFFAULT;
	for(int i = 1; i < MAX_UPDATE_EVENTS; i++){
		notifiers[i].handle = doNothing;
		notifiers[i].event = EVENT_UNASSIGNED;
	}
	
}


// TODO: Comentar: Esta función debe bloquear los eventos (hay que poner un mutex)
// TODO: Cambiar el parametro newEvent por algo más userfriendly? Probablemente con hacer update_notifier_t más userfrienly sea suficiente...
int addUpdateNotifier(bot_info_t *bot_info, update_notifier_t newNotifier){
	
	int found = 0;
	sem_t * mutex_updateEvents = bot_info->mutex_updateEvents;

	// TODO: Poner exclusión mutua
	//bajar
	sem_wait(mutex_updateEvents);

	if(newNotifier.event == EVENT_DEFFAULT){
		bot_info->notifiers[0].handle = newNotifier.handle;
	}
	else{
		for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
			// Cuando encontremos uno vacío lo usamos
			if(bot_info->notifiers[i].event == EVENT_UNASSIGNED){
				bot_info->notifiers[i].handle = newNotifier.handle;
				bot_info->notifiers[i].event = newNotifier.event;
				found = 1;
			}
		}
	}
	// subir
	sem_post(mutex_updateEvents);
	
	return found-1;
	
}


// TODO: Comentar
int removeUpdateNotifier(bot_info_t *bot_info, update_notifier_t notifier){
	
	int found = 0;
	sem_t * mutex_updateEvents = bot_info->mutex_updateEvents;
	
	// TODO: Poner exclusión mutua
	// TODO: ¿Cambiar la exclusión mutua para que solo la garantice cuando moficica? Realmente no tiene mucha importancia porque modificar es un hecho anecdótico.
	//bajar
	sem_wait(mutex_updateEvents);
	if(notifier.event == EVENT_DEFFAULT){
		// TODO: Cambiar NULL por una función especial "DEFAULT"
		bot_info->notifiers[0].handle = NULL;
	}
	for(int i = 1; i < MAX_UPDATE_EVENTS && !found; i++){
		// TODO: Cambiar este "found" a algo que diga si son iguales (¿por una comparación hash?, ¿elemento a elemento?)
		if(found){
			bot_info->notifiers[i].handle = doNothing;
			bot_info->notifiers[i].event = EVENT_UNASSIGNED;
			found = 1;
		}
	}
	// subir
	sem_post(mutex_updateEvents);
	
	return found-1;
	
}