#ifndef MEU_BANCO_H
#define MEU_BANCO_H

/* ===================== CONSTANTES ===================== */
#define ARQUIVO_DADOS   "banco.dat"
#define NUM_INICIAL     1000      /* primeiro numero de conta           */
#define MAX_HIST        5         /* extrato guarda as ultimas 5 ops    */
#define TAM_NOME        50
#define TAM_SORTEIO     5         /* qtde de clientes no sorteio        */

#define TIPO_CORRENTE   1
#define TIPO_SALARIO    2
#define TIPO_POUPANCA   3

#define TITULO_MIN      1000.0    /* faixa de saldo p/ comprar titulo   */
#define TITULO_MAX      1500.0
#define VALOR_TITULO    100.0     /* custo do titulo (suposicao)        */

/* ===================== REGISTROS ===================== */
typedef struct {
    char   op;                    /* 'S', 'D' ou 'E' */
    double valor;
} Transacao;

typedef struct {
    int       ativa;              /* 1 = cliente possui essa conta      */
    int       tipo;               /* 1, 2 ou 3                          */
    double    saldo;
    int       qtdHist;
    Transacao hist[MAX_HIST];     /* historico: ultimas transacoes      */
} Conta;

typedef struct {
    char  nome[TAM_NOME];
    int   numero;                 /* 1000, 1001, ...                    */
    Conta contas[4];              /* indice = tipo (posicao 0 sem uso)  */
    int   emprestimo;             /* 1 = ja pegou emprestimo            */
    int   titulo;                 /* 1 = ja comprou titulo              */
} Cliente;

typedef struct {
    Cliente *clientes;            /* vetor alocado dinamicamente        */
    int      qtd;
    int      capacidade;
    int      aberto;
    int      proximoNumero;
    int      temVencedor;         /* dados do ultimo sorteio            */
    char     vencedorNome[TAM_NOME];
    int      vencedorNumero;
    double   vencedorPremio;
} Banco;

/* ===================== utils.c ===================== */
void   lerTexto(const char *msg, char *dest, int tam);
int    lerInt(const char *msg);
double lerDouble(const char *msg);
int    lerSimNao(const char *msg);
const char *nomeTipo(int tipo);

/* ===================== arquivo.c ===================== */
int  salvarBanco(Banco *b);
int  carregarBanco(Banco *b);

/* ===================== banco.c ===================== */
void inicializarBanco(Banco *b);
void liberarBanco(Banco *b);
void abrirBanco(Banco *b);
void cadastrarCliente(Banco *b);
void consultarHistorico(Banco *b);
void efetuarSorteio(Banco *b);
int  fibonacci(int n);

/* ===================== cliente.c ===================== */
void     acessarConta(Banco *b);
Cliente *buscarCliente(Banco *b, int numero);
double   saldoTotalCliente(Cliente *c);
double   limiteEmprestimo(Cliente *c);
int      aptoEmprestimo(Cliente *c);
int      aptoTitulo(Cliente *c);
void     registrarTransacao(Conta *conta, char op, double valor);
void     depositar(Banco *b, Cliente *c, int tipo);
void     sacar(Banco *b, Cliente *c, int tipo);
void     solicitarEmprestimo(Banco *b, Cliente *c);
void     adquirirTitulo(Banco *b, Cliente *c);
void     solicitarExtrato(Cliente *c, int tipo);
void     menuCliente(Banco *b, Cliente *c, int tipo);

#endif
