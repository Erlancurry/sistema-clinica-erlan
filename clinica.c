/*
 * Sistema de Gerenciamento de Senhas - Clinica
 * Autor: Erlan Luiz
 * Linguagem: C (C99)
 *
 * Implementacao com filas circulares independentes:
 * urgente > preferencial > normal.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define CAPACIDADE 100
#define TAM_NOME 80

typedef struct {
    char nome[TAM_NOME];
    int idade;
    char sexo;
    int gestante;
    int lactante;
    int necessidade_especial;
    int obesidade;
    char senha[12];
} Paciente;

typedef struct {
    Paciente itens[CAPACIDADE];
    int inicio;
    int fim;
    int quantidade;
    char codigo;
    unsigned int proxima_senha;
} Fila;

typedef struct {
    Fila normal;
    Fila preferencial;
    Fila urgente;
    Paciente em_atendimento;
    int existe_atendimento;
} Clinica;

/* ---------- Entrada e validacao ---------- */

static void limpar_resto_linha(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void ler_texto(const char *mensagem, char *destino, size_t tamanho) {
    size_t n;
    for (;;) {
        printf("%s", mensagem);
        if (fgets(destino, (int)tamanho, stdin) == NULL) {
            destino[0] = '\0';
            return;
        }
        n = strlen(destino);
        if (n > 0 && destino[n - 1] == '\n') {
            destino[n - 1] = '\0';
        } else {
            limpar_resto_linha();
        }
        if (destino[0] != '\0') return;
        puts("O campo nao pode ficar vazio.");
    }
}

static int ler_inteiro(const char *mensagem, int minimo, int maximo) {
    char linha[64], *fim;
    long valor;

    for (;;) {
        printf("%s", mensagem);
        if (!fgets(linha, sizeof(linha), stdin)) return minimo;
        valor = strtol(linha, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (fim != linha && *fim == '\0' && valor >= minimo && valor <= maximo)
            return (int)valor;
        printf("Digite um numero entre %d e %d.\n", minimo, maximo);
    }
}

static int ler_sim_nao(const char *mensagem) {
    char resposta[16];
    for (;;) {
        ler_texto(mensagem, resposta, sizeof(resposta));
        resposta[0] = (char)toupper((unsigned char)resposta[0]);
        if (resposta[0] == 'S' && resposta[1] == '\0') return 1;
        if (resposta[0] == 'N' && resposta[1] == '\0') return 0;
        puts("Responda somente S ou N.");
    }
}

static char ler_sexo(void) {
    char resposta[16];
    for (;;) {
        ler_texto("Sexo (F/M): ", resposta, sizeof(resposta));
        resposta[0] = (char)toupper((unsigned char)resposta[0]);
        if ((resposta[0] == 'F' || resposta[0] == 'M') && resposta[1] == '\0')
            return resposta[0];
        puts("Informe F ou M.");
    }
}

/* ---------- Operacoes das filas circulares ---------- */

static void iniciar_fila(Fila *fila, char codigo) {
    fila->inicio = 0;
    fila->fim = 0;
    fila->quantidade = 0;
    fila->codigo = codigo;
    fila->proxima_senha = 1;
}

static int fila_vazia(const Fila *fila) {
    return fila->quantidade == 0;
}

static int fila_cheia(const Fila *fila) {
    return fila->quantidade == CAPACIDADE;
}

static int inserir_fila(Fila *fila, Paciente paciente) {
    if (fila_cheia(fila)) return 0;

    snprintf(paciente.senha, sizeof(paciente.senha), "%c%03u",
             fila->codigo, fila->proxima_senha++);
    fila->itens[fila->fim] = paciente;
    fila->fim = (fila->fim + 1) % CAPACIDADE;
    fila->quantidade++;
    return 1;
}

static int retirar_fila(Fila *fila, Paciente *paciente) {
    if (fila_vazia(fila)) return 0;

    *paciente = fila->itens[fila->inicio];
    fila->inicio = (fila->inicio + 1) % CAPACIDADE;
    fila->quantidade--;
    return 1;
}

static const Paciente *consultar_inicio(const Fila *fila) {
    if (fila_vazia(fila)) return NULL;
    return &fila->itens[fila->inicio];
}

static void mostrar_fila(const Fila *fila, const char *titulo) {
    int i, pos;
    printf("\n%s (%d paciente(s))\n", titulo, fila->quantidade);
    if (fila_vazia(fila)) {
        puts("  Fila vazia.");
        return;
    }

    for (i = 0; i < fila->quantidade; i++) {
        pos = (fila->inicio + i) % CAPACIDADE;
        printf("  %s - %s, %d anos\n",
               fila->itens[pos].senha,
               fila->itens[pos].nome,
               fila->itens[pos].idade);
    }
}

/* ---------- Regras de negocio ---------- */

static void inicializar_clinica(Clinica *clinica) {
    iniciar_fila(&clinica->normal, 'N');
    iniciar_fila(&clinica->preferencial, 'P');
    iniciar_fila(&clinica->urgente, 'U');
    clinica->existe_atendimento = 0;
}

static int eh_urgente(const Paciente *p) {
    return p->idade > 80;
}

static int eh_preferencial(const Paciente *p) {
    if (p->gestante || p->lactante || p->necessidade_especial || p->obesidade)
        return 1;
    if (p->sexo == 'F' && p->idade >= 60) return 1;
    if (p->sexo == 'M' && p->idade >= 65) return 1;
    return 0;
}

static Fila *selecionar_fila(Clinica *clinica, const Paciente *p,
                             const char **descricao) {
    if (eh_urgente(p)) {
        *descricao = "URGENTE";
        return &clinica->urgente;
    }
    if (eh_preferencial(p)) {
        *descricao = "PREFERENCIAL";
        return &clinica->preferencial;
    }
    *descricao = "NORMAL";
    return &clinica->normal;
}

static void cadastrar_paciente(Clinica *clinica) {
    Paciente p;
    Fila *destino;
    const char *categoria;

    puts("\n--- CADASTRO DE PACIENTE ---");
    ler_texto("Nome completo: ", p.nome, sizeof(p.nome));
    p.idade = ler_inteiro("Idade (0 a 120): ", 0, 120);
    p.sexo = ler_sexo();

    p.gestante = ler_sim_nao("Gestante? (S/N): ");
    p.lactante = ler_sim_nao("Lactante? (S/N): ");
    p.necessidade_especial = ler_sim_nao("Possui necessidade especial? (S/N): ");
    p.obesidade = ler_sim_nao("Possui obesidade? (S/N): ");

    destino = selecionar_fila(clinica, &p, &categoria);
    if (!inserir_fila(destino, p)) {
        puts("Fila cheia. Nao foi possivel cadastrar o paciente.");
        return;
    }

    printf("\nCadastro realizado!\nSenha: %s\nCategoria: %s\n",
           destino->itens[(destino->fim + CAPACIDADE - 1) % CAPACIDADE].senha,
           categoria);
}

static const Paciente *proximo_paciente(const Clinica *clinica) {
    const Paciente *p = consultar_inicio(&clinica->urgente);
    if (p != NULL) return p;
    p = consultar_inicio(&clinica->preferencial);
    if (p != NULL) return p;
    return consultar_inicio(&clinica->normal);
}

static Fila *fila_prioritaria(Clinica *clinica) {
    if (!fila_vazia(&clinica->urgente)) return &clinica->urgente;
    if (!fila_vazia(&clinica->preferencial)) return &clinica->preferencial;
    if (!fila_vazia(&clinica->normal)) return &clinica->normal;
    return NULL;
}

static void consultar_proximo(const Clinica *clinica) {
    const Paciente *p = proximo_paciente(clinica);
    puts("\n--- PROXIMO DA FILA ---");
    if (p == NULL) {
        puts("Nao ha pacientes aguardando.");
        return;
    }
    printf("Senha: %s\nNome: %s\nIdade: %d\n",
           p->senha, p->nome, p->idade);
}

static void chamar_paciente(Clinica *clinica) {
    Fila *fila;
    Paciente p;

    if (clinica->existe_atendimento) {
        puts("Finalize o atendimento atual antes de chamar outro paciente.");
        return;
    }

    fila = fila_prioritaria(clinica);
    if (fila == NULL || !retirar_fila(fila, &p)) {
        puts("Nao ha pacientes aguardando.");
        return;
    }

    clinica->em_atendimento = p;
    clinica->existe_atendimento = 1;
    printf("\n>>> CHAMANDO SENHA %s - %s <<<\n", p.senha, p.nome);
}

static void finalizar_atendimento(Clinica *clinica) {
    if (!clinica->existe_atendimento) {
        puts("Nao existe atendimento em andamento.");
        return;
    }
    printf("Atendimento da senha %s (%s) finalizado.\n",
           clinica->em_atendimento.senha,
           clinica->em_atendimento.nome);
    clinica->existe_atendimento = 0;
}

static void exibir_painel(const Clinica *clinica) {
    puts("\n======================================");
    puts("          PAINEL DA CLINICA");
    puts("======================================");

    if (clinica->existe_atendimento) {
        printf("EM ATENDIMENTO: %s - %s\n",
               clinica->em_atendimento.senha,
               clinica->em_atendimento.nome);
    } else {
        puts("EM ATENDIMENTO: nenhum paciente");
    }

    {
        const Paciente *p = proximo_paciente(clinica);
        if (p != NULL)
            printf("PROXIMA SENHA: %s - %s\n", p->senha, p->nome);
        else
            puts("PROXIMA SENHA: nenhuma");
    }

    mostrar_fila(&clinica->urgente, "FILA URGENTE");
    mostrar_fila(&clinica->preferencial, "FILA PREFERENCIAL");
    mostrar_fila(&clinica->normal, "FILA NORMAL");
    puts("======================================");
}

static void exibir_menu(void) {
    puts("\n========== SISTEMA DA CLINICA ==========");
    puts("1 - Cadastrar paciente");
    puts("2 - Consultar proximo paciente");
    puts("3 - Chamar proximo paciente");
    puts("4 - Finalizar atendimento");
    puts("5 - Exibir painel");
    puts("0 - Sair");
    puts("========================================");
}

int main(void) {
    Clinica clinica;
    int opcao;

    inicializar_clinica(&clinica);
    puts("Sistema de Gerenciamento de Senhas - Clinica");

    do {
        exibir_menu();
        opcao = ler_inteiro("Escolha uma opcao: ", 0, 5);

        switch (opcao) {
            case 1: cadastrar_paciente(&clinica); break;
            case 2: consultar_proximo(&clinica); break;
            case 3: chamar_paciente(&clinica); break;
            case 4: finalizar_atendimento(&clinica); break;
            case 5: exibir_painel(&clinica); break;
            case 0: puts("Sistema encerrado."); break;
            default: puts("Opcao invalida."); break;
        }
    } while (opcao != 0);

    return 0;
}
