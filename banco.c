/* Regras do banco: abertura, cadastro, relatorio e sorteio */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

void inicializarBanco(Banco *b)
{
    b->clientes       = NULL;
    b->qtd            = 0;
    b->capacidade     = 0;
    b->aberto         = 0;
    b->proximoNumero  = NUM_INICIAL;
    b->temVencedor    = 0;
    b->vencedorNome[0] = '\0';
    b->vencedorNumero = 0;
    b->vencedorPremio = 0;
}

void liberarBanco(Banco *b)
{
    free(b->clientes);
    b->clientes = NULL;
    b->qtd = b->capacidade = 0;
}

void abrirBanco(Banco *b)
{
    if (b->aberto) {
        printf("O banco ja esta aberto.\n");
        return;
    }
    if (carregarBanco(b))
        printf("Banco aberto! %d cliente(s) carregado(s).\n", b->qtd);
    else
        printf("Banco aberto! Nenhum dado anterior, comecando do zero.\n");
    b->aberto = 1;
}

void cadastrarCliente(Banco *b)
{
    Cliente c;
    int t;

    memset(&c, 0, sizeof(Cliente));
    lerTexto("Nome do cliente: ", c.nome, TAM_NOME);
    if (c.nome[0] == '\0') {
        printf("Nome invalido.\n");
        return;
    }

    for (t = TIPO_CORRENTE; t <= TIPO_POUPANCA; t++) {
        char msg[64];
        sprintf(msg, "Abrir conta %s?", nomeTipo(t));
        if (lerSimNao(msg)) {
            c.contas[t].ativa = 1;
            c.contas[t].tipo  = t;
            c.contas[t].saldo = 0.0;        /* saldo inicial zerado */
        }
    }

    if (c.contas[TIPO_CORRENTE].ativa && c.contas[TIPO_SALARIO].ativa) {
        printf("Erro: nao e permitido ter conta corrente e salario ao mesmo tempo.\n");
        return;
    }
    if (!c.contas[TIPO_CORRENTE].ativa && !c.contas[TIPO_SALARIO].ativa &&
        !c.contas[TIPO_POUPANCA].ativa) {
        printf("Erro: o cliente precisa de ao menos uma conta.\n");
        return;
    }

    /* cresce o vetor dinamicamente quando lota */
    if (b->qtd == b->capacidade) {
        int novaCap = (b->capacidade == 0) ? 4 : b->capacidade * 2;
        Cliente *novo = (Cliente *) realloc(b->clientes, novaCap * sizeof(Cliente));
        if (novo == NULL) {
            printf("Memoria insuficiente!\n");
            return;
        }
        b->clientes = novo;
        b->capacidade = novaCap;
    }

    c.numero = b->proximoNumero++;
    b->clientes[b->qtd++] = c;
    salvarBanco(b);

    printf("Cliente cadastrado! Contas criadas:\n");
    for (t = TIPO_CORRENTE; t <= TIPO_POUPANCA; t++)
        if (c.contas[t].ativa)
            printf("  %04d-%02d (%s)\n", c.numero, t, nomeTipo(t));
}

void consultarHistorico(Banco *b)
{
    int i;
    double total = 0;

    for (i = 0; i < b->qtd; i++)
        total += saldoTotalCliente(&b->clientes[i]);

    printf("\n== Saldo Total do banco ==\n");
    printf("%d | R$ %.2f\n", b->qtd, total);

    printf("== Clientes Aptos a Emprestimos ==\n");
    for (i = 0; i < b->qtd; i++) {
        Cliente *c = &b->clientes[i];
        if (aptoEmprestimo(c))
            printf("%s|%04d-%02d|R$ %.2f\n", c->nome, c->numero,
                   TIPO_CORRENTE, limiteEmprestimo(c));
    }

    /* o enunciado diz "Conta Salario", mas so a corrente pode ficar negativa */
    printf("== Clientes com Saldo Negativo na Conta Corrente ==\n");
    for (i = 0; i < b->qtd; i++) {
        Cliente *c = &b->clientes[i];
        if (c->contas[TIPO_CORRENTE].ativa && c->contas[TIPO_CORRENTE].saldo < 0)
            printf("%s|%04d-%02d|R$ %.2f\n", c->nome, c->numero,
                   TIPO_CORRENTE, c->contas[TIPO_CORRENTE].saldo);
    }

    printf("== Clientes Aptos a Compra de Titulos ==\n");
    for (i = 0; i < b->qtd; i++) {
        Cliente *c = &b->clientes[i];
        if (aptoTitulo(c))
            printf("%s|%04d-%02d|R$ %.2f\n", c->nome, c->numero,
                   TIPO_SALARIO, c->contas[TIPO_SALARIO].saldo);
    }

    printf("== Clientes Contemplado no Sorteio ==\n");
    if (b->temVencedor)
        printf("%s|%04d-%02d|R$ %.2f\n", b->vencedorNome, b->vencedorNumero,
               TIPO_SALARIO, b->vencedorPremio);
}

