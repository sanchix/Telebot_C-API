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
void *doNothing(void *p){/*do nothing absolutely*/ return NULL; }


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
	updateHandlers[0].handler = NULL;
	updateHandlers[0].condition = CONDITION_UNASSIGNED;
	for(int i = 1; i < MAX_UPDATE_HANDLERS; i++){
		updateHandlers[i].handler = doNothing;
		updateHandlers[i].condition = CONDITION_UNASSIGNED;
	}
	
}


// TODO: Comentar: Esta función debe bloquear los eventos (hay que poner un mutex)
// TODO: Cambiar el parametro newEvent por algo más userfriendly? Probablemente con hacer event_t más userfrienly sea suficiente...
int addUpdateEvent(event_t *updateEvents, event_t newEvent){
	
	int found = 0;
	
	// TODO: Poner exclusión mutua
	//bajar
	for(int i = 1; i < MAX_UPDATE_HANDLERS && !found; i++){
		// Cuando encontremos uno vacío lo usamos
		if(updateEvents[i].condition == CONDITION_UNASSIGNED){
			updateEvents[i].handler = newEvent.handler;
			updateEvents[i].condition = newEvent.condition;
			found = 1;
		}
	}
	// subir
	
	return found-1
}


// TODO: Comentar
int removeUpdateEvent(event_t *updateEvents, event_t event){
	
	int found = 0;
	
	// TODO: Poner exclusión mutua
	//bajar
	for(i in(1, MAX_UPDATE_HANDLERS) && !found){
		// TODO: Cambiar este "found" a algo que diga si son iguales (¿por una comparación hash?, ¿elemento a elemento?)
		if(found){
			updateEvents[i].handler = doNothing();
			updateEvents[i].condition = CONDITION_UNASSIGNED;
			found = 1;
		}
	}
	// subir
	
	return found-1
	
}