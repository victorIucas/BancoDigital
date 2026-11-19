#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "meu_banco.h"

int main(void)
{
    Banco banco;
    int op;

    srand((unsigned) time(NULL));
    inicializarBanco(&banco);

    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        printf("1. Abrir o banco\n");
        printf("2. Consultar historico\n");
        printf("3. Efetuar sorteio\n");
        printf("4. Cadastrar cliente\n");
        printf("5. Acessar conta (cliente)\n");
        printf("0. Sair\n");
        op = lerInt("Opcao: ");

        if (op >= 2 && op <= 5 && !banco.aberto) {
            printf("O banco precisa estar aberto! Use a opcao 1.\n");
            continue;
        }

        switch (op) {
            case 1: abrirBanco(&banco);         break;
            case 2: consultarHistorico(&banco); break;
            case 3: efetuarSorteio(&banco);     break;
            case 4: cadastrarCliente(&banco);   break;
            case 5: acessarConta(&banco);       break;
            case 0: printf("Encerrando...\n");  break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);

    if (banco.aberto)
        salvarBanco(&banco);
    liberarBanco(&banco);
    return 0;
}
