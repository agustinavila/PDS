#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

FILE *abrir_archivo(int i, char modo[], char extra[])
{
    FILE *archivo;
    char string[100];
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    snprintf(string, sizeof(string), "%s\\PDS\\DSP1\\Signal_0%d%s.txt", cwd, i, extra);
    archivo = fopen(string, modo);
    //printf("%s\n", string);
    if (archivo == NULL)
    {
        printf("NO SE PUEDE ABRIR EL ARCHIVO ESPECIFICADO\n");
    }
    //fclose(archivo); //tengo que cerrarlo?
    return archivo;
}

int contar_lineas(FILE *archivo)
{
    int n = 0;
    float numero;
    while (!feof(archivo))
    {
        fscanf(archivo, "%f\n", &numero);

        n++;
    }
    rewind(archivo);
    return n;
}
float valor_medio(FILE *arch)
{
    int n = 0;
    float suma = 0, actual = 0;
    fscanf(arch, "%f\n", &actual); //salta la frecuencia
    while (!feof(arch))
    {
        fscanf(arch, "%f\n", &actual);
        suma += actual;
        n++;
    }
    rewind(arch);
    return (suma / n);
}
void resta_vmedio(FILE *archivo_original, int i, float vm)
{
    float valor;
    FILE *archivo_nuevo;
    archivo_nuevo = abrir_archivo(i, "w+", "Vmedio");
    fscanf(archivo_original, "%f\n", &valor);
    fprintf(archivo_nuevo, "%.1f\n", valor); //las frecuencias
    while (!feof(archivo_original))
    {
        fscanf(archivo_original, "%f\n", &valor);
        fprintf(archivo_nuevo, "%.1f\n", valor - vm);
    }
    fclose(archivo_nuevo);
    rewind(archivo_original);
}
float interpola_lineal(float x1, float x2, float y1, float y2, float x)
{
    return ((((y2 - y1) * (x - x1)) / (x2 - x1)) + y1);
}
void adapta_signals(FILE *arch1, FILE *arch2, float *out_s1, float *out_s2, int *cant_muestras, float *freq_muestreo)
{
    int cantmin, cant1, cant2;
    long i, j = 0;
    float freq1, freq2, *sig1, *sig2, *n1, *n2, tmin, t1, t2;
    cant1 = contar_lineas(arch1) - 1; //cuenta la cantidad de valores que tiene menos el de frecuencia
    cant2 = contar_lineas(arch2) - 1;
    cantmin = (cant1 >= cant2) ? cant2 : cant1;
    ///*********lectura para valor correspondiente a la freq*****
    fscanf(arch1, "%f\n", &freq1);                     //leo frecuencia 1
    fscanf(arch2, "%f\n", &freq2);                     //leo frecuencia 2
    *freq_muestreo = (freq1 >= freq2) ? freq1 : freq2; //la frecuencia de muestreo es la maximo de las dos
    printf("%f\n", *freq_muestreo);
    n1 = (float *)malloc((cant1) * sizeof(float)); // DIMENSIONO ARREGLOS
    sig1 = (float *)malloc((cant1) * sizeof(float));
    n2 = (float *)malloc((cant2) * sizeof(float));
    sig2 = (float *)malloc((cant2) * sizeof(float));

    /// GENERO ARREGLO DE TIEMPO MUESTREO Y CARGO DATOS DEL ARCHIVO DE SE�ALES AL ARREGLO
    for (i = 0; i < cant1; i++)
    {
        n1[i] = i / freq1; //genero el vector de tiempo
        fscanf(arch1, "%f\n", &sig1[i]);
        //printf("%d\t%f\t%f\n", i,sig1[i], n1[i]);
    }
    rewind(arch1);

    for (i = 0; i < cant2; i++)
    {
        n2[i] = i / freq2; //genero el vector de tiempo
        fscanf(arch2, "%f\n", &sig2[i]);
        //printf("%d\t%f\t%f\n", i,sig2[i], n2[i]);
    }
    rewind(arch2);
    t1 = n1[cant1 - 1];          //tiempo final 1
    t2 = n2[cant2 - 1];          //tiempo final 2
    tmin = (t1 <= t2) ? t1 : t2; //el minimo tiempo
    *cant_muestras = (tmin) * (*freq_muestreo);
    printf("frecuencia:%f\t cant_muestras:%d\t tmin:%f, %f, %f\n", *freq_muestreo, *cant_muestras, tmin, t1, t2);
    out_s1 = (float *)malloc(*cant_muestras * sizeof(float));
    out_s2 = (float *)malloc(*cant_muestras * sizeof(float));
    if (freq1 == freq2)
    {
        j = 0;
        printf("eskereeeee");
        // ******************

        *cant_muestras = cantmin;
        //*****************
        for (i = 0; i < cantmin; i++)
        {
            out_s1[i] = sig1[i];
            out_s2[i] = sig2[i];
        }
    }
    //****************************************************************************************

    if (freq2 > freq1)
    {
        j = 0;
        for (i = 0; n1[i] < tmin; i++)
        {
            //printf("%d\n",i);
            while ((n2[j] >= n1[i]) && (n2[j] <= n1[i + 1]))
            {
                out_s1[j] = interpola_lineal(n1[i], n1[i + 1], sig1[i], sig1[i + 1], n2[j]);
                out_s2[j] = sig2[j];
                //printf("%f\n", out_s1);
                j++;
            }
        }
    }
    else
    {
        j = 0;
        for (i = 0; n2[i] < tmin; i++)
        {
            //printf("%d\n", i);
            while ((n1[j] >= n2[i]) && (n1[j] < n2[i + 1]))
            {
                out_s2[j] = interpola_lineal(n2[i], n2[i + 1], sig2[i], sig2[i + 1], n1[j]);
                out_s1[j] = sig1[j];
                //printf("%f\n", out_s2);
                j++;
            }
        }
    }
}
void Suma_Resta(FILE *archivo1, FILE *archivo2)
{
    int cant_muestras,i;
    float freq_muestreo, *signal1, *signal2;
    FILE *suma, *resta;
    signal1 = malloc(contar_lineas(archivo1) * sizeof(float));
    signal2 = malloc(contar_lineas(archivo2) * sizeof(float));
    suma = abrir_archivo(12, "w+", "Suma");
    resta = abrir_archivo(12, "w+", "Resta");
    adapta_signals(archivo1, archivo2, signal1, signal2, &cant_muestras, &freq_muestreo);
    fprintf(suma, "%.1f\n", freq_muestreo);
    fprintf(resta, "%.1f\n", freq_muestreo);
    for (i = 0; i < cant_muestras; i++)
    {
        fprintf(suma, "%.1f\n", signal1[i] + signal2[i]);
        fprintf(resta, "%.1f\n", signal1[i] - signal2[i]);
    }
    fclose(resta);
    fclose(suma);
    free(signal1);
    free(signal2);
}
float freq_analogica(FILE *arch, float mayor, float menor)
{
    int n = 0, bandera = 0; // n es la frecuencia digital
    float valor, Fs;
    fscanf(arch, "%f\n", &Fs); //frecuencia de muestreo

    while ((!feof(arch)) && (!bandera))
    {
        fscanf(arch, "%f\n", &valor);
        if (valor == mayor)
        {

            bandera = 1; //indico que encontre el 1er maximo
            n++;
            while ((!feof(arch)) && bandera)
            { //cuento hasta encontrar el primer minimo y salgo del bucle
                fscanf(arch, "%f\n", &valor);
                if (valor == menor)
                    bandera = 0; //indico que encontre el minimo consecutivo al mayor
                n++;
            }

            while ((!feof(arch)) && (!bandera))
            { //sigo contando hasta encontrar nuevamente el 1er maximo
                fscanf(arch, "%f\n", &valor);
                if (valor == mayor)
                    bandera = 1;
                n++;
            }

        } //terino el ciclo de cuenta
    }
    rewind(arch);
    return (Fs / (n - 1));
}
void max_min_freq(FILE *arch, int i)
{
    float maximo = -10000.0, minimo = 10000.0;
    float valor, Fs, freq_analog;
    fscanf(arch, "%f\n", &Fs); //guardo la frecuencia de muestreado
    while (!feof(arch))
    {
        fscanf(arch, "%f\n", &valor);
        if (valor >= maximo)
            maximo = valor;
        if (valor <= minimo)
            minimo = valor;
    }
    rewind(arch);
    freq_analog = 1;
    freq_analog = freq_analogica(arch, maximo, minimo);
    printf("MIN: %.1fV\tMAX: %.1fV\tFREQ ANALOG: %.1fHz\n", minimo, maximo, freq_analog);
}

