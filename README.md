# Sistema de Gerenciamento de Senhas - Clínica

**Aluno:** Erlan Luiz  
**Linguagem:** C (padrão C99)  
**Proposta:** Questão 2 — Gerenciamento de fila de atendimento clínico

## Descrição

Aplicação em terminal para cadastrar pacientes, classificá-los por prioridade e organizar a ordem de atendimento em três filas independentes.

## Funcionalidades

- Cadastro de nome, idade, sexo e condições de prioridade.
- Classificação automática em urgente, preferencial ou normal.
- Geração de senhas independentes: U001, P001 e N001.
- Atendimento por prioridade: urgente, preferencial e normal.
- Ordem FIFO (primeiro a entrar, primeiro a sair) dentro de cada categoria.
- Consulta do próximo paciente.
- Chamada e finalização de atendimento.
- Painel com paciente em atendimento e filas aguardando.
- Validação de entradas e limite de 100 pacientes por fila.

## Regras de classificação implementadas

- **Urgente:** paciente com mais de 80 anos.
- **Preferencial:** gestante, lactante, pessoa com necessidade especial ou obesidade; mulher com 60 anos ou mais; homem com 65 anos ou mais.
- **Normal:** paciente que não se enquadra nas condições anteriores.

A categoria urgente tem precedência sobre a preferencial.

## Organização e modularização

O programa foi dividido em funções com responsabilidades específicas, por exemplo:

- `inicializar_fila`, `inserir_fila`, `retirar_fila`: operações da fila circular.
- `ler_texto`, `ler_inteiro`, `ler_sim_nao`, `ler_sexo`: leitura e validação.
- `eh_urgente`, `eh_preferencial`, `selecionar_fila`: regras de classificação.
- `cadastrar_paciente`, `consultar_proximo`, `chamar_paciente`, `finalizar_atendimento`: operações do sistema.
- `exibir_painel`, `exibir_menu`: apresentação ao usuário.

A função `main()` controla o menu e encaminha cada opção para a função correspondente.

## Como compilar

Com GCC instalado:

```bash
gcc -std=c99 -Wall -Wextra -pedantic clinica.c -o clinica
```

No Windows:

```bash
clinica.exe
```

No Linux/macOS:

```bash
./clinica
```

## Observação

Os dados ficam armazenados em memória durante a execução. Ao encerrar o programa, as filas são reiniciadas.
