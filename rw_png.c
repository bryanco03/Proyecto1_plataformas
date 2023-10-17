#include <stdlib.h>
#include <stdio.h>
#include <png.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

int width, height;
png_byte color_type;
png_byte bit_depth;
png_bytep *row_pointers = NULL;

void read_png_file(char *filename) {
  FILE *fp = fopen(filename, "rb");

  png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png) abort();

  png_infop info = png_create_info_struct(png);
  if(!info) abort();

  if(setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);

  png_read_info(png, info);

  width      = png_get_image_width(png, info);
  height     = png_get_image_height(png, info);
  color_type = png_get_color_type(png, info);
  bit_depth  = png_get_bit_depth(png, info);


  if(bit_depth == 16)
    png_set_strip_16(png);

  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);


  if(color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
    png_set_expand_gray_1_2_4_to_8(png);

  if(png_get_valid(png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png);


  if(color_type == PNG_COLOR_TYPE_RGB ||
     color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);

  if (row_pointers) abort();

  row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
  for(int y = 0; y < height; y++) {
    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
  }

  png_read_image(png, row_pointers);

  fclose(fp);

  png_destroy_read_struct(&png, &info, NULL);
}

void write_png_file(char *filename) {
  int y;

  FILE *fp = fopen(filename, "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) abort();

  png_infop info = png_create_info_struct(png);
  if (!info) abort();

  if (setjmp(png_jmpbuf(png))) abort();

  png_init_io(png, fp);


  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    width, height,
    8,
    PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);

  
  if (!row_pointers) abort();

  png_write_image(png, row_pointers);
  png_write_end(png, NULL);

  for(int y = 0; y < height; y++) {
    free(row_pointers[y]);
  }
  free(row_pointers);

  fclose(fp);

  png_destroy_write_struct(&png, &info);
}

void process_png_file() {
	int temp;

      	for(int y = 0; y < height; y++) {
    		png_bytep row = row_pointers[y];
    		for(int x = 0; x < width; x++) {
      			png_bytep px = &(row[x * 4]);
      			//Se toma un valor promedio de los valores RGB
			temp = (px[0] + 2*px[1] + px[3])/4;
      	
			//Se cambia cada valor RGB por el promedio tomado
			px[0] = temp;
			px[1] = temp;
			px[2] = temp;
			//printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
    		}
  	}	
}

//Imprime mensaje de error en argumentos
void error_arg(){
	printf("Hay un error con los argumentos ingrasados");
}

//Imprime menu de ayuda dado por argumento en linea de comando -h
void print_help(){
	printf("Menu de ayuda\n");
	printf("Programa toma como entrada una imagen en formato png y le aplica una transformacion a escala de grises\n");
	printf("-h: Abre el menu de ayuda\n");
	printf("-o: Establece el path del archivo a procesar. En caso de no utilizar, tomara como path al primer argumento sin identificador '-' dado\n");
	printf("-i: Establece el path de salida para el archivo procesado. En caso de no utilizar, se toma como path el mismo path de entrada\n");
}

//Programa principal
int main( int argc, char * argv[]) {
  
	if( argc > 1){
		char * input_path;
		char * output_path;

		bool hay_entrada = false;
		bool hay_salida = false;		
		
		//Revisar argumentos
		for( int i = 1; i < argc; i++){
			if( !strcmp(argv[i],"-h") ){
				print_help();
				return 0;
			}
			else if( !strcmp(argv[i],"-i") ){
				if( (i+1)<argc && !hay_entrada){
					input_path = argv[i+1];
					i++;
					hay_entrada = true;
				}
				else{
					error_arg();
					return 1;
				}
			}
			else if( !strcmp(argv[i],"-o") ){
				if( (i+1)<argc && !hay_salida ){
					output_path = argv[i+1];
					i++;
					hay_salida = true;
				}
				else{
					error_arg();
					return 1;
				}	
			}
			else if( !hay_entrada && !hay_salida ){
				input_path = argv[i];
				output_path = argv[i];
				hay_salida = true;
				hay_entrada = true;
			}
			else{
				error_arg();
				return 1;
			}	
		}
		if(!hay_salida){
			output_path = input_path;
			hay_salida = true;
		}	
		if(hay_entrada && hay_salida){
			read_png_file(input_path);
			process_png_file();
			write_png_file(output_path);
			return 0;			
		}
		else{
			printf("Error en la ejecucion");
			return 1;
		}
	}
	else{
		printf("Debe ingresar algun argumento");
		return 1;
	}
	
	
  	//read_png_file(argv[1]);
  	//process_png_file();
  	//write_png_file(argv[2]);

  	//return 0;
}

