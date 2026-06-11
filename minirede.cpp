#include "minirede.h"

void inicializarMiniRede(MiniRede& rede) {
    // Garante que a árvore comece vazia
    rede.raizUsuarios = nullptr;
    
    // Garante que todas as gavetas da Tabela Hash comecem vazias
    for (int i = 0; i < 101; i++) {
        rede.hashUsernames[i] = nullptr;
    }
}
//Ela vai pegar o texto do username, somar os valores das letras e dividir pelo tamanho do vetor (101), pegando o resto da divisão
int calcularHash(const char username[]) {
    int hash = 0;
    for (int i = 0; username[i] != '\0'; i++) {
        hash = (hash + username[i]) % TAM_HASH;
    }
    return hash;
}

void liberarMiniRede(MiniRede& rede) {
    // TODO
}

void processarComandos(MiniRede& rede, std::istream& entrada, std::ostream& saida) {
    // TODO: ler comandos da entrada padrao ate END.
    // Para cada comando, chamar a funcao correspondente.
    // Nao imprimir menu, prompt ou texto extra.
}

void cadastrarUsuario(MiniRede& rede, int id, const char username[], const char nomeCompleto[], std::ostream& saida) {
    // ==========================================
    // PASSO A: Verificar se o ID já existe (Busca na Árvore)
    // ==========================================
    NoUsuarioBST* atualBST = rede.raizUsuarios;
    bool idExiste = false;
    while (atualBST != nullptr) {
        if (atualBST->user->id == id) {
            idExiste = true; 
            break;
        }
        if (id < atualBST->user->id) {
            atualBST = atualBST->esq;
        } else {
            atualBST = atualBST->dir;
        }
    }

    // ==========================================
    // PASSO B: Verificar se Username já existe (Busca na Hash)
    // ==========================================
    int indiceHash = calcularHash(username);
    NoUsuarioHash* atualHash = rede.hashUsernames[indiceHash];
    bool userExiste = false;
    while (atualHash != nullptr) {
        if (strcmp(atualHash->user->username, username) == 0) {
            userExiste = true;
            break;
        }
        atualHash = atualHash->prox;
    }

    // Se ID ou Username já estiverem em uso, barra o cadastro
    if (idExiste || userExiste) {
        saida << "ERROR USER_EXISTS\n";
        return; // Sai da função, não cadastra nada
    }

    // ==========================================
    // PASSO C: Criar o novo Usuário na memória
    // ==========================================
    Usuario* novoUser = new Usuario;
    novoUser->id = id;
    strcpy(novoUser->username, username);
    strcpy(novoUser->nomeCompleto, nomeCompleto);
// Inicializando as estruturas internas do usuário como vazias
    novoUser->seguidos = nullptr;
    novoUser->filaNotif.inicio = nullptr;
    novoUser->filaNotif.fim = nullptr;

    // ==========================================
    // PASSO D: Inserir na Árvore Binária (por ID)
    // ==========================================
    NoUsuarioBST* novoNoBST = new NoUsuarioBST{novoUser, nullptr, nullptr};
    
    if (rede.raizUsuarios == nullptr) {
        // A árvore está vazia, ele será a raiz!
        rede.raizUsuarios = novoNoBST;
    } else {
        // Descer na árvore até achar a folha certa para pendurar o novo nó
        NoUsuarioBST* pai = nullptr;
        NoUsuarioBST* temp = rede.raizUsuarios;
        
        while (temp != nullptr) {
            pai = temp;
            if (id < temp->user->id) temp = temp->esq;
            else temp = temp->dir;
        }
        
        if (id < pai->user->id) pai->esq = novoNoBST;
        else pai->dir = novoNoBST;
    }

    // ==========================================
    // PASSO E: Inserir na Tabela Hash (por Username)
    // ==========================================
    // Inserimos sempre no início da lista encadeada daquela gaveta (é mais fácil e rápido)
    NoUsuarioHash* novoNoHash = new NoUsuarioHash{novoUser, rede.hashUsernames[indiceHash]};
    rede.hashUsernames[indiceHash] = novoNoHash;

    // ==========================================
    // PASSO F: Sucesso!
    // ==========================================
    saida << "USER_ADDED\n";
}


void buscarUsuarioPorId(MiniRede& rede, int id, std::ostream& saida) {
    NoUsuarioBST* usuario = UsuarioPorId(rede, id);
    if (usuario != nullptr) {
        saida << "USER " << usuario->user->id << " " 
              << usuario->user->username << " " 
              << usuario->user->nomeCompleto << "\n";
        return;
    }
    saida << "ERROR USER_NOT_FOUND\n";
}

