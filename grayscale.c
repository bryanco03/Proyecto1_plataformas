#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "rw_png.c"
#include "rw_jpg.c"


//Revisar si es PNG
bool es_PNG(char * input_path){
	FILE * file = fopen(input_path, "r");
	if (file == NULL){
		return false;
	}

	unsigned char bytes[4];

	fread(bytes, 4, 1, file);
	if(bytes[0] == 0x89 && bytes[1] == 0x50 && bytes[2] == 0x4e && bytes[3] == 0x47){
		return true;
	}
	else{
		return false;
	}

}

//Revisar si es JPEG
bool es_JPEG(char * input_path){
	FILE * file = fopen(input_path, "r");
	if (file == NULL){
		return false;
	}

	unsigned char bytes[3];
	fread(bytes, 3, 1, file);

	if(bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff){
		return true;
	}
	else{
		return false;
	}
}



//Imprime mensaje de error en argumentos
void error_arg(){
	printf("Hay un error con los argumentos ingresados\n");
}

//Imprime menu de ayuda dado por argumento en lindea de comando -h
void print_help(){
	printf("Menu de ayuda\n");
	printf("Programa toma como entrada una imagen en formato PNG o JPEG y le aplica una transformacion a escala de grises\n");
	printf("-h: Abre el menu de ayuda");
	printf("-o: Establede el path del archivo a procesar. En caso de no utiliza, tomara como path al primer argumento sin identificador '-' dado\n");
	printf("-i: Establece el path de salida para el archivo procesado. En caso de no utilizar, se muestra un mensaje de error solicitando un path de salida\n");
}

/*Determina si es PNG o JPEG
 *Ejecuta el programa respectivo 
 */
int ejecutar( char * input_path, char * output_path){
	
	if( es_PNG(input_path) ){
		grayscalePNG(input_path, output_path);	
		return 0;
	}
	else if( es_JPEG(input_path) ){
		grayscaleJPEG(input_path, output_path);
		return 0;
	}
	else{
		printf("Debe ingresar una imagen en formato PNG o JPEG\n");
	       	return 1;	
	}
}


/* Recibe como parametros:
 * -h: Ayuda
 * -i: Path de imagen a ingresar
 * -o: Path de imagen de salida
 */
int main( int argc, char *argv[] ){
// Revisar argumentos ingresados	
	if(argc > 1){
		char * input_path;
		char * output_path;
		
		bool hay_entrada = false;
       		bool hay_salida = false;
		
		//Revisar argumentos
		for (int i = 1; i < argc; i++){
			if( !strcmp(argv[i],"-h") ){ //Imprimir menu de ayuda
				print_help(); 
				return 0;
			}	
			else if( !strcmp(argv[i],"-i") ){ //Agregar path de salida
				if( (i+1) < argc && !hay_entrada ){
					input_path = argv[i+1];
					i++;
					hay_entrada = true;
				}
				else{
					error_arg();
					return 1;
				}			
			}
			else if( !strcmp(argv[i],"-o") ){ //Agregar path salida
				if( (i+1) < argc && !hay_salida){
					output_path = argv[i+1];
					i++;
					hay_salida = true;
				}	
				else{
					error_arg();
					return 1;
				}
			}
			else if( !hay_entrada ){
				input_path = argv[i];
				i++;
				hay_entrada = true;
			}
			else{
				error_arg();
				return 1;
			}
		}

		if( !hay_salida ){
			printf("Debe ingresar un path de salida utilizando el argumento -o en linea de comando\n");
		}	
		if( hay_entrada && hay_salida ){		
			ejecutar(input_path, output_path);	
		}
		
	}
	else {
		printf("Debe ingresar algun argumento");
		return 1;
	}
}

