/* Persistencia dos dados em arquivo binario */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

int salvarBanco(Banco *b)
{
    FILE *f = fopen(ARQUIVO_DADOS, "wb");
    if (f == NULL) {
        printf("Erro ao salvar o arquivo de dados!\n");
        return 0;
    }
    fwrite(&b->qtd, sizeof(int), 1, f);
    fwrite(&b->proximoNumero, sizeof(int), 1, f);
    fwrite(&b->temVencedor, sizeof(int), 1, f);
    fwrite(b->vencedorNome, sizeof(char), TAM_NOME, f);
    fwrite(&b->vencedorNumero, sizeof(int), 1, f);
    fwrite(&b->vencedorPremio, sizeof(double), 1, f);
    fwrite(b->clientes, sizeof(Cliente), b->qtd, f);
    fclose(f);
    return 1;
}

int carregarBanco(Banco *b)
{
    FILE *f = fopen(ARQUIVO_DADOS, "rb");
    if (f == NULL)
        return 0;                       /* primeira execucao: sem arquivo */

    fread(&b->qtd, sizeof(int), 1, f);
    fread(&b->proximoNumero, sizeof(int), 1, f);
    fread(&b->temVencedor, sizeof(int), 1, f);
    fread(b->vencedorNome, sizeof(char), TAM_NOME, f);
    fread(&b->vencedorNumero, sizeof(int), 1, f);
    fread(&b->vencedorPremio, sizeof(double), 1, f);

    b->capacidade = (b->qtd > 4) ? b->qtd : 4;
    b->clientes = (Cliente *) malloc(b->capacidade * sizeof(Cliente));
    if (b->clientes == NULL) {
        printf("Memoria insuficiente!\n");
        fclose(f);
        exit(1);
    }
    fread(b->clientes, sizeof(Cliente), b->qtd, f);
    fclose(f);
    return 1;
}
