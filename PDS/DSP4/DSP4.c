//Juan Agustin Avila
// Julio 2020
//Reg 26076 - ELO

//TODO: revisar como pasarle argumentos
//TODO: resolver las funciones del filtro

#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
//#include <stdin.h>

FILE *abrir_archivo(char modo[], char nombre[], char extra[]) //abre los archivos en el modo indicado, se le puede agregar un sufijo al nombre
{
	FILE *archivo;
	char string[150];
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));											  //obtiene el directorio donde se esta ejecutando
	snprintf(string, sizeof(string), "%s\\%s%s.txt", cwd, nombre, extra); //siguen el patron original, solo les cambia el numero
	archivo = fopen(string, modo);
	if (archivo == NULL)
	{
		printf("No se pudo abrir el archivo \"%s%s.txt\"\n", nombre, extra); //no tiene mucho sentido porque no hace nada
	}
	else
	{
		printf("Se abrio el archivo \"%s%s.txt\" correctamente\n", nombre, extra); //no tiene mucho sentido porque no hace nada
	}
	return archivo;
}
int contar_lineas(FILE *archivo)
{
	int n = 0;
	float numero;
	while (!feof(archivo))
	{
		fscanf(archivo, "%f\n", &numero); //revisar esto
		printf("%.2f\n", numero);
		n++;
	}
	rewind(archivo);
	printf("%d\n", n);
	return n;
}
float *carga_filtro(FILE *archfiltro, int N)
{
	int i = 0;
	float *filtro = malloc(sizeof(float) * N);
	while (!feof(archfiltro))
	{
		fscanf(archfiltro, "%f\n", &filtro[i]); //revisar esto
		i++;
	}
	rewind(archfiltro);
	return filtro;
}

void filtrado(FILE *archivo_original, float filtro[], int N, char nombre[], char extra[])
// Los argumentos son: *archivo_original es el archivo a analizar
// offset es un offset a partir del cual se toman los datos
// N es la cantidad de puntos con los cuales realiza la transformada
// nombre es el nombre del archivo original
{
	float entrada[N], freq, salida = 0, in;
	int k = 0;
	FILE *archivo_nuevo;
	archivo_nuevo = abrir_archivo("w+", nombre, extra); //abre un nuevo archivo
	fscanf(archivo_original, "%f\n", &freq);			//obtiene la frecuencia
	fprintf(archivo_nuevo, "%.1f\n", freq);				//y la guarda en el nuevo
	for (k = 0; k < N; k++)								//inicializa el arreglo
	{
		entrada[k] = 0; //coloca todos los valores en cero
	}
	printf("termina de inicializar la entrada\n");
	while (!feof(archivo_original)) //lee una nueva linea
	{
		printf("entra al while\n");
		salida = 0;
		for (k = 1; k < N; k++)
		{
			entrada[k - 1] = entrada[k]; //desplaza los valores
		}

		fscanf(archivo_original, "%f", &entrada[N - 1]);
		printf("%.1f %.1f %.1f %.1f %.1f \n", entrada[0], entrada[1], entrada[2], entrada[3], entrada[4]);
		for (k = 0; k < N; k++)
		{
			salida = salida + (filtro[k] * entrada[k]); //realiza el calculo
		}
		fprintf(archivo_nuevo, "%.3f\n", salida);
	}

	fclose(archivo_nuevo); //cierra el archivo nuevo
	rewind(archivo_original);
}
int main(int argc, char *argv[])
{

	int N, i;
	FILE *arch;
	FILE *filtroarch;
	int *numeradorfiltro;
	//float *filtro;
	N = 5;
	//	float filtro[] = {0.255, 0.444, 1.645, 1.2351, 0.3333};
	//TODO: reemplazar por getopt
	if (argc == 3)
	{
		printf("Se aplicara el filtro %s a la señal %s\n", argv[1], argv[2]);
	}
	else if (argc > 3)
	{
		printf("Demasiados argumentos.\n");
		return (1);
	}
	else
	{
		printf("No se pasaron suficientes argumentos.\n\n");
		printf("El programa se debe correr de la siguiente manera:\n");
		printf("DSP3.exe filtro_a_aplicar archivo_a_analizar \n");
		printf("Por ejemplo para el archivo \"Tono_50Hz.txt\" con el filtro \"FiltroFIR_LP50Hz.txt\":\n");
		printf("\"DSP3.exe FiltroFIR_LP50Hz Tono_50Hz\"\n");
		printf("(No se debe incluir la terminacion .txt de los archivos)\n");
		return (1);
	}
	filtroarch = abrir_archivo("r", argv[1], ""); //abre archivo
	arch = abrir_archivo("r", argv[2], "");		  //abre archivo
	N = contar_lineas(filtroarch);
	//*filtro = malloc(sizeof(float) * N);
	float *filtro = carga_filtro(filtroarch, N);
	//denomfiltro=carga_filtro(archfiltro);
	filtrado(arch, filtro, N, argv[2], argv[1]);
	// transformada(arch, offset, N, argv[1]);
	fclose(arch);
	printf("Transformada calculada con exito.\n");
}