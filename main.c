/*
 * Sistema de Gerenciamento de Senhas de Atendimento Clinico
 * Linguagem: C (C99 ou superior)
 *
 * Regras didaticas:
 * - Urgente: idade superior a 80 anos.
 * - Preferencial: gestante, lactante, pessoa com necessidade especial,
 *   pessoa com obesidade, mulher com 60 anos ou mais ou homem com 65 anos ou mais.
 * - Normal: pacientes que nao se enquadram nas condicoes acima.
 *
 * As filas sao independentes e seguem FIFO. A prioridade de chamada e:
 * Urgente > Preferencial > Normal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define TAM_NOME 80
#define TAM_CATEGORIA 20

typedef struct {
    char nome[TAM_NOME];
    char genero; /* M = masculino, F = feminino, O = outro/nao informado */
    int idade;
    int gestante;
    int lactante;
    int necessidadeEspecial;
    int obeso;
    char codigo;
    int numero;
} Paciente;

typedef struct No {
    Paciente paciente;
    struct No *proximo;
} No;

typedef struct {
    No *inicio;
    No *fim;
    int quantidade;
} Fila;

typedef struct {
    Fila normal;
    Fila preferencial;
    Fila urgente;
    int proximoNormal;
    int proximoPreferencial;
    int proximoUrgente;
    int possuiEmAtendimento;
    Paciente emAtendimento;
} Sistema;

/* ---------- Entrada e validacao ---------- */

static void lerTexto(const char *mensagem, char *destino, size_t tamanho) {
    for (;;) {
        printf("%s", mensagem);
        if (fgets(destino, (int)tamanho, stdin) == NULL) {
            printf("\nEntrada encerrada. Finalizando.\n");
            exit(EXIT_SUCCESS);
        }

        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
                /* descarta o restante da linha */
            }
        }

        if (destino[0] != '\0') {
            return;
        }
        printf("Entrada obrigatoria. Tente novamente.\n");
    }
}

static int lerInteiro(const char *mensagem, int minimo, int maximo) {
    char linha[100];
    char *fim;
    long valor;

    for (;;) {
        printf("%s", mensagem);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\nEntrada encerrada. Finalizando.\n");
            exit(EXIT_SUCCESS);
        }

        errno = 0;
        valor = strtol(linha, &fim, 10);

        while (isspace((unsigned char)*fim)) {
            fim++;
        }

        if (fim == linha || *fim != '\0' || errno == ERANGE ||
            valor < minimo || valor > maximo) {
            printf("Valor invalido. Informe um numero entre %d e %d.\n",
                   minimo, maximo);
            continue;
        }
        return (int)valor;
    }
}

static int lerSimNao(const char *mensagem) {
    char linha[30];
    char resposta;

    for (;;) {
        lerTexto(mensagem, linha, sizeof(linha));
        resposta = (char)toupper((unsigned char)linha[0]);

        if ((resposta == 'S' || resposta == 'N') && linha[1] == '\0') {
            return resposta == 'S';
        }
        printf("Resposta invalida. Digite S para sim ou N para nao.\n");
    }
}

static char lerGenero(void) {
    char linha[30];
    for (;;) {
        lerTexto("Genero (F = feminino, M = masculino, O = outro/nao informar): ",
                 linha, sizeof(linha));
        if (linha[1] == '\0') {
            char g = (char)toupper((unsigned char)linha[0]);
            if (g == 'F' || g == 'M' || g == 'O') {
                return g;
            }
        }
        printf("Opcao invalida. Digite F, M ou O.\n");
    }
}

/* ---------- Filas ---------- */

static void inicializarFila(Fila *fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->quantidade = 0;
}

static int filaVazia(const Fila *fila) {
    return fila->inicio == NULL;
}

static int enfileirar(Fila *fila, Paciente paciente) {
    No *novo = (No *)malloc(sizeof(No));
    if (novo == NULL) {
        return 0;
    }

    novo->paciente = paciente;
    novo->proximo = NULL;

    if (fila->fim == NULL) {
        fila->inicio = novo;
    } else {
        fila->fim->proximo = novo;
    }

    fila->fim = novo;
    fila->quantidade++;
    return 1;
}

static int desenfileirar(Fila *fila, Paciente *paciente) {
    if (filaVazia(fila)) {
        return 0;
    }

    No *removido = fila->inicio;
    *paciente = removido->paciente;
    fila->inicio = removido->proximo;

    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    free(removido);
    fila->quantidade--;
    return 1;
}

static void liberarFila(Fila *fila) {
    Paciente temporario;
    while (desenfileirar(fila, &temporario)) {
        /* libera todos os nos da fila */
    }
}

