# Definición del target
proyecto: grayscale.c
# Comando para compilar el programa
	gcc -o proyecto grayscale.c -lpng -ljpeg
# Imprime mensaje de confirmación
	@echo "Compilación exitosa. Ejecutable creado: proyecto"
# Regla phony para borrar el ejecutable
.PHONY: clean
clean:
	rm -f proyecto
	
