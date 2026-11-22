
# Jogo do Galo em C

Jogo do Galo desenvolvido em linguagem C, com interface
colorida no terminal e adversário controlado pelo computador.

## Funcionalidades

- Campeonato à melhor de 1, 3 ou 5.
- Alternância do jogador que começa cada partida.
- Jogadas através do teclado numérico (1-9).
- Tabuleiro colorido com destaque da linha vencedora.
- Adversário com estratégia de ataque e defesa.
- Deteção de vitórias e empates.
- Marcador do campeonato.
- Possibilidade de iniciar um novo campeonato.


## Controlos

Utiliza o teclado numérico para escolher a casa:

| 7 | 8 | 9 |
|:---:|:---:|:---:|
| 4 | 5 | 6 |
| 1 | 2 | 3 |

Não é necessário pressionar Enter depois de escolher uma casa.

Pressiona ESC para cancelar o campeonato.


## Como executar

O Jogo do Galo pode ser executado de duas formas: através do executável disponibilizado no GitHub ou compilando o código-fonte no Visual Studio.

### Opção 1 — Descarregar e jogar (recomendado)

Esta é a forma mais simples de jogar, não sendo necessário instalar o Visual Studio.

1. Acede à página de [Releases do projeto](https://github.com/EnfRuiSousa/TIC-TAC-TOE_C-Language/releases).
2. Descarrega o ficheiro `JogoDoGalo-Windows-x64.zip` da versão mais recente.
3. Extrai o conteúdo do ficheiro ZIP para uma pasta à tua escolha.
4. Executa o ficheiro `JogoDoGalo_C.exe`.
5. Escolhe o formato do campeonato e começa a jogar!

**Requisitos:** Windows de 64 bits.

### Opção 2 — Compilar e executar através do Visual Studio

Esta opção destina-se a quem pretende explorar o código-fonte, modificar o jogo ou compilá-lo.

1. Descarrega o código-fonte através de **Code → Download ZIP** ou clona o repositório com Git.
2. Extrai os ficheiros, caso tenhas descarregado o ZIP.
3. Abre o ficheiro `JogoDoGalo_C.slnx` no Visual Studio.
4. Seleciona a configuração `Release` ou `Debug` e a plataforma `x64`.
5. Compila o projeto através de **Build → Build Solution** (`Ctrl + Shift + B`).
6. Executa o jogo pressionando `Ctrl + F5`.

**Requisitos:** Visual Studio com os componentes de desenvolvimento para C/C++ instalados.

---

## Controlos

O jogo é controlado através do teclado numérico:

| 7 | 8 | 9 |
|:---:|:---:|:---:|
| 4 | 5 | 6 |
| 1 | 2 | 3 |

Não é necessário pressionar Enter depois de escolher uma casa.

Pressiona **ESC** para cancelar o campeonato e regressar ao menu.
## Tecnologias

- Linguagem C
- Biblioteca padrão de C
- API da consola do Windows
- Cores ANSI, quando suportadas

## Regras

O jogador utiliza X e o computador utiliza O.

O primeiro jogo começa com o computador.
O jogador que começa alterna a cada partida.

Os empates não contam para o número de vitórias
necessárias no campeonato.
