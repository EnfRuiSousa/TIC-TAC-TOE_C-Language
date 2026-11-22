
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

### Windows — Visual Studio

1. Abrir a solução no Visual Studio.
2. Compilar o projeto.
3. Executar com Ctrl + F5.

### Compilação com GCC

```bash
gcc -std=c11 main.c -o jogo
```

No Windows:
`jogo.exe`

No Linux/macOS:
`./jogo`

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
