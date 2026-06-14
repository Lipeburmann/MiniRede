#include "minirede.h"
#include <sstream>

void inicializarMiniRede(MiniRede& rede) {
    // Garante que a árvore comece vazia
    rede.raizUsuarios = nullptr;
    
    // Garante que todas as gavetas da Tabela Hash comecem vazias
    for (int i = 0; i < 101; i++) {
        rede.hashUsernames[i] = nullptr;
    }
}

int calcularHash(const char username[]) {
//Ela vai pegar o texto do username, somar os valores das letras e dividir pelo tamanho do vetor , pegando o resto da divisão
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
    std::string linha;

    while (std::getline(entrada, linha)) {
        if (linha.empty()) continue; // Ignora linhas em branco acidentais

        std::stringstream ss(linha);
        std::string comando;
        ss >> comando;

        if (comando == "END") {
            break; // Encerra o processamento de comandos
        } else if (comando == "ADD_USER") {
            int id;
            char username[TAM_USERNAME];
            char nomeCompleto[TAM_NOME];
            ss >> id >> username >> nomeCompleto;
            cadastrarUsuario(rede, id, username, nomeCompleto, saida);
        } else if (comando == "FIND_USER") {
            int id;
            ss >> id;
            buscarUsuarioPorId(rede, id, saida);
        } else if (comando == "FIND_USERNAME") {
            char username[TAM_USERNAME];
            ss >> username;
            buscarUsuarioPorUsername(rede, username, saida);
        } else if (comando == "LIST_USERS") {
            listarUsuarios(rede, saida);
        } else if (comando == "FOLLOW") {
            int idSeguidor, idSeguido;
            ss >> idSeguidor >> idSeguido;
            seguirUsuario(rede, idSeguidor, idSeguido, saida);
        } else if (comando == "LIST_FOLLOWING") {
            int idUsuario;
            ss >> idUsuario;
            listarSeguindo(rede, idUsuario, saida);
        } else if (comando == "ADD_POST") {
            int idPost, idAutor, timestamp;
            char texto[TAM_TEXTO];
            ss >> idPost >> idAutor >> timestamp;
            ss.get(); // Consumir o espaço antes do texto
            ss.getline(texto, TAM_TEXTO); // Ler o texto do post
            cadastrarPublicacao(rede, idPost, idAutor, timestamp, texto, saida);
        } else if (comando == "LIKE") {
            int idUsuario, idPost;
            ss >> idUsuario >> idPost;
            curtirPublicacao(rede, idUsuario, idPost, saida);
        } else if (comando == "GET_NOTIFICATIONS") {
            int idUsuario, k;
            ss >> idUsuario >> k;
            consultarNotificacoes(rede, idUsuario, k, saida);
        } else if (comando == "FEED") {
            int idUsuario, k;
            ss >> idUsuario >> k;
            gerarFeed(rede, idUsuario, k, saida);
        } else if (comando == "TOP_POSTS") {
            int k;
            ss >> k;
            listarTopPosts(rede, k, saida);
        }
    }
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
    Usuario* usuario = UsuarioPorId(rede, id);  
    if (usuario != nullptr) {
        saida << "USER " << usuario->id << " " 
              << usuario->username << " " 
              << usuario->nomeCompleto << "\n";
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
    saida << "USERS_BEGIN\n"; // 
    
    // Chama a função recursiva passando a raiz da árvore
    imprimirArvoreInOrdem(rede.raizUsuarios, saida);
    
    saida << "USERS_END\n"; // 
}

void seguirUsuario(MiniRede& rede, int idSeguidor, int idSeguido, std::ostream& saida) {
    // TODO
}

void listarSeguindo(MiniRede& rede, int idUsuario, std::ostream& saida) {
    Usuario* usuario = UsuarioPorId(rede, idUsuario);
    
    if (usuario == nullptr) {
        saida << "ERROR USER_NOT_FOUND\n"; 
        return;
    }

    saida << "FOLLOWING BEGIN\n";

    IntNode* atual = usuario->seguidos;
    
    while (atual != nullptr) {
        
        // Pega o ID da lista e acha o objeto completo daquele usuário
        Usuario* usuarioEncontrado = UsuarioPorId(rede, atual->id);
        
        if (usuarioEncontrado != nullptr) {
            // Imprime os dados usando a nova variável
            saida << "USER " << usuarioEncontrado->id << " " 
                  << usuarioEncontrado->username << " " 
                  << usuarioEncontrado->nomeCompleto << "\n";
        }
        
        // Pula para o próximo ID da lista
        atual = atual->prox;
    }

    saida << "FOLLOWING_END\n";
}

void cadastrarPublicacao(MiniRede& rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream& saida) {
    // 1. Verificar se o autor existe ANTES de criar o post 
    Usuario* autor = UsuarioPorId(rede, idAutor);
    if (autor == nullptr) {
        saida << "ERROR USER_NOT_FOUND\n"; // 
        return;
    }

    // 2. Verificar se o ID do post já existe na rede 
    Publicacao* postExistente = AcharPublicacaoPorId(rede, idPost);
    if (postExistente != nullptr) {
        saida << "ERROR POST_EXISTS\n"; // 
        return;
    }

    // 3. Se passou pelas validações, podemos criar o post na memória
    Publicacao* nova = new Publicacao;
    nova->id = idPost;
    nova->curtidas = 0;
    nova->idAutor = idAutor;
    nova->timestamp = timestamp;
    strcpy(nova->texto, texto); // CORREÇÃO: Copiando a string corretamente
    nova->listaCurtidas = nullptr;
    
    // Inserir na lista global da rede
    nova->prox = rede.listaPublicacoes;
    rede.listaPublicacoes = nova; 

    // Inserir na lista de posts do autor
    IntNode* novoPost = new IntNode;
    novoPost->id = idPost;
    novoPost->prox = autor->postsCriados;
    autor->postsCriados = novoPost;
    
    // 4. Imprimir sucesso! 
    saida << "POST_ADDED\n";
}

void curtirPublicacao(MiniRede& rede, int idUsuario, int idPost, std::ostream& saida) {
    // 1. Verificar se o usuário que está curtindo existe ANTES de tudo
    Usuario* userCurtindo = UsuarioPorId(rede, idUsuario);
    if (userCurtindo == nullptr) {
        saida << "ERROR USER_NOT_FOUND\n"; // 
        return;
    }

    // 2. Achar post
    Publicacao* post = AcharPublicacaoPorId(rede, idPost);
    if (post == nullptr) {
        saida << "ERROR POST_NOT_FOUND\n"; // [cite: 81]
        return;
    }
    
    // 3. Descobrir se já foi curtida
    bool ja_curtida = false;
    IntNode* atual = post->listaCurtidas;
    while (atual != nullptr){
        if (atual->id == idUsuario){
            ja_curtida = true;
            break;
        }
        atual = atual->prox;
    }

    if (ja_curtida) {
        saida << "ERROR ALREADY_LIKED\n"; // 
        return;
    }

    // 4. Curtir
    post->curtidas++;

    IntNode* nova_curtida = new IntNode;
    nova_curtida->id = idUsuario; // O ponto e vírgula arrumado aqui!
    nova_curtida->prox = post->listaCurtidas;
    post->listaCurtidas = nova_curtida;

    // 5. Enviar Notificação para o Autor do Post
    Usuario* autor = UsuarioPorId(rede, post->idAutor);
    if (autor != nullptr) {
        Notificacao* novaNotif = new Notificacao;
        novaNotif->tipo = NOTIF_LIKE;
        novaNotif->idOrigem = idUsuario;
        novaNotif->idPost = idPost;
        novaNotif->prox = nullptr;

        // Inserir no final da fila (FIFO)
        if (autor->filaNotif.inicio == nullptr) {
            autor->filaNotif.inicio = novaNotif;
            autor->filaNotif.fim = novaNotif;
        } else {
            autor->filaNotif.fim->prox = novaNotif;
            autor->filaNotif.fim = novaNotif;
        }
    }

    // 6. Mensagem de Sucesso
    saida << "LIKED\n"; // 
}

void consultarNotificacoes(MiniRede& rede, int idUsuario, int k, std::ostream& saida) {
    // 1. Busca o usuário dono da fila de notificações
    Usuario* usuario = UsuarioPorId(rede, idUsuario);
    
    if (usuario == nullptr) {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

    // 2. Imprime o início do bloco de notificações
    saida << "NOTIFICATIONS_BEGIN\n";

    int contador = 0;

    // 3. Loop: remove enquanto não atingir 'k' E a fila não estiver vazia
    while (contador < k && usuario->filaNotif.inicio != nullptr) {
        
        // Pega o primeiro nó (o início da fila)
        Notificacao* noRemovido = usuario->filaNotif.inicio;
        
        // 4. Verifica o tipo da notificação para imprimir no formato correto
        if (noRemovido->tipo == NOTIF_FOLLOW) {
            saida << "FOLLOW " << noRemovido->idOrigem << "\n";
        } 
        else if (noRemovido->tipo == NOTIF_LIKE) {
            saida << "LIKE " << noRemovido->idOrigem << " " << noRemovido->idPost << "\n";
        }

        // Avança o início da fila para o próximo nó
        usuario->filaNotif.inicio = usuario->filaNotif.inicio->prox;
        
        // Se a fila ficou vazia após o avanço, o 'fim' também precisa apontar para nullptr
        if (usuario->filaNotif.inicio == nullptr) {
            usuario->filaNotif.fim = nullptr;
        }

        // 5. Deleta o nó antigo da memória
        delete noRemovido;

        // Incrementa o contador para não rodar mais do que 'k' vezes
        contador++;
    }

    // 6. Imprime o fim do bloco
    saida << "NOTIFICATIONS_END\n";
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
