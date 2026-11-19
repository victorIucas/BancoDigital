/* Funcoes auxiliares de entrada de dados */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

void lerTexto(const char *msg, char *dest, int tam)
{
    printf("%s", msg);
    if (fgets(dest, tam, stdin) == NULL) {
        dest[0] = '\0';
        return;
    }
    dest[strcspn(dest, "\n")] = '\0';
}

int lerInt(const char *msg)
{
    char buf[64];
    int v;
    printf("%s", msg);
    if (fgets(buf, sizeof(buf), stdin) == NULL || sscanf(buf, "%d", &v) != 1)
        return -1;
    return v;
}

double lerDouble(const char *msg)
{
    char buf[64];
    double v;
    printf("%s", msg);
    if (fgets(buf, sizeof(buf), stdin) == NULL || sscanf(buf, "%lf", &v) != 1)
        return -1;
    return v;
}

int lerSimNao(const char *msg)
{
    char buf[16];
    printf("%s (s/n): ", msg);
    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;
    return (buf[0] == 's' || buf[0] == 'S');
}

const char *nomeTipo(int tipo)
{
    if (tipo == TIPO_CORRENTE) return "Corrente";
    if (tipo == TIPO_SALARIO)  return "Salario";
    return "Poupanca";
}