void buscarUsuarioPorUsername(MiniRede& rede, const char username[], std::ostream& saida) {
    Usuario* usuario = UsuarioPorUsername(rede, username);
    if (usuario != nullptr) {
        saida << "USER " << usuario->id << " " 
              << usuario->username << " " 
              << usuario->nomeCompleto << "\n";
        return;
    }
    saida << "ERROR USER_NOT_FOUND\n";
    
}

void imprimirArvoreInOrdem(NoUsuarioBST* no, std::ostream& saida) {
    if (no == nullptr) {
        return; // Caso base da recursão
    }
    
    // 1. Visita toda a subárvore esquerda (IDs menores)
    imprimirArvoreInOrdem(no->esq, saida);
    
    // 2. Imprime o nó atual [cite: 49]
    saida << "USER " << no->user->id << " " 
          << no->user->username << " " 
          << no->user->nomeCompleto << "\n";
          
    // 3. Visita toda a subárvore direita (IDs maiores)
    imprimirArvoreInOrdem(no->dir, saida);
}

void listarUsuarios(MiniRede& rede, std::ostream& saida) {
    saida << "USERS_BEGIN\n"; // [cite: 48]
    
    // Chama a função recursiva passando a raiz da árvore
    imprimirArvoreInOrdem(rede.raizUsuarios, saida);
    
    saida << "USERS_END\n"; // [cite: 49]
}

void seguirUsuario(MiniRede& rede, int idSeguidor, int idSeguido, std::ostream& saida) {
    // TODO
}

void listarSeguindo(MiniRede& rede, int idUsuario, std::ostream& saida) {
    // TODO
}

void cadastrarPublicacao(MiniRede& rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream& saida) {
    Publicacao* nova = new Publicacao;
    nova->id = idPost;
    nova->curtidas = 0;
    nova->idAutor = idAutor;
    nova->timestamp = timestamp;
    nova->texto = texto;
    nova->listaCurtidas = nullptr;
    nova->prox = rede.listaPublicacoes;
    rede.listaPublicacoes = nova; 

    Usuario* autor = UsuarioPorId(rede, idAutor);
    if (autor != nullptr) {
        IntNode* novoPost = new IntNode;
        novoPost->id = idPost;
        novoPost->prox = autor->postsCriados;
        autor->postsCriados = novoPost;
    }
}

void curtirPublicacao(MiniRede& rede, int idUsuario, int idPost, std::ostream& saida) {
    // achar post
    Publicacao* post = AcharPublicacaoPorId(rede, idPost);
    if (post == nullptr) {
        saida << "ERROR POST_NOT_FOUND\n";
        return;
    }
    
    // descobrir se já foi curtida
    bool ja_curtida = false;
    IntNode* atual = post->listaCurtidas;
    while (atual != nullptr){
        if (atual->id == idUsuario){
            ja_curtida = true;
            break;
        }
        atual = atual->prox;
    }

    if (ja_curtida) return;

    // curtir
    post->curtidas++;

    IntNode* nova_curtida = new IntNode;
    nova_curtida->id = idUsuario
    nova_curtida->prox = post->listaCurtidas;
    post->listaCurtidas = nova_curtida;
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

// auxiliares

Usuario* UsuarioPorId(MiniRede& rede, int id) {
    NoUsuarioBST* atual = rede.raizUsuarios;
    
    while (atual != nullptr) {
        if (atual->user->id == id) {
            return atual->user; // Encontrou, retorna o ponteiro para o usuário
        }
        
        if (id < atual->user->id) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    
    return nullptr; // Não encontrou
}

Usuario* UsuarioPorUsername(MiniRede& rede, const char username[]) {
    int indice = calcularHash(username);
    NoUsuarioHash* atual = rede.hashUsernames[indice];
    
    while (atual != nullptr) {
        if (strcmp(atual->user->username, username) == 0) {
            return atual->user; // Encontrou, retorna o ponteiro para o usuário
        }
        atual = atual->prox;
    }
    
    return nullptr; // Não encontrou
}

Publicacao* AcharPublicacaoPorId(MiniRede& rede, int idPost) {
    Publicacao* post = rede.listaPublicacoes;

    while(post != nullptr){
        if (post->id == idPost) break;

        post = post->prox;
    }

    return post;
}
