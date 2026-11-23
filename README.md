# Sistema de Gerenciamento de Senhas de Atendimento Clínico

Projeto acadêmico desenvolvido em linguagem C para simular o cadastro, a classificação e o gerenciamento de filas de atendimento de uma clínica.

## Objetivo

Organizar a fila de atendimento por três categorias, respeitando a prioridade definida no enunciado e a ordem de chegada dentro de cada categoria (FIFO).

> **Aviso:** as regras de prioridade deste programa são exclusivamente didáticas e foram implementadas conforme a proposta da atividade. Elas não substituem avaliação clínica, triagem médica ou a legislação aplicável.

## Funcionalidades

- Cadastro de pacientes com nome, gênero, idade e condições informadas.
- Classificação automática em:
  - **U — Urgente:** idade superior a 80 anos.
  - **P — Preferencial:** gestante, lactante, pessoa com necessidade especial, pessoa com obesidade, mulher com 60 anos ou mais ou homem com 65 anos ou mais.
  - **N — Normal:** paciente que não se enquadra nas condições anteriores.
- Geração de senhas sequenciais e independentes: `N001`, `P001`, `U001` etc.
- Consulta da próxima senha.
- Chamada do próximo paciente seguindo a prioridade **Urgente > Preferencial > Normal**.
- Finalização do atendimento atual.
- Painel com senha em atendimento, próximas senhas e quantidade de pessoas aguardando por categoria.
- Validação de entradas e liberação da memória alocada para as filas.

## Estruturas e conceitos de C utilizados

- `struct` para representar pacientes, nós e filas.
- Listas encadeadas para armazenar as filas dinamicamente.
- Ponteiros e alocação dinâmica com `malloc()` e `free()`.
- Funções com responsabilidades específicas.
- Estruturas condicionais (`if`, `else`, `switch`).
- Estrutura de repetição (`do...while`, `while`, `for`).
- Validação de dados com `fgets()`, `strtol()` e verificações de faixa.

## Funções implementadas

| Função | Responsabilidade |
|---|---|
| `lerTexto()` | Lê textos obrigatórios. |
| `lerInteiro()` | Lê e valida números inteiros dentro de uma faixa. |
| `lerSimNao()` | Valida respostas Sim/Não. |
| `lerGenero()` | Valida o gênero informado. |
| `inicializarFila()` | Inicializa uma fila vazia. |
| `enfileirar()` | Insere paciente no final da fila. |
| `desenfileirar()` | Remove o primeiro paciente da fila. |
| `liberarFila()` | Libera os nós restantes da fila. |
| `inicializarSistema()` | Inicializa filas e contadores. |
| `classificarPaciente()` | Define a categoria do paciente. |
| `gerarSenha()` | Gera senha e encaminha para a fila correspondente. |
| `obterProximo()` | Consulta a próxima senha respeitando a prioridade. |
| `cadastrarPaciente()` | Coleta os dados e registra o paciente. |
| `consultarProximaSenha()` | Exibe a próxima senha a ser chamada. |
| `chamarProximoPaciente()` | Retira a próxima senha da fila e inicia o atendimento. |
| `finalizarAtendimento()` | Encerra o atendimento atual. |
| `exibirPainel()` | Apresenta o painel geral de chamadas. |
| `exibirMenu()` | Mostra as opções do sistema. |
| `encerrarSistema()` | Libera a memória das filas ao sair. |

## Como compilar e executar

### Linux

É necessário ter o GCC instalado.

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o sistema_clinica
./sistema_clinica
```

### Windows (MinGW/GCC)

No terminal, dentro da pasta do projeto:

```bash
gcc -std=c99 -Wall -Wextra -pedantic main.c -o sistema_clinica.exe
sistema_clinica.exe
```

Também é possível abrir a pasta no Visual Studio Code e executar os comandos pelo terminal integrado, desde que o compilador GCC esteja instalado e configurado no PATH.

## Exemplo de utilização

Exemplo ilustrativo de classificação:

| Dados do paciente | Categoria | Senha ilustrativa |
|---|---|---|
| 35 anos, sem condições de prioridade | Normal | N001 |
| Mulher de 62 anos | Preferencial | P001 |
| Pessoa de 84 anos | Urgente | U001 |

Se houver uma senha normal aguardando e uma urgente for cadastrada depois, a urgente será chamada primeiro. Dentro de cada categoria, a ordem de chegada será mantida.

## Regras importantes

1. A categoria urgente tem prioridade sobre a preferencial e a normal.
2. A categoria preferencial tem prioridade sobre a normal.
3. A ordem FIFO é respeitada dentro de cada categoria.
4. Não é permitido chamar outra senha enquanto houver um atendimento em andamento.
5. Ao chamar um paciente, sua senha sai da fila de espera e passa para o estado de atendimento.
6. Ao finalizar, o paciente não retorna à fila.
7. As senhas já emitidas não são reutilizadas durante a execução.

## Limitações

Os dados ficam em memória e são perdidos quando o programa é encerrado. O enunciado não exige banco de dados nem persistência em arquivo.

## Autor

**Felipe Marques Vieira**

Projeto acadêmico — Linguagem C.