/* ---------- Regras de negocio ---------- */

static void inicializarSistema(Sistema *sistema) {
    inicializarFila(&sistema->normal);
    inicializarFila(&sistema->preferencial);
    inicializarFila(&sistema->urgente);

    sistema->proximoNormal = 1;
    sistema->proximoPreferencial = 1;
    sistema->proximoUrgente = 1;
    sistema->possuiEmAtendimento = 0;
}

static char classificarPaciente(const Paciente *p) {
    if (p->idade > 80) {
        return 'U';
    }

    if (p->gestante || p->lactante || p->necessidadeEspecial || p->obeso) {
        return 'P';
    }

    if ((p->genero == 'F' && p->idade >= 60) ||
        (p->genero == 'M' && p->idade >= 65)) {
        return 'P';
    }

    return 'N';
}

static int gerarSenha(Sistema *sistema, Paciente *p) {
    Fila *destino = NULL;

    p->codigo = classificarPaciente(p);

    if (p->codigo == 'U') {
        p->numero = sistema->proximoUrgente++;
        destino = &sistema->urgente;
    } else if (p->codigo == 'P') {
        p->numero = sistema->proximoPreferencial++;
        destino = &sistema->preferencial;
    } else {
        p->numero = sistema->proximoNormal++;
        destino = &sistema->normal;
    }

    if (!enfileirar(destino, *p)) {
        /* Reverte o contador caso nao seja possivel alocar memoria. */
        if (p->codigo == 'U') sistema->proximoUrgente--;
        else if (p->codigo == 'P') sistema->proximoPreferencial--;
        else sistema->proximoNormal--;
        return 0;
    }
    return 1;
}

static const char *nomeCategoria(char codigo) {
    switch (codigo) {
        case 'U': return "Urgente";
        case 'P': return "Preferencial";
        default: return "Normal";
    }
}

static void imprimirSenha(const Paciente *p) {
    printf("%c%03d", p->codigo, p->numero);
}

static int obterProximo(Sistema *sistema, Paciente *p) {
    if (!filaVazia(&sistema->urgente)) {
        *p = sistema->urgente.inicio->paciente;
        return 1;
    }
    if (!filaVazia(&sistema->preferencial)) {
        *p = sistema->preferencial.inicio->paciente;
        return 1;
    }
    if (!filaVazia(&sistema->normal)) {
        *p = sistema->normal.inicio->paciente;
        return 1;
    }
    return 0;
}

/* ---------- Operacoes do sistema ---------- */

static void cadastrarPaciente(Sistema *sistema) {
    Paciente p = {0};

    printf("\n--- CADASTRO DE PACIENTE ---\n");
    lerTexto("Nome do paciente: ", p.nome, sizeof(p.nome));
    p.genero = lerGenero();
    p.idade = lerInteiro("Idade (0 a 120): ", 0, 120);

    if (p.genero == 'F') {
        p.gestante = lerSimNao("A paciente esta gestante? (S/N): ");
    } else {
        p.gestante = 0;
    }

    p.lactante = lerSimNao("E lactante? (S/N): ");
    p.necessidadeEspecial =
        lerSimNao("Possui necessidade especial? (S/N): ");
    p.obeso = lerSimNao("Possui condicao de obesidade? (S/N): ");

    if (!gerarSenha(sistema, &p)) {
        printf("Erro: nao foi possivel reservar memoria para a senha.\n");
        return;
    }

    printf("\nCadastro realizado com sucesso!\n");
    printf("Paciente: %s\nSenha: ", p.nome);
    imprimirSenha(&p);
    printf("\nCategoria: %s\n", nomeCategoria(p.codigo));
}

static void consultarProximaSenha(Sistema *sistema) {
    Paciente proximo;

    printf("\n--- PROXIMA SENHA ---\n");
    if (!obterProximo(sistema, &proximo)) {
        printf("Nao ha pacientes aguardando.\n");
        return;
    }

    printf("Senha: ");
    imprimirSenha(&proximo);
    printf("\nPaciente: %s\nCategoria: %s\n",
           proximo.nome, nomeCategoria(proximo.codigo));
}

