/* Regras e operacoes do cliente */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meu_banco.h"

Cliente *buscarCliente(Banco *b, int numero)
{
    int i;
    for (i = 0; i < b->qtd; i++)
        if (b->clientes[i].numero == numero)
            return &b->clientes[i];
    return NULL;
}

double saldoTotalCliente(Cliente *c)
{
    double total = 0;
    int t;
    for (t = TIPO_CORRENTE; t <= TIPO_POUPANCA; t++)
        if (c->contas[t].ativa)
            total += c->contas[t].saldo;
    return total;
}

double limiteEmprestimo(Cliente *c)
{
    return 0.25 * (10.0 * saldoTotalCliente(c));
}

int aptoEmprestimo(Cliente *c)
{
    if (c->emprestimo) return 0;
    if (!c->contas[TIPO_POUPANCA].ativa || !c->contas[TIPO_CORRENTE].ativa) return 0;
    if (c->contas[TIPO_POUPANCA].saldo <= 0) return 0;
    return limiteEmprestimo(c) > 0;
}

int aptoTitulo(Cliente *c)
{
    Conta *s = &c->contas[TIPO_SALARIO];
    if (c->titulo || !s->ativa) return 0;
    return (s->saldo >= TITULO_MIN && s->saldo <= TITULO_MAX);
}

void registrarTransacao(Conta *conta, char op, double valor)
{
    int i;
    if (conta->qtdHist == MAX_HIST) {            /* descarta a mais antiga */
        for (i = 1; i < MAX_HIST; i++)
            conta->hist[i - 1] = conta->hist[i];
        conta->qtdHist--;
    }
    conta->hist[conta->qtdHist].op    = op;
    conta->hist[conta->qtdHist].valor = valor;
    conta->qtdHist++;
}

void depositar(Banco *b, Cliente *c, int tipo)
{
    double v = lerDouble("Valor do deposito: R$ ");
    if (v <= 0) {
        printf("Valor invalido.\n");
        return;
    }
    c->contas[tipo].saldo += v;
    registrarTransacao(&c->contas[tipo], 'D', v);
    salvarBanco(b);
    printf("Deposito realizado. Saldo: R$ %.2f\n", c->contas[tipo].saldo);
}

void sacar(Banco *b, Cliente *c, int tipo)
{
    Conta *conta = &c->contas[tipo];
    double v = lerDouble("Valor do saque: R$ ");
    if (v <= 0) {
        printf("Valor invalido.\n");
        return;
    }
    /* so a conta corrente pode ficar negativa */
    if (tipo != TIPO_CORRENTE && v > conta->saldo) {
        printf("Saldo insuficiente.\n");
        return;
    }
    conta->saldo -= v;
    registrarTransacao(conta, 'S', v);
    salvarBanco(b);
    printf("Saque realizado. Saldo: R$ %.2f\n", conta->saldo);
}

void solicitarEmprestimo(Banco *b, Cliente *c)
{
    double limite, v;

    if (c->emprestimo) {
        printf("Voce ja possui um emprestimo (permitido apenas um).\n");
        return;
    }
    if (!c->contas[TIPO_CORRENTE].ativa || !c->contas[TIPO_POUPANCA].ativa) {
        printf("E preciso ter conta corrente e poupanca.\n");
        return;
    }
    if (c->contas[TIPO_POUPANCA].saldo <= 0) {
        printf("E preciso ter saldo na poupanca.\n");
        return;
    }
    limite = limiteEmprestimo(c);
    if (limite <= 0) {
        printf("Limite de emprestimo indisponivel.\n");
        return;
    }
    printf("Limite disponivel: R$ %.2f\n", limite);
    v = lerDouble("Valor do emprestimo: R$ ");
    if (v <= 0 || v > limite) {
        printf("Valor invalido ou acima do limite.\n");
        return;
    }
    c->contas[TIPO_CORRENTE].saldo += v;         /* credita na corrente */
    registrarTransacao(&c->contas[TIPO_CORRENTE], 'E', v);
    c->emprestimo = 1;
    salvarBanco(b);
    printf("Emprestimo de R$ %.2f creditado na conta corrente.\n", v);
}

void adquirirTitulo(Banco *b, Cliente *c)
{
    Conta *s = &c->contas[TIPO_SALARIO];

    if (c->titulo) {
        printf("Voce ja possui um titulo de capitalizacao.\n");
        return;
    }
    if (!aptoTitulo(c)) {
        printf("Para comprar e preciso ter conta salario com saldo entre R$ %.2f e R$ %.2f.\n",
               TITULO_MIN, TITULO_MAX);
        return;
    }
    s->saldo -= VALOR_TITULO;
    registrarTransacao(s, 'S', VALOR_TITULO);
    c->titulo = 1;
    salvarBanco(b);
    printf("Titulo adquirido por R$ %.2f. Saldo salario: R$ %.2f\n", VALOR_TITULO, s->saldo);
}

void solicitarExtrato(Cliente *c, int tipo)
{
    Conta *conta = &c->contas[tipo];
    int i;
    printf("== Extrato %04d-%02d (%s) ==\n", c->numero, tipo, nomeTipo(tipo));
    if (conta->qtdHist == 0)
        printf("Sem transacoes.\n");
    for (i = conta->qtdHist - 1; i >= 0; i--)         /* mais recente primeiro */
        printf("%.2f :%c\n", conta->hist[i].valor, conta->hist[i].op);
    printf("Saldo atual: R$ %.2f\n", conta->saldo);
}

void menuCliente(Banco *b, Cliente *c, int tipo)
{
    int op;

    /* alerta de saldo negativo na corrente, a cada acesso */
    if (c->contas[TIPO_CORRENTE].ativa && c->contas[TIPO_CORRENTE].saldo < 0)
        printf("\n*** ALERTA: sua conta corrente esta NEGATIVA (R$ %.2f)! ***\n",
               c->contas[TIPO_CORRENTE].saldo);

    printf("\nBem-vindo, %s!\n", c->nome);
    do {
        printf("\n--- Conta %04d-%02d | Saldo: R$ %.2f ---\n",
               c->numero, tipo, c->contas[tipo].saldo);
        printf("1. Depositar\n2. Sacar\n3. Solicitar emprestimo\n");
        printf("4. Adquirir titulos de capitalizacao\n5. Solicitar extrato\n0. Voltar\n");
        op = lerInt("Opcao: ");
        switch (op) {
            case 1: depositar(b, c, tipo);        break;
            case 2: sacar(b, c, tipo);            break;
            case 3: solicitarEmprestimo(b, c);    break;
            case 4: adquirirTitulo(b, c);         break;
            case 5: solicitarExtrato(c, tipo);    break;
            case 0: break;
            default: printf("Opcao invalida.\n");
        }
    } while (op != 0);
}

void acessarConta(Banco *b)
{
    char buf[32];
    int numero, tipo;
    Cliente *c;

    lerTexto("Numero da conta (ex: 1000-01): ", buf, sizeof(buf));
    if (sscanf(buf, "%d-%d", &numero, &tipo) != 2 ||
        tipo < TIPO_CORRENTE || tipo > TIPO_POUPANCA) {
        printf("Formato invalido.\n");
        return;
    }
    c = buscarCliente(b, numero);
    if (c == NULL || !c->contas[tipo].ativa) {
        printf("Conta nao encontrada.\n");
        return;
    }
    menuCliente(b, c, tipo);
}
