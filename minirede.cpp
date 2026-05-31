#include "minirede.h"
#include <cstring> // Necessário para usar strcpy e manipular char arrays

// --- FUNÇÕES AUXILIARES ---

BSTNode* criarNoUsuario(int id, const char username[], const char nomeCompleto[]) {
    BSTNode* novoNo = new BSTNode;
    
    // Preenchendo os dados
    novoNo->user.id = id;
    std::strcpy(novoNo->user.username, username);
    std::strcpy(novoNo->user.nomeCompleto, nomeCompleto);
    
    // Inicializando ponteiros internos como nulos (vazios)
    novoNo->user.seguindo = nullptr;
    novoNo->user.postsCriados = nullptr;
    novoNo->user.fila.inicio = nullptr;
    novoNo->user.fila.fim = nullptr;
    
    // Inicializando filhos da árvore
    novoNo->esq = nullptr;
    novoNo->dir = nullptr;
    
    return novoNo;
}

bool inserirNaBST(BSTNode*& raiz, BSTNode* novoNo) {
    if (raiz == nullptr) {
        raiz = novoNo;
        return true; 
    }
    
    if (novoNo->user.id == raiz->user.id) {
        return false; 
    } 
    else if (novoNo->user.id < raiz->user.id) {
        return inserirNaBST(raiz->esq, novoNo);
    } 
    else {
        return inserirNaBST(raiz->dir, novoNo);
    }
}

// --- FUNÇÕES PRINCIPAIS ---

void inicializarMiniRede(MiniRede& rede) {
    // Prepara a rede para o primeiro uso garantindo que está limpa
    rede.raizUsuarios = nullptr;
    rede.tabelaHashUsers = nullptr; // Será inicializada no futuro
    rede.listaPublicacoes = nullptr;
    rede.tamanhoHash = 1009; // Um número primo ajuda na dispersão da hash
}

void liberarMiniRede(MiniRede& rede) {
    // TODO: Percorrer e dar 'delete' em todos os nós da árvore, listas e hash 
    // para evitar memory leaks antes de fechar o programa.
}

void processarComandos(MiniRede& rede, std::istream& entrada, std::ostream& saida) {
    // TODO: ler comandos da entrada padrao ate END.
}

void cadastrarUsuario(MiniRede& rede, int id, const char username[], const char nomeCompleto[], std::ostream& saida) {
    BSTNode* novoNo = criarNoUsuario(id, username, nomeCompleto);
    
    if (inserirNaBST(rede.raizUsuarios, novoNo)) {
        // TODO Futuro: Lógica para inserir também na Hash (verificar duplicidade de username antes)
        saida << "USER_ADDED\n";
    } else {
        delete novoNo; // Evita vazamento de memória se o nó foi recusado
        saida << "ERROR USER_EXISTS\n";
    }
}

void buscarUsuarioPorId(MiniRede& rede, int id, std::ostream& saida) {
    // TODO
}

void buscarUsuarioPorUsername(MiniRede& rede, const char username[], std::ostream& saida) {
    // TODO
}

void listarUsuarios(MiniRede& rede, std::ostream& saida) {
    // TODO
}

void seguirUsuario(MiniRede& rede, int idSeguidor, int idSeguido, std::ostream& saida) {
    // TODO
}

void listarSeguindo(MiniRede& rede, int idUsuario, std::ostream& saida) {
    // TODO
}

void cadastrarPublicacao(MiniRede& rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream& saida) {
    // TODO
}

void curtirPublicacao(MiniRede& rede, int idUsuario, int idPost, std::ostream& saida) {
    // TODO
}

void consultarNotificacoes(MiniRede& rede, int idUsuario, int k, std::ostream& saida) {
    // TODO
}

void gerarFeed(MiniRede& rede, int idUsuario, int k, std::ostream& saida) {
    // TODO
}

void listarTopPosts(MiniRede& rede, int k, std::ostream& saida) {
    // TODO
}

int main() {
    MiniRede rede;

    inicializarMiniRede(rede);
    processarComandos(rede, std::cin, std::cout);
    liberarMiniRede(rede);

    return 0;
}