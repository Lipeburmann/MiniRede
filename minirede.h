#ifndef MINI_REDE_H
#define MINI_REDE_H

#include <iostream>

const int TAM_USERNAME = 50;
const int TAM_NOME = 100;
const int TAM_TEXTO = 280;
const int TAM_COMANDO = 30;
const int TAM_HASH = 167;
// TODO: definir as structs principais do trabalho.
//
// Sugestao de structs que provavelmente serao necessarias:

// Nó genérico de lista encadeada para guardar IDs (útil para Seguidores, Curtidas, etc)
struct IntNode {
    int id;
    IntNode* prox;
};

struct Usuario {
    int id;
    char username[TAM_USERNAME];
    char nomeCompleto[TAM_NOME];
    
    IntNode* seguindo;        // Lista de IDs de usuários que ele segue
    IntNode* postsCriados;    // Lista de IDs dos posts que ele criou
    FilaNotificacoes fila;    // Fila de notificações
};

struct Publicacao {
    int id;
    int idAutor;
    int timestamp;
    char texto[TAM_TEXTO];
    int curtidas;
    IntNode* listaCurtidas; // Lista de IDs de quem curtiu (para evitar curtida dupla)
    Publicacao* prox;       // Caso decida guardar as publicações em uma lista global
};

// - nos para arvore binaria de usuarios por id
struct NoUsuarioBST {
    Usuario* user;
    NoUsuarioBST* esq;
    NoUsuarioBST* dir;
};
// - nos para tabela hash de usernames
struct NoUsuarioHash {
    Usuario* user;
    NoUsuarioHash* prox;
};
// Estrutura para as Notificações
enum TipoNotificacao {
    NOTIF_FOLLOW,
    NOTIF_LIKE
};

struct Notificacao {
    // Pode ser "FOLLOW" ou "LIKE"
    TipoNotificacao tipo;
    int idOrigem;
    int idPost; // Só usado se for curtida
    Notificacao* prox;
};

// Fila baseada em lista encadeada para cada usuário
struct FilaNotificacoes {
    Notificacao* inicio;
    Notificacao* fim;
};
// Os campos de cada struct fazem parte do projeto dos alunos.

struct MiniRede {
    NoUsuarioBST* raizUsuarios; 
    NoUsuarioHash* hashUsernames[TAM_HASH]; // O tamanho da hash pode ser um número primo
    Publicacao* listaPublicacoes; // Lista encadeada simples com todos os posts globais             
};

void inicializarMiniRede(MiniRede& rede);
void liberarMiniRede(MiniRede& rede);
void processarComandos(MiniRede& rede, std::istream& entrada, std::ostream& saida);

void cadastrarUsuario(MiniRede& rede, int id, const char username[], const char nomeCompleto[], std::ostream& saida);
void buscarUsuarioPorId(MiniRede& rede, int id, std::ostream& saida);
void buscarUsuarioPorUsername(MiniRede& rede, const char username[], std::ostream& saida);
void listarUsuarios(MiniRede& rede, std::ostream& saida);

void seguirUsuario(MiniRede& rede, int idSeguidor, int idSeguido, std::ostream& saida);
void listarSeguindo(MiniRede& rede, int idUsuario, std::ostream& saida);

void cadastrarPublicacao(MiniRede& rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream& saida);
void curtirPublicacao(MiniRede& rede, int idUsuario, int idPost, std::ostream& saida);

void consultarNotificacoes(MiniRede& rede, int idUsuario, int k, std::ostream& saida);
void gerarFeed(MiniRede& rede, int idUsuario, int k, std::ostream& saida);
void listarTopPosts(MiniRede& rede, int k, std::ostream& saida);

// TODO: declarar aqui as funcoes auxiliares escolhidas pelo grupo.
//
// Exemplos de responsabilidades auxiliares:
// - buscar usuario por id
// - buscar usuario por username
// - buscar publicacao por id
// - inserir/listar/liberar arvore
// - inserir/buscar/liberar tabela hash
// - enfileirar/desenfileirar notificacoes
// - manipular listas encadeadas
// - ordenar vetores auxiliares para feed e ranking

#endif
