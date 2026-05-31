// --- ESTRUTURAS AUXILIARES ---

// Nó genérico de lista encadeada para guardar IDs (útil para Seguidores, Curtidas, etc)
struct IntNode {
    int id;
    IntNode* prox;
};

// Estrutura para as Notificações
struct Notificacao {
    // Pode ser "FOLLOW" ou "LIKE"
    char tipo[TAM_COMANDO]; 
    int idOrigem;
    int idPost; // Só usado se for curtida
    Notificacao* prox;
};

// Fila baseada em lista encadeada para cada usuário
struct FilaNotificacoes {
    Notificacao* inicio;
    Notificacao* fim;
};

// --- ENTIDADES PRINCIPAIS ---

struct Publicacao {
    int id;
    int idAutor;
    int timestamp;
    char texto[TAM_TEXTO];
    int curtidas;
    IntNode* listaCurtidas; // Lista de IDs de quem curtiu (para evitar curtida dupla)
    Publicacao* prox;       // Caso decida guardar as publicações em uma lista global
};

struct Usuario {
    int id;
    char username[TAM_USERNAME];
    char nomeCompleto[TAM_NOME];
    
    IntNode* seguindo;        // Lista de IDs de usuários que ele segue
    IntNode* postsCriados;    // Lista de IDs dos posts que ele criou
    FilaNotificacoes fila;    // Fila de notificações
};

// Nó para a Árvore Binária de Pesquisa (BST)
struct BSTNode {
    Usuario user;
    BSTNode* esq;
    BSTNode* dir;
};

// Nó para a Tabela Hash (Tratamento de colisão por encadeamento)
struct HashNode {
    char username[TAM_USERNAME];
    int idUsuario; // Apenas guarda o ID para depois buscar na BST
    HashNode* prox;
};

// --- ESTRUTURA GLOBAL DA REDE ---

struct MiniRede {
    BSTNode* raizUsuarios;        // Árvore binária ordenada por ID
    HashNode** tabelaHashUsers;   // Array de ponteiros para a tabela Hash
    Publicacao* listaPublicacoes; // Lista encadeada simples com todos os posts globais
    int tamanhoHash;              // Ex: 1009 (um número primo)
};