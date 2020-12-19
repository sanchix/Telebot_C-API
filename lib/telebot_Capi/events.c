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
	for(i in(1,MAX_UPDATE_HANDLERS){
		uH[i].handler = doNothing();
	}
	
}

// Esta función debe bloquear los eventos (hay que poner un mutex)
void addUpdateEvent(){
	//TODO: Poner exclusión mutua
	//bajar
	
	//subir
}

void removeUpdateEvent(){
	
	
	
}