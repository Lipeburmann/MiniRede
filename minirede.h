#ifndef MINI_REDE_H
#define MINI_REDE_H

#include <iostream>
#include <cstring>

const int TAM_USERNAME = 50;
const int TAM_NOME = 100;
const int TAM_TEXTO = 280;
const int TAM_COMANDO = 30;
const int TAM_HASH = 167;
//////////////////////////////////////////////
//geral
struct IntNode
{
    int id;
    IntNode *prox;
};

//Estrutura para as Notificações
enum TipoNotificacao
{
    NOTIF_FOLLOW,
    NOTIF_LIKE,
    NOTIF_COMMENT
};

struct Notificacao
{
    TipoNotificacao tipo; //follow,like,comment
    int idOrigem;
    int idPost; 
    Notificacao *prox;
};

struct FilaNotificacoes
{
    Notificacao *inicio;
    Notificacao *fim;
};

/////////////////////////////////////////////////////

struct Publicacao; // Forward declaration

struct Usuario
{
    int id;
    char username[TAM_USERNAME];
    char nomeCompleto[TAM_NOME];

    IntNode *seguidos;          // lista de ids de usuários que ele segue
    Publicacao *postsCriados;   // lista de posts que ele criou
    FilaNotificacoes filaNotif; // fila de notificações
};

struct NoPublicacao
{ // estrutura para lista encadeada temporária de publicações 
    Publicacao *post;
    NoPublicacao *prox;
};

enum TipoBuscaPublicacao
{
    BUSCA_GLOBAL,
    BUSCA_USUARIO
};

struct Comentario
{
    int id;
    int idAutor;
    char texto[TAM_TEXTO];
    Comentario *prox; // para criar uma lista de comentários em cada post
};

struct Publicacao
{
    int id;
    int idAutor;
    int timestamp;
    char texto[TAM_TEXTO];
    int curtidas;
    Comentario *listaComentarios; // lista de comentários sobre a publicação
    IntNode *listaCurtidas;       // lista de ids de quem curtiu 
    Publicacao *prox_global;      //para lista global
    Publicacao *prox_autor;       //para lista do autor
};


struct NoUsuarioBST
{
    Usuario *user;
    NoUsuarioBST *esq;
    NoUsuarioBST *dir;
};

struct NoUsuarioHash
{
    Usuario *user;
    NoUsuarioHash *prox;
};


struct MiniRede
{
    NoUsuarioBST *raizUsuarios;
    NoUsuarioHash *hashUsernames[TAM_HASH]; 
    Publicacao *listaPublicacoes;           
};

void inicializarMiniRede(MiniRede &rede);
void liberarMiniRede(MiniRede &rede);
void processarComandos(MiniRede &rede, std::istream &entrada, std::ostream &saida);

void cadastrarUsuario(MiniRede &rede, int id, const char username[], const char nomeCompleto[], std::ostream &saida);
void buscarUsuarioPorId(MiniRede &rede, int id, std::ostream &saida);
void buscarUsuarioPorUsername(MiniRede &rede, const char username[], std::ostream &saida);
void listarUsuarios(MiniRede &rede, std::ostream &saida);

void seguirUsuario(MiniRede &rede, int idSeguidor, int idSeguido, std::ostream &saida);
void listarSeguindo(MiniRede &rede, int idUsuario, std::ostream &saida);

void cadastrarPublicacao(MiniRede &rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream &saida);
void curtirPublicacao(MiniRede &rede, int idUsuario, int idPost, std::ostream &saida);
void removerPostMaisRecenteGlobal(MiniRede &rede, std::ostream &saida);

void consultarNotificacoes(MiniRede &rede, int idUsuario, int k, std::ostream &saida);
void gerarFeed(MiniRede &rede, int idUsuario, int k, std::ostream &saida);
void listarTopPosts(MiniRede &rede, int k, std::ostream &saida);


//responsabilidades auxiliares:
// - buscar usuario por id
Usuario *UsuarioPorId(MiniRede &rede, int id);
// - buscar usuario por username
Usuario *UsuarioPorUsername(MiniRede &rede, const char username[]);
// - buscar publicacao por id
Publicacao *PublicacaoPorId(MiniRede &rede, int idPost);
NoPublicacao *AcharPublicacaoEanteriorPorId(MiniRede &rede, int idPost); // usada para remover post
// - inserir/listar/liberar arvore
void liberarArvore(NoUsuarioBST *raiz);
// - inserir/buscar/liberar tabela hash
void liberarTabelaHash(NoUsuarioHash *hashUsernames[]);
// - enfileirar/desenfileirar notificacoes
void liberarFilaNotificacoes(Notificacao *inicio);
// - manipular listas encadeadas
void liberarListaIntNode(IntNode *inicio);
// - imprimir saida formatada de usuários, posts, feeds, etc
void imprimirArvoreInOrdem(NoUsuarioBST *raiz, std::ostream &saida);
void imprimirXPosts(NoPublicacao *lista, int k, std::ostream &saida);
void liberarListaPublicacao(NoPublicacao *inicio);
void liberarPublicacoes(Publicacao *inicio);

NoPublicacao *SelecionarKPosts(Publicacao *posts, int k, TipoBuscaPublicacao tipo); // cria a lista auxiliar de posts para feed ou ranking

// auxiliares para gerar feed
NoPublicacao *SelecionarKPostsMaisRecentes(NoPublicacao *posts, int k);
NoPublicacao *OrdenarPostsPorTimestamp(NoPublicacao *posts);         // ordena
NoPublicacao *ListarPostsSeguindo(MiniRede &rede, Usuario *usuario); // acessa os seguidos do usuário e seleciona os posts para o feed

// auxiliares para listar top posts
NoPublicacao *SelecionarKPostsMaisCurtidos(Publicacao *posts, int k);
NoPublicacao *OrdenarPostsPorCurtidas(NoPublicacao *posts); // ordena

void notificarUsuario(MiniRede &rede, int idUsuario, int idOrigem, int idPost, TipoNotificacao tipo);

// extras

void unfollowUsuario(MiniRede &rede, int idSeguidor, int idSeguido, std::ostream &saida);
void removerPost(MiniRede &rede, int idPost, std::ostream &saida);

void comentar(MiniRede &rede, int idUsuario, int idPost, int idComentario, const char texto[], std::ostream &saida);
void listarComentarios(MiniRede &rede, int idPost, std::ostream &saida);

void imprimirXPostsGlobal(MiniRede &rede, int k, std::ostream &saida);
void buscarPublicacoesPorPalavra(MiniRede &rede, const char palavra[], std::ostream &saida);
#endif
