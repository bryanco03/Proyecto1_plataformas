#include <stdio.h>
#include <jpeglib.h>
#include <stdlib.h>

// Función para leer un archivo JPEG y almacenar los píxeles en un búfer
int leerImagenJPEG(const char *nombreArchivo, unsigned char **buffer, int *width, int *height, int *numChannels) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile;

    // Abre el archivo JPEG para lectura en modo binario
    if ((infile = fopen(nombreArchivo, "rb")) == NULL) {
        printf("No se pudo abrir el archivo de imagen.\n");
        return 1;
    }

    // Configura el manejador de errores de libjpeg
    cinfo.err = jpeg_std_error(&jerr);

    // Inicializa la estructura de descompresión de JPEG
    jpeg_create_decompress(&cinfo);

    // Asocia el archivo de entrada con el manejador de JPEG
    jpeg_stdio_src(&cinfo, infile);

    // Lee la cabecera de la imagen JPEG
    jpeg_read_header(&cinfo, TRUE);

    // Comienza el proceso de descompresión
    jpeg_start_decompress(&cinfo);

    // Variables para almacenar la información de la imagen
    *width = cinfo.output_width;
    *height = cinfo.output_height;
    *numChannels = cinfo.output_components;
    int row_stride = *width * *numChannels;

    // Buffer para almacenar los píxeles de la imagen
    *buffer = (unsigned char *)malloc(row_stride * *height);

    // Itera a través de las filas de la imagen y almacena los píxeles en el buffer
    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *row_ptr = (*buffer) + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, &row_ptr, 1);
    }

    // Finaliza el proceso de descompresión
    jpeg_finish_decompress(&cinfo);

    // Libera los recursos de libjpeg
    jpeg_destroy_decompress(&cinfo);

    // Cierra el archivo
    fclose(infile);

    return 0;
}



int escribirImagenJPEG(const char *nombreArchivo, unsigned char *buffer, int width, int height, int numChannels) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    FILE *archivo = fopen(nombreArchivo, "wb");
    if (!archivo) {
        fprintf(stderr, "No se pudo abrir el archivo para escritura.\n");
        return 1;
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, archivo);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = numChannels;
    cinfo.in_color_space = numChannels == 3 ? JCS_RGB : JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE); // Ajusta la calidad de compresión (0-100)

    jpeg_start_compress(&cinfo,TRUE);

    JSAMPROW row_pointer[1];
    int row_stride = width * numChannels;

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(archivo);
    jpeg_destroy_compress(&cinfo);

    return 0;
}


void convertirAGrises(unsigned char *buffer, int width, int height, int numChannels) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int suma = 0;
            for (int c = 0; c < numChannels; c++) {
                suma += buffer[i * width * numChannels + j * numChannels + c];
            }
            int valorGrises = suma / numChannels;
            for (int c = 0; c < numChannels; c++) {
                buffer[i * width * numChannels + j * numChannels + c] = valorGrises;
            }
        }
    }
}







int main() {
    unsigned char *buffer;
    int width, height, numChannels;

    if (leerImagenJPEG("Las-imagenes-raw-son-los-negativos-digitales.jpg", &buffer, &width, &height, &numChannels) == 0) {
        // Procesa la imagen leída
        // printf("Ancho: %d, Alto: %d, Canales: %d\n", width, height, numChannels);
        
        convertirAGrises(buffer, width, height, numChannels);
        escribirImagenJPEG("imagen_salida.jpg", buffer, width, height, numChannels);
        

        // Liberar memoria 
        free(buffer);
    }

    return 0;
}

