// Compiladores y Lenguajes de Bajo Nivel
// Analizador Léxico

// Integrantes:
// - Derlis Salinas
// - Félix Gómez

// Inclusión de cabecera
#include "anlex.h"

token t;
char cad[5*TAMLEX];

FILE *output, *fuente;	
char id[TAMLEX];
int numeroLinea=1;
char c=0;

void error(string mensaje) {
	printf("\nLinea %d. \tError lexico: %s.\n", numeroLinea, mensaje);
	// En caso de error, mientras no sea nueva linea, se ignoran errores léxicos
	while(fgetc(fuente) != '\n'){
		fgetc(fuente);		 
	}
	numeroLinea++;		
}

void sigLex() {
	int i=0;
	int estado=0;
	int acepto=0;
	char msg[5*TAMLEX];
	while((c=fgetc(fuente))!=EOF) {
		if (c==' ')
			continue;
		else if(c=='\t')		
			continue;
		else if(c=='\n') {
			// Se incrementa el numero de linea
			numeroLinea++;
		}
		else if (isalpha(c)) {
			// Se espera una palabra reservada
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(fuente);	   
			}while(isalpha(c));

			id[i]='\0';
			if (c != EOF){
				ungetc(c,fuente);
			} else 
				c=0;
			palabraReservada(id);
			t.lexema = id;
			break;
		}
		else if (c=='"'){
			// Se espera LITERAL_CADENA
			i=0;
			estado=0;
			acepto=0;
			id[i]=c;
			while(!acepto){
			 	switch(estado){
			 		case 0:
			 			c=fgetc(fuente);
			 			if(i>=TAMLEX) {
			 				estado = -1;
						}
			 			else if(c=='\t' || c=='\n' || c==EOF) {
			 				estado = -1;
			 			}
			 			else if(c=='"') {
			 				id[++i] = c;
			 				estado = 1;
			 			}
			 			else {
			 				id[++i]= c;
			 				estado=0;
			 			}
			 			break;
			 		case 1:
						c=0;
						id[++i]='\0';
						acepto=1;
						t.compLex=LITERAL_CADENA;
						t.lexema = id;
						t.componente = nombresComponentes[LITERAL_CADENA - 256]; 
						break;
					case -1:						
						if (c==EOF) {
							error("Fin de fuente inesperado");
						}
						else {
							ungetc(c,fuente);
							if(i>= TAMLEX) {
								error("Tamaño de Buffer menor a la longitud de identificador");
							}
							else if(c=='\n') {
								error("Salto de linea inesperado");
							}
							else if(c=='\t') { 
								error("Tabulacion inesperada");
							}
							t.componente = 0;
							acepto=1;
							break;
						}					
			 	} 
			}
			break;
		}
		else if (isdigit(c)) {
			// Se espera un numero
			i=0;
			estado=0;
			acepto=0;
			id[i]=c;
			while(!acepto) {
				switch(estado){ 
					// Se espera secuencia de digios o valores '.' o 'e'
					case 0:
						c=fgetc(fuente);
						if (isdigit(c)) {
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					// Se espera un digito
					// En caso de array, se espera un punto
					case 1:
						c=fgetc(fuente);						
						if (isdigit(c)) {
							id[++i]=c;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					// Se esperan digitos o 'e'
					case 2:
						c=fgetc(fuente);
						if (isdigit(c)) {
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e') {
							id[++i]=c;
							printf("%c = ",c);
							estado=3;
						}
						else
							estado=6;
						break;
					// Se esperan digitos, '+', o '-'
					case 3:
						c=fgetc(fuente);
						if (c=='+' || c=='-') {
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c)) {
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					// Se espera un digito o mas
					case 4:
						c=fgetc(fuente);
						if (isdigit(c)) {
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					// Se esperan digitos para exponente
					case 5:
						c=fgetc(fuente);
						if (isdigit(c)) {
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					// Aceptacion. Se devuelve caracter al componente lexico
					case 6:
						if (c!=EOF)
							ungetc(c,fuente);
						else
							c=0;
							id[++i]='\0';
							acepto=1;
							t.compLex=LITERAL_NUM;
							t.lexema = id;
							t.componente = nombresComponentes[LITERAL_NUM - 256]; 
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de fuente");
						else {
							t.componente = 0;
							sprintf(msg,"%c no esperado",c);
							error(msg);
							acepto=1;
						}					
						break;
				}
			}
			break;
		}
		else if (c==':') {
			t.compLex= DOS_PUNTOS;
			t.lexema = ":";			
			t.componente = nombresComponentes[DOS_PUNTOS - 256];
			break;
		}
		else if (c==',') {
			t.compLex= COMA;
			t.lexema = "," ;
			t.componente = nombresComponentes[COMA - 256];			
			break;
		}		
		else if (c=='[') {
			t.compLex=L_CORCHETE;
			t.lexema = "[";
			t.componente = nombresComponentes[L_CORCHETE - 256];
			break;
		}
		else if (c==']') {
			t.compLex=R_CORCHETE;
			t.lexema = "]";
			t.componente = nombresComponentes[R_CORCHETE - 256];
			break;
		}
		else if (c=='{') {
			t.compLex= L_LLAVE;
			t.lexema = "{";			
			t.componente = nombresComponentes[L_LLAVE - 256];
			break;		
		}
		else if (c=='}')
		{
			t.compLex= R_LLAVE;
			t.lexema = "}";			
			t.componente = nombresComponentes[R_LLAVE - 256];
			break;		
		}
		else if (c!=EOF) {
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF) {
		t.compLex=EOF;
	}
}

void palabraReservada(char id []){
   if (strcmp(id,"TRUE") == 0 || strcmp(id,"true") == 0 ) {
       t.compLex = PR_TRUE;
       t.lexema = id;
       t.componente = nombresComponentes[PR_TRUE - 256]; 
   	}else if(strcmp(id,"FALSE") == 0 || strcmp(id,"false") == 0 ) {
       t.compLex = PR_FALSE;
       t.lexema = id;
       t.componente = nombresComponentes[PR_FALSE - 256]; 
   	}else if(strcmp(id,"NULL") == 0 || strcmp(id,"null") == 0 ) {
       t.compLex = PR_NULL;
       t.lexema = id;
       t.componente = nombresComponentes[PR_NULL - 256]; 
   	}
   	else{
   		ungetc(c,fuente);
   		sprintf(cad,"No se esperaba '%s'",id);
   		t.componente =0;
       	error(cad);
    }
}

// Main
/*
int main(int argc,char* args[]) {
	// Se inicializa el analizador lexico
	if(argc > 1) {
		if (!(fuente=fopen(args[1],"rt"))) {
			printf("Archivo fuente no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			sigLex();
			if(t.componente)
       			printf("%s ", t.componente);
		}
		fclose(fuente);
	} else {
		printf("No se ha recibido la ubicacion del archivo fuente.\n");
		exit(1);
	}
	return 0;
}
*/