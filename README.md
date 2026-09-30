# MiniRede

Uma rede social simplificada com motor escrito em C++ e uma interface web moderna. O projeto demonstra, na prática, árvore binária de busca, tabela hash, filas e listas encadeadas — implementadas manualmente, sem usar a STL nas estruturas do domínio.

## Interface gráfica

Requisitos: `g++`, `make`, Python 3 e um navegador moderno.

```bash
make run
```

A aplicação será aberta em `http://127.0.0.1:8080`. Ela já inicia com dados de demonstração e permite:

- criar e alternar perfis;
- publicar, remover, curtir e comentar;
- seguir e deixar de seguir pessoas;
- consultar notificações;
- buscar publicações e visualizar o ranking;
- restaurar a demonstração a qualquer momento.

O servidor local usa somente a biblioteca padrão do Python. Cada ação da interface é traduzida para os comandos originais e processada pelo executável C++, preservando o motor e suas regras.

## Interface por terminal

```bash
make
./minirede < entrada_1.txt
```

Desenvolvedores: Felipe Burmann e Francisco Braga.