void correlacion(FILE *arch1, FILE *arch2, char nombre[])
{

    int cant_muestras, i, k, son_iguales = 0, n1, n2;
    float *s1, *s2, freq_muestreo, dato, freq1, freq2;
    double correlacion;
    FILE *arch_nuevo;

    n1 = contar_lineas(arch1) - 1;
    n2 = contar_lineas(arch2) - 1;
    s1 = (float *)malloc(n1 * sizeof(float));
    s2 = (float *)malloc(n2 * sizeof(float));
    fscanf(arch1, "%f\n", &freq1);
    fscanf(arch2, "%f\n", &freq2);
    // copio los archivos en arreglos
    for (i = 0; i < n1; i++)
        fscanf(arch1, "%f\n", &s1[i]);
    for (i = 0; i < n2; i++)
        fscanf(arch2, "%f\n", &s2[i]);
    // listo.
    rewind(arch1);
    rewind(arch2);

    arch_nuevo = abrir_archivo(12, "w","correlacion");

    if (n1 == n2)
    {                    // si tienen el mismo largo, podrian ser iguales, analizemos..
        son_iguales = 1; // podrian ser iguales, veamos..
        for (i = 0; ((i < n1) && (son_iguales)); i++)
        {
            if (s1[i] != s2[i])
                son_iguales = 0; // no son iguales
        }
    }

    if (son_iguales)
    {
        fprintf(arch_nuevo, "%.1f\n", freq1); // guardo la frecuencia de muestreo

        for (k = 0; k < n1; k++)
        { // con este for genero el retardo
            correlacion = 0;
            for (i = 0; i < n1 - k; i++)
                correlacion += s1[i] * s2[i + k];

            fprintf(arch_nuevo, "%lf\n", correlacion); //guardo el coeficiente de autocorrelacion para el elemento [i]
        }
    }

    else
    {
        adapta_signals(arch1, arch2, s1, s2, &cant_muestras, &freq_muestreo);
        fprintf(arch_nuevo, "%.1f\n", freq_muestreo); // guardo la freq de muestreo
                                                      //********* calculo el valor medio para las nueva sse�ales interpoladas

        for (k = 0; k < cant_muestras; k++)
        { // con este for genero el retardo
            correlacion = 0;
            for (i = 0; i < cant_muestras - k; i++)
                correlacion += s1[i] * s2[i + k];

            fprintf(arch_nuevo, "%lf\n", correlacion); //guardo el coeficiente de autocorrelacion para el elemento [i]
        }
    }

    fclose(arch_nuevo);
    free(s1);
    free(s2);
} // fin convolucion

int main()
{
    FILE *arch, *arch2;
    float medio;
    int i;
    //abre los archivos
    for (i = 1; i < 6; i++)
    {
        printf("\nSignal %d\n", i);
        arch = abrir_archivo(i, "r", "");
        medio = valor_medio(arch);
        printf("v%imedio= %f\n", i, medio);
        resta_vmedio(arch, i, medio);
        max_min_freq(arch, i);
        fclose(arch);
    }
    
    arch = abrir_archivo(1, "r", "");
    arch2 = abrir_archivo(2, "r", "");
    Suma_Resta(arch, arch2);
    //correlacion(arch2,arch2,"2");
    //sumar y restar dos señales y guardar resultados

    //realizar autocorrelacion y correlacion entre 2
    //graficar en matlab xd
}