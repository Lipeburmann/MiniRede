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
    NoUsuarioBST* atual = rede.raizUsuarios;
    
    // Desce na árvore procurando o ID
    while (atual != nullptr) {
        if (atual->user->id == id) {
            // Encontrou! Imprime e sai da função 
            saida << "USER " << atual->user->id << " " 
                  << atual->user->username << " " 
                  << atual->user->nomeCompleto << "\n";
            return;
        }
        
        // Se o ID buscado for menor, vai para a esquerda. Senão, vai para a direita.
        if (id < atual->user->id) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    
    // Se o while terminar e 'atual' virar nullptr, é porque não achamos 
    saida << "ERROR USER_NOT_FOUND\n";
}

void buscarUsuarioPorUsername(MiniRede& rede, const char username[], std::ostream& saida) {
    // Descobre o índice (a gaveta) usando a função Hash
    int indice = calcularHash(username);
    
    // Pega o primeiro nó da lista encadeada daquela gaveta
    NoUsuarioHash* atual = rede.hashUsernames[indice];
    
    // Percorre a lista encadeada
    while (atual != nullptr) {
        if (strcmp(atual->user->username, username) == 0) {
            // Encontrou! Imprime e sai da função [cite: 42]
            saida << "USER " << atual->user->id << " " 
                  << atual->user->username << " " 
                  << atual->user->nomeCompleto << "\n";
            return;
        }
        atual = atual->prox;
    }
    
    // Se varreu a lista toda e não achou [cite: 43]
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
    // TODO
}

void curtirPublicacao(MiniRede& rede, int idUsuario, int idPost, std::ostream& saida) {
    // achar post
    Publicacao* post = rede.listaPublicacoes;
    bool publicacao_existe = false;
    while(post != nullptr){
        if (post->id == idPost){
            publicacao_existe = true;
            break;
        }
    }

    if (!publicacao_existe) return;
    
    // descobrir se já foi curtida
    bool ja_curtida = false;
    IntNode* atual = post->listaCurtidas;
    IntNode* ultima_curtida; // para não precisar procurar de novo quando for curtir
    while (atual != nullptr){
        if (atual->id == idUsuario){
            ja_curtida = true;
            break;
        }
        ultima_curtida = atual;
        atual = atual->prox;
    }

    if (ja_curtida) return;

    // curtir
    post->curtidas++;

    IntNode* nova_curtida = new IntNode;
    nova_curtida->id = idUsuario

    ultima_curtida->prox = nova_curtida;

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

