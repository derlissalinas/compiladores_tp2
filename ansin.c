// Compiladores y Lenguajes de Bajo Nivel
// Analizador Sintactico

// Integrantes:
// - Derlis Salinas
// - Félix Gómez

// Inclusión de cabeceras
#include "anlex.c"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#define CANT_COMP_LEX 10

// Prototipos
void json();
void element();
void array();
void array1();
void elementList();
void elementList1();
void object();
void object1();
void attribute();
void attributesList();
void attributesList1();
void attributeName();
void attributeValue(int []);
void match(int);
void errorSintactico(int []);
int aceptar=1;

/* BNF JSON 
    json -> element eof
    element -> object | array
    array -> [ array'
    array' -> element-list ] | ]
    element-list -> element element-list'
    element-list' ->  ,element element-list' | ε
    object -> { object'
    object' -> attributes-list} | }
    attributes-list -> attribute attributes-list'
    attributes-list' -> ,attribute attributes-list' | ε
    attribute -> attribute-name : attribute-value
    attribute-name -> string
    attribute-value -> element | string | number | true | false | null
*/

void match(int expectedToken){
    if(t.compLex == expectedToken){
        sigLex();
    }
  
}

void errorSintactico(int sincronizacion []) {
    aceptar=0;
   	printf("\nError sintactico en linea: %d. No se esperaba %s.\n",numeroLinea,t.componente);
	int i = 0;
    while(t.compLex != sincronizacion[i] && t.compLex != EOF){   
        if (sincronizacion[i] == '\0')
        {
            sigLex();
            i=0;        
        }
        i++;
    }
    sigLex();
    return;    	   
}

void check_input(int primero[], int siguiente[]) {
    int syncset[]={CANT_COMP_LEX};
    int i=0;
    if(t.compLex == EOF) return;    
    while(primero[i] != '\0') 
    {
        if(t.compLex == primero[i])
        {
            return;
        }
        i++;
    }
    i=0;    
    while(primero[i] != '\0'){ 
        syncset[i] = primero[i];
        i++;
    }
    int j=0;
    while(siguiente[j] != '\0'){
        syncset[i] = siguiente[j];
        i++;
        j++;
    }
    errorSintactico(syncset);

}

// json -> element eof
void json(){
    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    element(siguiente);
}

// element -> object | array
void element(int syncset[]){

    int primero[] = {L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);

    if(t.compLex == L_LLAVE){
        object(siguiente);
    }
    else if(t.compLex == L_CORCHETE){
        array(siguiente);
    }
    check_input(siguiente,primero);
}

// array -> [ array1
void array(int syncset[]){
    int primero[] = {L_CORCHETE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
   
    if(t.compLex == L_CORCHETE){
        match(L_CORCHETE);
        array1(siguiente);
    }
    check_input(siguiente,primero);
}

// array' -> element-list ] | ]
void array1(int syncset[]){
   
    int primero[]={L_CORCHETE,R_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {L_CORCHETE,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        elementList(siguiente);
        match(R_CORCHETE);
    }
    else if(t.compLex == R_CORCHETE){
        match(R_CORCHETE);
    }
    check_input(siguiente,primero);

}

// element-list -> element element-list'
void elementList(int syncset[]){
    
    int primero[]={L_CORCHETE,L_LLAVE,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element(siguiente);
        elementList1(siguiente);
    }
    check_input(siguiente,primero);
}

// element-list' ->  ,element element-list' | ε
void elementList1(int syncset[]){
    if(t.compLex == R_CORCHETE){ 
        return;
    }
    int primero[]={COMA,'\0'};
    int siguiente[] = {R_CORCHETE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == COMA){
        match(COMA);
        element(siguiente);
        elementList1(siguiente);
    }
    check_input(siguiente,primero);
}

// object -> { object'
void object(int syncset[]){
    int primero[]={L_LLAVE,'\0'};
    int siguiente[] = {COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_LLAVE){
        match(L_LLAVE);
        object1(siguiente);
    }
    check_input(siguiente,primero);
}

// object' -> attributes-list} | }
void object1(int syncset[]){
    int primero[]={LITERAL_CADENA,R_LLAVE,'\0'};
    int siguiente[] = {LITERAL_CADENA,COMA,R_CORCHETE,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == LITERAL_CADENA){
        attributesList(siguiente);
        match(R_LLAVE);
    }
    else if(t.compLex == R_LLAVE){
        match(R_LLAVE);
    }
    check_input(siguiente,primero);
}

// attributes-list -> attribute attributes-list'
void attributesList(int syncset[]){
    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == LITERAL_CADENA){
        attribute(siguiente);
        attributesList1(siguiente);
    }
    check_input(siguiente,primero);
}

// attributes-list' -> ,attribute attributes-list' | ε
void attributesList1(int syncset[]){
    if (t.compLex == R_LLAVE){
        return;
    }
    int primero[]={COMA,'\0'};
    int siguiente[] = {R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == COMA){
        match(COMA);
        attribute(siguiente);
        attributesList1(siguiente);
    }
    check_input(siguiente,primero);
}

// attribute -> attribute-name : attribute-value
void attribute(int syncset[]){
    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,siguiente);
    if(t.compLex == LITERAL_CADENA){
        attributeName(siguiente);
        match(DOS_PUNTOS);
        attributeValue(siguiente);
    }
    check_input(siguiente,primero);
}

// attribute-name -> string
void attributeName(int syncset[]){
    int primero[]={LITERAL_CADENA,'\0'};
    int siguiente[] = {DOS_PUNTOS, '\0'};
    check_input(primero,syncset);
    if(t.compLex == LITERAL_CADENA){
        match(LITERAL_CADENA);
    }
    check_input(siguiente,primero);
}

/*attribute-value -> element | string | number | true | false | null*/
void attributeValue(int syncset[]){
    int primero[]={L_CORCHETE,L_LLAVE,LITERAL_CADENA,LITERAL_NUM,PR_TRUE,PR_FALSE,PR_NULL,'\0'};
    int siguiente[] = {COMA,R_LLAVE, '\0'};
    check_input(primero,syncset);
    if(t.compLex == L_CORCHETE || t.compLex == L_LLAVE){
        element(siguiente);
    }
    else if(t.compLex == LITERAL_CADENA){
        match(LITERAL_CADENA);
    }
    else if(t.compLex == LITERAL_NUM){
        match(LITERAL_NUM);
    }
    else if(t.compLex == PR_TRUE){
        match(PR_TRUE);
    }
    else if(t.compLex == PR_FALSE){
        match(PR_FALSE);
    }
    else if(t.compLex == PR_NULL){
        match(PR_NULL);
    }
    check_input(siguiente,primero);
}

// Main
/*
int main (int argc,char* args[]) {
    if(argc > 1) {
        if (!(fuente=fopen(args[1],"rt"))) {
            printf("Archivo fuente no encontrado.\n");
            exit(1);
        }
        sigLex();
        json();
        if(aceptar) printf("Sintacticamente correcto\n");
        fclose(fuente);
    } else {
        printf("No se ha recibido la ubicacion del archivo fuente.\n");
        exit(1);
    }
    return 0;
}
*/