int fibonacci(int n)           /* fib(1)=1, fib(2)=1, fib(3)=2, ... */
{
    int a = 1, b = 1, t, i;
    for (i = 3; i <= n; i++) {
        t = a + b;
        a = b;
        b = t;
    }
    return b;
}

void efetuarSorteio(Banco *b)
{
    Cliente *top[TAM_SORTEIO];
    char usado[1000] = {0};        /* marca quem ja entrou no top (limite didatico) */
    int pesos[TAM_SORTEIO], soma = 0, i, j, melhor, sorteado, acum, venc = 0;
    Conta *sal;

    if (b->qtd < TAM_SORTEIO) {
        printf("E preciso ter ao menos %d clientes cadastrados.\n", TAM_SORTEIO);
        return;
    }
    if (b->qtd > 1000) {
        printf("Limite de clientes do sorteio excedido.\n");
        return;
    }

    /* seleciona os 5 maiores saldos (selecao repetida) */
    for (i = 0; i < TAM_SORTEIO; i++) {
        melhor = -1;
        for (j = 0; j < b->qtd; j++) {
            if (usado[j]) continue;
            if (melhor == -1 ||
                saldoTotalCliente(&b->clientes[j]) > saldoTotalCliente(&b->clientes[melhor]))
                melhor = j;
        }
        usado[melhor] = 1;
        top[i] = &b->clientes[melhor];
    }

    /* todos precisam ter titulo */
    for (i = 0; i < TAM_SORTEIO; i++) {
        if (!top[i]->titulo) {
            printf("Sorteio cancelado: %s (top %d) nao possui titulo de capitalizacao.\n",
                   top[i]->nome, i + 1);
            return;
        }
    }

    /* pesos de Fibonacci: 1o colocado = fib(5)=5, ... 5o = fib(1)=1 */
    for (i = 0; i < TAM_SORTEIO; i++) {
        pesos[i] = fibonacci(TAM_SORTEIO - i);
        soma += pesos[i];
    }
    sorteado = rand() % soma + 1;
    acum = 0;
    for (i = 0; i < TAM_SORTEIO; i++) {
        acum += pesos[i];
        if (sorteado <= acum) {
            venc = i;
            break;
        }
    }

    sal = &top[venc]->contas[TIPO_SALARIO];
    b->vencedorPremio = 10.0 * sal->saldo;           /* 10x o saldo atual */
    sal->saldo += b->vencedorPremio;
    registrarTransacao(sal, 'D', b->vencedorPremio);

    strcpy(b->vencedorNome, top[venc]->nome);
    b->vencedorNumero = top[venc]->numero;
    b->temVencedor = 1;
    salvarBanco(b);

    printf("\nParticipantes (peso Fibonacci):\n");
    for (i = 0; i < TAM_SORTEIO; i++)
        printf("  %d. %s (peso %d)\n", i + 1, top[i]->nome, pesos[i]);
    printf("Numero sorteado: %d de %d\n", sorteado, soma);
    printf("VENCEDOR: %s! Premio de R$ %.2f creditado na conta %04d-%02d.\n",
           b->vencedorNome, b->vencedorPremio, b->vencedorNumero, TIPO_SALARIO);
}