static void chamarProximoPaciente(Sistema *sistema) {
    Paciente chamado;
    int removido = 0;

    printf("\n--- CHAMADA DE PACIENTE ---\n");

    if (sistema->possuiEmAtendimento) {
        printf("Finalize o atendimento atual antes de chamar outra senha.\n");
        printf("Em atendimento: ");
        imprimirSenha(&sistema->emAtendimento);
        printf("\n");
        return;
    }

    if (!filaVazia(&sistema->urgente)) {
        removido = desenfileirar(&sistema->urgente, &chamado);
    } else if (!filaVazia(&sistema->preferencial)) {
        removido = desenfileirar(&sistema->preferencial, &chamado);
    } else if (!filaVazia(&sistema->normal)) {
        removido = desenfileirar(&sistema->normal, &chamado);
    }

    if (!removido) {
        printf("Nao ha pacientes aguardando atendimento.\n");
        return;
    }

    sistema->emAtendimento = chamado;
    sistema->possuiEmAtendimento = 1;

    printf(">>> CHAMANDO SENHA ");
    imprimirSenha(&chamado);
    printf(" <<<\n");
    printf("Paciente: %s\nCategoria: %s\n",
           chamado.nome, nomeCategoria(chamado.codigo));
}

static void finalizarAtendimento(Sistema *sistema) {
    printf("\n--- FINALIZAR ATENDIMENTO ---\n");

    if (!sistema->possuiEmAtendimento) {
        printf("Nao existe atendimento em andamento.\n");
        return;
    }

    printf("Atendimento da senha ");
    imprimirSenha(&sistema->emAtendimento);
    printf(" (%s) finalizado.\n", sistema->emAtendimento.nome);

    sistema->possuiEmAtendimento = 0;
    memset(&sistema->emAtendimento, 0, sizeof(Paciente));
}

static void exibirPainel(Sistema *sistema) {
    int posicao = 1;

    printf("\n========================================\n");
    printf("       PAINEL DE ATENDIMENTO CLINICO\n");
    printf("========================================\n");

    if (sistema->possuiEmAtendimento) {
        printf("EM ATENDIMENTO: ");
        imprimirSenha(&sistema->emAtendimento);
        printf(" - %s (%s)\n", sistema->emAtendimento.nome,
               nomeCategoria(sistema->emAtendimento.codigo));
    } else {
        printf("EM ATENDIMENTO: Nenhuma senha chamada.\n");
    }

    printf("\nQUANTIDADE EM ESPERA\n");
    printf("Urgentes:      %d\n", sistema->urgente.quantidade);
    printf("Preferenciais: %d\n", sistema->preferencial.quantidade);
    printf("Normais:       %d\n", sistema->normal.quantidade);
    printf("Total:         %d\n",
           sistema->urgente.quantidade +
           sistema->preferencial.quantidade +
           sistema->normal.quantidade);

    printf("\nPROXIMAS SENHAS (ordem de prioridade)\n");
    if (filaVazia(&sistema->urgente) &&
        filaVazia(&sistema->preferencial) &&
        filaVazia(&sistema->normal)) {
        printf("Nenhuma senha aguardando.\n");
    } else {
        const Fila *filas[] = {
            &sistema->urgente, &sistema->preferencial, &sistema->normal
        };
        const char *titulos[] = {
            "Urgente", "Preferencial", "Normal"
        };

        for (int f = 0; f < 3; f++) {
            const No *atual = filas[f]->inicio;
            while (atual != NULL) {
                printf("%2d. ", posicao++);
                imprimirSenha(&atual->paciente);
                printf(" - %s (%s)\n", atual->paciente.nome, titulos[f]);
                atual = atual->proximo;
            }
        }
    }

    printf("========================================\n");
}

static void exibirMenu(void) {
    printf("\n========================================\n");
    printf(" SISTEMA DE SENHAS - ATENDIMENTO CLINICO\n");
    printf("========================================\n");
    printf("1 - Cadastrar paciente\n");
    printf("2 - Consultar proxima senha\n");
    printf("3 - Chamar proximo paciente\n");
    printf("4 - Finalizar atendimento\n");
    printf("5 - Exibir painel de chamada\n");
    printf("0 - Sair\n");
    printf("----------------------------------------\n");
}

static void encerrarSistema(Sistema *sistema) {
    liberarFila(&sistema->normal);
    liberarFila(&sistema->preferencial);
    liberarFila(&sistema->urgente);
}

/* ---------- Funcao principal ---------- */

int main(void) {
    Sistema sistema;
    int opcao;

    inicializarSistema(&sistema);

    do {
        exibirMenu();
        opcao = lerInteiro("Escolha uma opcao: ", 0, 5);

        switch (opcao) {
            case 1:
                cadastrarPaciente(&sistema);
                break;
            case 2:
                consultarProximaSenha(&sistema);
                break;
            case 3:
                chamarProximoPaciente(&sistema);
                break;
            case 4:
                finalizarAtendimento(&sistema);
                break;
            case 5:
                exibirPainel(&sistema);
                break;
            case 0:
                encerrarSistema(&sistema);
                printf("Sistema encerrado. Ate logo!\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }
    } while (opcao != 0);

    return 0;
}
