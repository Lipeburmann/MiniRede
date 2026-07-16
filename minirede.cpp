#include "minirede.h"
#include <sstream>

void inicializarMiniRede(MiniRede &rede)
{
    rede.raizUsuarios = nullptr;
    rede.listaPublicacoes = nullptr;
    for (int i = 0; i < TAM_HASH; i++)
    {
        rede.hashUsernames[i] = nullptr;
    }
}

int calcularHash(const char username[])
{
    int hash = 0;
    for (int i = 0; username[i] != '\0'; i++)
    {
        hash = (hash + username[i]) % TAM_HASH;
    }
    return hash;
}

void liberarMiniRede(MiniRede &rede)
{
    // Limpa a lista global de publicações
    Publicacao *postAtual = rede.listaPublicacoes;
    while (postAtual != nullptr)
    {
        Publicacao *temp = postAtual;
        postAtual = postAtual->prox_global;
        liberarListaIntNode(temp->listaCurtidas);
        liberarListaComentarios(temp->listaComentarios);
        delete temp;
    }
    rede.listaPublicacoes = nullptr;
    //limpa arvore e hash
    liberarArvore(rede.raizUsuarios);
    rede.raizUsuarios = nullptr;
    liberarTabelaHash(rede.hashUsernames);
}

void processarComandos(MiniRede &rede, std::istream &entrada, std::ostream &saida)
{
    std::string linha;

    while (std::getline(entrada, linha))
    {
        if (linha.empty())
            continue;

        std::stringstream ss(linha);
        std::string comando;
        ss >> comando;

        if (comando == "END")
        {
            break;
        }
        else if (comando == "ADD_USER")
        {
            int id;
            char username[TAM_USERNAME];
            char nomeCompleto[TAM_NOME];
            ss >> id >> username;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            ss.get();
            ss.getline(nomeCompleto, TAM_NOME);
            cadastrarUsuario(rede, id, username, nomeCompleto, saida);
        }
        else if (comando == "FIND_USER")
        {
            int id;
            ss >> id;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            buscarUsuarioPorId(rede, id, saida);
        }
        else if (comando == "FIND_USERNAME")
        {
            char username[TAM_USERNAME];
            ss >> username;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            buscarUsuarioPorUsername(rede, username, saida);
        }
        else if (comando == "LIST_USERS")
        {
            listarUsuarios(rede, saida);
        }
        else if (comando == "FOLLOW")
        {
            int idSeguidor, idSeguido;
            ss >> idSeguidor >> idSeguido;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            seguirUsuario(rede, idSeguidor, idSeguido, saida);
        }
        else if (comando == "LIST_FOLLOWING")
        {
            int idUsuario;
            ss >> idUsuario;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            listarSeguindo(rede, idUsuario, saida);
        }
        else if (comando == "ADD_POST")
        {
            int idPost, idAutor, timestamp;
            char texto[TAM_TEXTO];
            ss >> idPost >> idAutor >> timestamp;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            ss.get();
            ss.getline(texto, TAM_TEXTO);
            cadastrarPublicacao(rede, idPost, idAutor, timestamp, texto, saida);
        }
        else if (comando == "LIKE")
        {
            int idUsuario, idPost;
            ss >> idUsuario >> idPost;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            curtirPublicacao(rede, idUsuario, idPost, saida);
        }
        else if (comando == "GET_NOTIFICATIONS")
        {
            int idUsuario, k;
            ss >> idUsuario >> k;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            consultarNotificacoes(rede, idUsuario, k, saida);
        }
        else if (comando == "FEED")
        {
            int idUsuario, k;
            ss >> idUsuario >> k;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            gerarFeed(rede, idUsuario, k, saida);
        }
        else if (comando == "TOP_POSTS")
        {
            int k;
            ss >> k;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            listarTopPosts(rede, k, saida);
        }
        else if (comando == "UNFOLLOW")
        {
            int idSeguidor, idSeguido;
            ss >> idSeguidor >> idSeguido;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            unfollowUsuario(rede, idSeguidor, idSeguido, saida);
        }
        else if (comando == "REMOVE_POST")
        {
            int idPost;
            ss >> idPost;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            removerPost(rede, idPost, saida);
        }
        else if (comando == "COMMENT")
        {
            int idUsuario, idPost, idComentario;
            char texto[TAM_TEXTO];
            ss >> idUsuario >> idPost >> idComentario;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            ss.get();
            ss.getline(texto, TAM_TEXTO);
            comentar(rede, idUsuario, idPost, idComentario, texto, saida);
        }
        else if (comando == "LIST_COMMENTS")
        {
            int idPost;
            ss >> idPost;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            listarComentarios(rede, idPost, saida);
        }
        else if (comando == "LIST_RECENT_POSTS")
        {
            int k;
            ss >> k;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            imprimirXPostsGlobal(rede, k, saida);
        }
        else if (comando == "SEARCH_POSTS")
        {
            char palavra[TAM_TEXTO];
            ss >> palavra;
            if (ss.fail())
            {
                saida << "ERROR INVALID_COMMAND\n";
                continue;
            }
            buscarPublicacoesPorPalavra(rede, palavra, saida);
        }
        else if (comando == "REMOVE_RECENT_POST")
        {
            removerPostMaisRecenteGlobal(rede, saida);
        }
        else
        {
            saida << "ERROR INVALID_COMMAND\n";
        }
    }
}

void cadastrarUsuario(MiniRede &rede, int id, const char username[], const char nomeCompleto[], std::ostream &saida)
{
    // verifica se o id ja existe 
    bool idExiste = false;
    Usuario *usuario_possivel = UsuarioPorId(rede, id);
    if (usuario_possivel != nullptr)
    {
        idExiste = true;
    }


    // verifica se username ja existe 
    bool userExiste = false;

    usuario_possivel = UsuarioPorUsername(rede, username);
    if (usuario_possivel != nullptr)
    {
        userExiste = true;
    }

    if (idExiste || userExiste)
    {
        saida << "ERROR USER_EXISTS\n";
        return; 
    }

    //se passou no teste,cadastra novo usuario
    Usuario *novoUser = new Usuario;
    novoUser->id = id;
    strcpy(novoUser->username, username);
    strcpy(novoUser->nomeCompleto, nomeCompleto);
    novoUser->seguidos = nullptr;
    novoUser->postsCriados = nullptr;
    novoUser->filaNotif.inicio = nullptr;
    novoUser->filaNotif.fim = nullptr;

    // insere na arvore binária pelo id
    NoUsuarioBST *novoNoBST = new NoUsuarioBST{novoUser, nullptr, nullptr};
    //primeiro ve se a arvore ta vazia
    if (rede.raizUsuarios == nullptr)
    {
        rede.raizUsuarios = novoNoBST;
    }
    //depois ve onde ele deve ficar
    else
    {
        NoUsuarioBST *pai = nullptr;
        NoUsuarioBST *temp = rede.raizUsuarios;

        while (temp != nullptr)
        {
            pai = temp;
            if (id < temp->user->id)
                temp = temp->esq;
            else
                temp = temp->dir;
        }

        if (id < pai->user->id)
            pai->esq = novoNoBST;
        else
            pai->dir = novoNoBST;
    }

    // insere na Tabela Hash pelo username
    int indiceHash = calcularHash(username);
    NoUsuarioHash *novoNoHash = new NoUsuarioHash{novoUser, rede.hashUsernames[indiceHash]};
    rede.hashUsernames[indiceHash] = novoNoHash;
    saida << "USER_ADDED\n";
}

void buscarUsuarioPorId(MiniRede &rede, int id, std::ostream &saida)
{
    Usuario *usuario = UsuarioPorId(rede, id);
    if (usuario != nullptr)
    {
        saida << "USER " << usuario->id << " "
              << usuario->username << " "
              << usuario->nomeCompleto << "\n";
        return;
    }
    saida << "ERROR USER_NOT_FOUND\n";
}

void buscarUsuarioPorUsername(MiniRede &rede, const char username[], std::ostream &saida)
{
    Usuario *usuario = UsuarioPorUsername(rede, username);
    if (usuario != nullptr)
    {
        saida << "USER " << usuario->id << " "
              << usuario->username << " "
              << usuario->nomeCompleto << "\n";
        return;
    }
    saida << "ERROR USER_NOT_FOUND\n";
}

void imprimirArvoreInOrdem(NoUsuarioBST *no, std::ostream &saida)
{
    if (no == nullptr)
    {
        return; // caso base da recursao
    }

    // visita toda a subarvore esquerda 
    imprimirArvoreInOrdem(no->esq, saida);

    // imprime o nó atual 
    saida << "USER " << no->user->id << " "
          << no->user->username << " "
          << no->user->nomeCompleto << "\n";

    // visita toda a subarvore direita 
    imprimirArvoreInOrdem(no->dir, saida);
}

void listarUsuarios(MiniRede &rede, std::ostream &saida)
{
    saida << "USERS_BEGIN\n"; 
    imprimirArvoreInOrdem(rede.raizUsuarios, saida);
    saida << "USERS_END\n"; 
}

void seguirUsuario(MiniRede &rede, int idSeguidor, int idSeguido, std::ostream &saida)
{
    if (idSeguidor == idSeguido)
    {
        saida << "ERROR CANNOT_FOLLOW_SELF\n";
        return;
    }
    Usuario *seguidor = UsuarioPorId(rede, idSeguidor);
    Usuario *seguido = UsuarioPorId(rede, idSeguido);
    if (seguidor == nullptr || seguido == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

     // percorre a lista de quem segue
    IntNode *atual = seguidor->seguidos;
    IntNode *anterior = nullptr;
    while (atual != nullptr && atual->id < idSeguido)
    {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual != nullptr && atual->id == idSeguido)
    {
        saida << "ERROR ALREADY_FOLLOWING\n";
        return;
    }

    // cria o novo nó de id e insere na posição correta
    IntNode *novoSeguido = new IntNode;
    novoSeguido->id = idSeguido;
    novoSeguido->prox = atual;

    if (anterior == nullptr)
    {
        // insere no início 
        seguidor->seguidos = novoSeguido;
    }
    else
    {
        // insere no meio ou no fim
        anterior->prox = novoSeguido;
    }

    notificarUsuario(rede, idSeguido, idSeguidor, -1, NOTIF_FOLLOW); // O '-1' indica que não tem post associado

    saida << "FOLLOWED\n";
}

void unfollowUsuario(MiniRede &rede, int idSeguidor, int idSeguido, std::ostream &saida)
{
    if (idSeguidor == idSeguido)
    {
        saida << "ERROR CANNOT_UNFOLLOW_SELF\n";
        return;
    }
    Usuario *seguidor = UsuarioPorId(rede, idSeguidor);
    Usuario *seguido = UsuarioPorId(rede, idSeguido);

    if (seguidor == nullptr || seguido == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

    // percorre a lista
    IntNode *atual = seguidor->seguidos;
    IntNode *anterior = nullptr;
    while (atual != nullptr && atual->id < idSeguido)
    {
        anterior = atual;
        atual = atual->prox;
    }
    if (atual == nullptr || atual->id != idSeguido)
    {
        saida << "ERROR NOT_FOLLOWING\n";
        return;
    }

    //remoçao
    if (anterior == nullptr)
    {
        // o cara que queremos remover é o primeiro da fila,inicio vira o prox
        seguidor->seguidos = atual->prox;
    }
    else
    {
        // o cara está no meio ou no fim
        anterior->prox = atual->prox;
    }
    delete atual;
    saida << "UNFOLLOWED\n";
}

void listarSeguindo(MiniRede &rede, int idUsuario, std::ostream &saida)
{
    Usuario *usuario = UsuarioPorId(rede, idUsuario);

    if (usuario == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

    saida << "FOLLOWING_BEGIN\n";
    //percorre a lista de seguidos e vai imprimindo a cada usuario que encontrar na lista usando o usuario por id 
    IntNode *atual = usuario->seguidos;

    while (atual != nullptr)
    {
        Usuario *usuarioEncontrado = UsuarioPorId(rede, atual->id);

        if (usuarioEncontrado != nullptr)
        {
            saida << "USER " << usuarioEncontrado->id << " "
                  << usuarioEncontrado->username << " "
                  << usuarioEncontrado->nomeCompleto << "\n";
        }

        // pula para o proximo id da lista
        atual = atual->prox;
    }

    saida << "FOLLOWING_END\n";
}

void cadastrarPublicacao(MiniRede &rede, int idPost, int idAutor, int timestamp, const char texto[], std::ostream &saida)
{
    Usuario *autor = UsuarioPorId(rede, idAutor);
    if (autor == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n"; //
        return;
    }
    Publicacao *postExistente = PublicacaoPorId(rede, idPost);
    if (postExistente != nullptr)
    {
        saida << "ERROR POST_EXISTS\n"; //
        return;
    }

    //se passou pelos testes cria o post
    Publicacao *nova = new Publicacao;
    nova->id = idPost;
    nova->curtidas = 0;
    nova->idAutor = idAutor;
    nova->timestamp = timestamp;
    strcpy(nova->texto, texto); 
    nova->listaCurtidas = nullptr;
    nova->listaComentarios = nullptr;

    //insere na lista global da rede
    nova->prox_global = rede.listaPublicacoes;
    rede.listaPublicacoes = nova;

    //insere na lista de posts do autor
    nova->prox_autor = autor->postsCriados;
    autor->postsCriados = nova;
    saida << "POST_ADDED\n";
}

void curtirPublicacao(MiniRede &rede, int idUsuario, int idPost, std::ostream &saida)
{
    Usuario *userCurtindo = UsuarioPorId(rede, idUsuario);
    if (userCurtindo == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n"; 
        return;
    }


    Publicacao *post = PublicacaoPorId(rede, idPost);
    if (post == nullptr)
    {
        saida << "ERROR POST_NOT_FOUND\n"; 
        return;
    }

   
    bool ja_curtida = false;
    IntNode *atual = post->listaCurtidas;
    while (atual != nullptr)
    {
        if (atual->id == idUsuario)
        {
            ja_curtida = true;
            break;
        }
        atual = atual->prox;
    }

    if (ja_curtida)
    {
        saida << "ERROR ALREADY_LIKED\n"; //
        return;
    }

    // curtir
    post->curtidas++;

    IntNode *nova_curtida = new IntNode;
    nova_curtida->id = idUsuario;
    nova_curtida->prox = post->listaCurtidas;
    post->listaCurtidas = nova_curtida;
    notificarUsuario(rede, post->idAutor, idUsuario, idPost, NOTIF_LIKE);
    saida << "LIKED\n"; //
}

void consultarNotificacoes(MiniRede &rede, int idUsuario, int k, std::ostream &saida)
{
    Usuario *usuario = UsuarioPorId(rede, idUsuario);

    if (usuario == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }
    saida << "NOTIFICATIONS_BEGIN\n";

    int contador = 0;
    while (contador < k && usuario->filaNotif.inicio != nullptr)
    {

        // pega  o início da fila
        Notificacao *noRemovido = usuario->filaNotif.inicio;

        //verifica o tipo da notificação 
        if (noRemovido->tipo == NOTIF_FOLLOW)
        {
            saida << "NOTIFICATION FOLLOW " << noRemovido->idOrigem << "\n";
        }
        else if (noRemovido->tipo == NOTIF_LIKE)
        {
            saida << "NOTIFICATION LIKE " << noRemovido->idOrigem << " " << noRemovido->idPost << "\n";
        }
        else if (noRemovido->tipo == NOTIF_COMMENT)
        {
            saida << "NOTIFICATION COMMENT " << noRemovido->idOrigem << " " << noRemovido->idPost << "\n";
        }

        // Avança o inicio da fila 
        usuario->filaNotif.inicio = usuario->filaNotif.inicio->prox;

        // se a fila ficou vazia após o avanço,faz o fim apontar pra null
        if (usuario->filaNotif.inicio == nullptr)
        {
            usuario->filaNotif.fim = nullptr;
        }
        delete noRemovido;
        contador++; //contador pra nao rodar mais que k vezes
    }
    saida << "NOTIFICATIONS_END\n";
}

void gerarFeed(MiniRede &rede, int idUsuario, int k, std::ostream &saida)
{
    Usuario *usuario = UsuarioPorId(rede, idUsuario);
    if (usuario == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

    NoPublicacao *todosPosts = ListarPostsSeguindo(rede, usuario); 
    NoPublicacao *postsEncontrados = SelecionarKPostsMaisRecentes(todosPosts, k);
    liberarListaPublicacao(todosPosts); 

    postsEncontrados = OrdenarPostsPorTimestamp(postsEncontrados); //ordena os posts por timestamp mais recentes primeiro

    //imprimir os posts do feed
    saida << "FEED_BEGIN\n";
    imprimirXPosts(postsEncontrados, k, saida);
    saida << "FEED_END\n";

    liberarListaPublicacao(postsEncontrados);
}

void listarTopPosts(MiniRede &rede, int k, std::ostream &saida)
{
    if (k <= 0 || rede.listaPublicacoes == nullptr)
    {
        saida << "TOP_POSTS_BEGIN\nTOP_POSTS_END\n";
        return;
    }

    //acha as k publicações com mais curtidas na lista global de publicações da rede e ordena com Bubble Sort

    NoPublicacao *k_mais_curtidas = SelecionarKPostsMaisCurtidos(rede.listaPublicacoes, k); 
    k_mais_curtidas = OrdenarPostsPorCurtidas(k_mais_curtidas);  

    // imprime os k mais curtidos
    saida << "TOP_POSTS_BEGIN\n";
    imprimirXPosts(k_mais_curtidas, k, saida); 
    saida << "TOP_POSTS_END\n";

    liberarListaPublicacao(k_mais_curtidas);
}

void removerPost(MiniRede &rede, int idPost, std::ostream &saida)
{
    NoPublicacao *resultadoBusca = AcharPublicacaoEanteriorPorId(rede, idPost);
    if (resultadoBusca == nullptr)
    {
        saida << "ERROR POST_NOT_FOUND\n";
        return; 
    }


    Publicacao *postAtual = resultadoBusca->post;
    Publicacao *postAnterior;

    if (resultadoBusca->prox == nullptr)
    {
        postAnterior = nullptr; // o post a remover é o primeiro da lista
    }
    else
    {
        postAnterior = resultadoBusca->prox->post;
    }
    liberarListaPublicacao(resultadoBusca);

    // remove da lista global
    if (postAnterior == nullptr)
    {
        // o post a remover é o primeiro da lista
        rede.listaPublicacoes = postAtual->prox_global;
    }
    else
    {
        // o post a remover tá no meio ou no fim da lista
        postAnterior->prox_global = postAtual->prox_global;
    }

    // remove da lista do autor
    Usuario *autor = UsuarioPorId(rede, postAtual->idAutor);
    if (autor != nullptr)
    {
        Publicacao *postAutorAtual = autor->postsCriados;
        Publicacao *postAutorAnterior = nullptr;

        while (postAutorAtual != nullptr && postAutorAtual->id != idPost)
        {
            postAutorAnterior = postAutorAtual;
            postAutorAtual = postAutorAtual->prox_autor;
        }

        if (postAutorAtual != nullptr)
        {
            if (postAutorAnterior == nullptr)
            {
                autor->postsCriados = postAutorAtual->prox_autor;
            }
            else
            {
                postAutorAnterior->prox_autor = postAutorAtual->prox_autor;
            }
        }
    }

    saida << "POST_REMOVED\n";
    liberarListaIntNode(postAtual->listaCurtidas);
    liberarListaComentarios(postAtual->listaComentarios);
    delete postAtual;
}

void removerPostMaisRecenteGlobal(MiniRede &rede, std::ostream &saida){
    if (rede.listaPublicacoes == nullptr)
    {
        saida << "ERROR NO_POSTS\n";
        return;
    }

    Publicacao *postMaisRecente = rede.listaPublicacoes; // O primeiro da lista global é o mais recente
    removerPost(rede, postMaisRecente->id, saida);
}

void comentar(MiniRede &rede, int idUsuario, int idPost, int idComentario, const char texto[], std::ostream &saida)
{
    Publicacao *post = PublicacaoPorId(rede, idPost);
    if (post == nullptr)
    {
        saida << "ERROR POST_NOT_FOUND\n";
        return;
    }

    Usuario *usuario = UsuarioPorId(rede, idUsuario);
    if (usuario == nullptr)
    {
        saida << "ERROR USER_NOT_FOUND\n";
        return;
    }

    Comentario *novoComentario = new Comentario;
    novoComentario->idAutor = idUsuario;
    strcpy(novoComentario->texto, texto);
    novoComentario->id = idComentario;
    novoComentario->prox = post->listaComentarios;
    post->listaComentarios = novoComentario;

    saida << "COMMENT_ADDED\n";
    notificarUsuario(rede, post->idAutor, idUsuario, idPost, NOTIF_COMMENT);
}

void listarComentarios(MiniRede &rede, int idPost, std::ostream &saida)
{
    Publicacao *post = PublicacaoPorId(rede, idPost);
    if (post == nullptr)
    {
        saida << "ERROR POST_NOT_FOUND\n";
        return;
    }

    Comentario *comentarioAtual = post->listaComentarios;

    saida << "COMMENTS_BEGIN\n";
    while (comentarioAtual != nullptr)
    {
        saida << "COMMENT " << comentarioAtual->id << " "
              << comentarioAtual->idAutor << " "
              << comentarioAtual->texto << "\n";
        comentarioAtual = comentarioAtual->prox;
    }
    saida << "COMMENTS_END\n";
}

void imprimirXPostsGlobal(MiniRede &rede, int x, std::ostream &saida)
{
    saida << "POSTS_BEGIN\n";
    int contador = 0;
    Publicacao *atual = rede.listaPublicacoes;
    while (contador < x && atual != nullptr)
    {
        saida << "POST " << atual->id << " " << atual->idAutor << " "
              << atual->timestamp << " "
              << atual->curtidas << " "
              << atual->texto << "\n";
        atual = atual->prox_global;
        contador++;
    }
    saida << "POSTS_END\n";
}

int main()
{
    MiniRede rede;

    inicializarMiniRede(rede);
    processarComandos(rede, std::cin, std::cout);
    liberarMiniRede(rede);

    return 0;
}

// --------------- auxiliares ---------------------

//***Notificações

void notificarUsuario(MiniRede &rede, int idUsuario, int idOrigem, int idPost, TipoNotificacao tipo)
{
    Usuario *usuario = UsuarioPorId(rede, idUsuario);
    if (usuario == nullptr)
    {
        return;
    }

    Notificacao *novaNotif = new Notificacao;
    novaNotif->tipo = tipo;
    novaNotif->idOrigem = idOrigem;
    novaNotif->idPost = idPost;
    novaNotif->prox = nullptr;

    // logica de fila (fifo)
    if (usuario->filaNotif.inicio == nullptr)
    {
        usuario->filaNotif.inicio = novaNotif;
        usuario->filaNotif.fim = novaNotif;
    }
    else
    {
        usuario->filaNotif.fim->prox = novaNotif;
        usuario->filaNotif.fim = novaNotif;
    }
}

// ***Feed e ranking

void imprimirXPosts(NoPublicacao *post, int x, std::ostream &saida)
{
    int contador = 0;
    NoPublicacao *atualFeed = post;
    while (contador < x && atualFeed != nullptr)
    {
        Publicacao *p = atualFeed->post;
        saida << "POST " << p->id << " " << p->idAutor << " "
              << p->timestamp << " "
              << p->curtidas << " "
              << p->texto << "\n";
        atualFeed = atualFeed->prox;
        contador++;
    }
}


NoPublicacao *ListarPostsSeguindo(MiniRede &rede, Usuario *usuario)
{
    // cria uma lista encadeada de NoPublicacao com os posts dos usuários que ele segue
    NoPublicacao *postsEncontrados = nullptr;
    IntNode *seguindo_atual = usuario->seguidos;
    while (seguindo_atual != nullptr)
    {
        Usuario *seguido = UsuarioPorId(rede, seguindo_atual->id);
        if (seguido != nullptr)
        {
            NoPublicacao *postsSeguido = SelecionarKPosts(seguido->postsCriados, 1000000, BUSCA_USUARIO);
            if (postsEncontrados == nullptr)
            {
                postsEncontrados = postsSeguido;
            }
            else
            {
                NoPublicacao *fim = postsEncontrados;
                while (fim->prox != nullptr) fim = fim->prox;
                fim->prox = postsSeguido;
            }
        }
        seguindo_atual = seguindo_atual->prox;
    }
    return postsEncontrados;
}

NoPublicacao *SelecionarKPosts(Publicacao *posts, int k, TipoBuscaPublicacao tipo)
{ // seleciona os 'k' posts de uma lista qualquer, caso queira todos basta usar um k muito grande
    NoPublicacao *k_posts = nullptr;
    int contador = 0;
    Publicacao *atual = posts;
    while (atual != nullptr && contador < k)
    {
        NoPublicacao *novo_no = new NoPublicacao;
        novo_no->post = atual;
        novo_no->prox = k_posts;
        k_posts = novo_no;
        if (tipo == BUSCA_USUARIO)
        {
            atual = atual->prox_autor; // acessa o próximo post do mesmo autor usando prox_autor
        }
        else
        {
            atual = atual->prox_global;
        }
        contador++;
    }
    return k_posts;
}

NoPublicacao *SelecionarKPostsMaisRecentes(NoPublicacao *posts, int k)
{ // seleciona os 'k' posts mais recentes da lista encadeada de NoPublicacao,que tem os posts dos usuários que ele segue
    NoPublicacao *k_mais_recentes = nullptr;
    int contador = 0;
    NoPublicacao *atual = posts;
    while (atual != nullptr && contador < k)
    { // pega os 'k' primeiros posts da lista para começar a comparação
        NoPublicacao *novo_no = new NoPublicacao;
        novo_no->post = atual->post;
        novo_no->prox = k_mais_recentes;
        k_mais_recentes = novo_no;
        atual = atual->prox;
        contador++;
    }

    while (atual != nullptr)
    {
        // Acha o post com o menor timestamp na lista de 'k' mais recentes
        NoPublicacao *menos_recente = k_mais_recentes;
        NoPublicacao *ptr = k_mais_recentes;
        while (ptr != nullptr)
        {
            if (ptr->post->timestamp < menos_recente->post->timestamp || (ptr->post->timestamp == menos_recente->post->timestamp && ptr->post->id > menos_recente->post->id))
            {
                menos_recente = ptr;
            }
            ptr = ptr->prox;
        }
        // Se o post atual for mais recente que o menos recente, substitui
        if (atual->post->timestamp > menos_recente->post->timestamp || (atual->post->timestamp == menos_recente->post->timestamp && atual->post->id < menos_recente->post->id))
        {
            menos_recente->post = atual->post;
        }
        atual = atual->prox;
    }

    return k_mais_recentes;
}

NoPublicacao *OrdenarPostsPorTimestamp(NoPublicacao *posts)
{ // ordena os posts por timestamp decrescente ,mais recentes primeiro, usando Bubble Sort
    if (posts == nullptr)
        return nullptr;

    bool trocou;
    do
    {
        trocou = false;
        NoPublicacao *ptr = posts;
        while (ptr != nullptr && ptr->prox != nullptr)
        {
            if (ptr->post->timestamp < ptr->prox->post->timestamp ||
                (ptr->post->timestamp == ptr->prox->post->timestamp && ptr->post->id > ptr->prox->post->id))
            {
                // troca os posts
                Publicacao *tempPost = ptr->post;
                ptr->post = ptr->prox->post;
                ptr->prox->post = tempPost;
                trocou = true;
            }
            ptr = ptr->prox;
        }
    } while (trocou);

    return posts; 
}

NoPublicacao *SelecionarKPostsMaisCurtidos(Publicacao *posts, int k)
{ // seleciona os 'k' posts mais curtidos da lista global de publicações da rede
    NoPublicacao *k_mais_curtidos = nullptr;
    int contador = 0;
    Publicacao *atual = posts;
    while (atual != nullptr && contador < k)
    { // pega os 'k' primeiros posts da lista para começar a comparação
        NoPublicacao *novo_no = new NoPublicacao;
        novo_no->post = atual;
        novo_no->prox = k_mais_curtidos;
        k_mais_curtidos = novo_no;
        atual = atual->prox_global;
        contador++;
    }

    while (atual != nullptr)
    {
        // acha o post com o menor número de curtidas na lista de 'k' mais curtidos
        NoPublicacao *menos_curtido = k_mais_curtidos;
        NoPublicacao *ptr = k_mais_curtidos;
        while (ptr != nullptr)
        {
            if (ptr->post->curtidas < menos_curtido->post->curtidas || (ptr->post->curtidas == menos_curtido->post->curtidas && ptr->post->id > menos_curtido->post->id))
            {
                menos_curtido = ptr;
            }
            ptr = ptr->prox;
        }
        // se o post atual tiver mais curtidas que o menos curtido, substitui
        if (atual->curtidas > menos_curtido->post->curtidas ||
            (atual->curtidas == menos_curtido->post->curtidas && atual->id < menos_curtido->post->id))
        {
            menos_curtido->post = atual;
        }
        atual = atual->prox_global;
    }

    return k_mais_curtidos;
}

NoPublicacao *OrdenarPostsPorCurtidas(NoPublicacao *posts)
{ // ordena os posts por curtidas decrescente usando Bubble Sort
    if (posts == nullptr)
        return nullptr;

    bool trocou;
    do
    {
        trocou = false;
        NoPublicacao *ptr = posts;
        while (ptr != nullptr && ptr->prox != nullptr)
        {
            if (ptr->post->curtidas < ptr->prox->post->curtidas || (ptr->post->curtidas == ptr->prox->post->curtidas && ptr->post->id > ptr->prox->post->id))
            {
                // Troca os posts
                Publicacao *tempPost = ptr->post;
                ptr->post = ptr->prox->post;
                ptr->prox->post = tempPost;
                trocou = true;
            }
            ptr = ptr->prox;
        }
    } while (trocou);

    return posts; // retorna a cabeça da lista, que agora ta ordenada
}

void liberarListaPublicacao(NoPublicacao *inicio)
{
    while (inicio != nullptr)
    {
        NoPublicacao *temp = inicio;
        inicio = inicio->prox;
        delete temp;
    }
}

//***Busca

Usuario *UsuarioPorId(MiniRede &rede, int id)
{
    NoUsuarioBST *atual = rede.raizUsuarios;

    while (atual != nullptr)
    {
        if (atual->user->id == id)
        {
            return atual->user; 
        }

        if (id < atual->user->id)
        {
            atual = atual->esq;
        }
        else
        {
            atual = atual->dir;
        }
    }

    return nullptr; 
}

Usuario *UsuarioPorUsername(MiniRede &rede, const char username[])
{
    int indice = calcularHash(username);
    NoUsuarioHash *atual = rede.hashUsernames[indice];

    while (atual != nullptr)
    {
        if (strcmp(atual->user->username, username) == 0)
        {
            return atual->user; 
        }
        atual = atual->prox;
    }

    return nullptr; 
}

Publicacao *PublicacaoPorId(MiniRede &rede, int idPost)
{
    NoPublicacao *busca = AcharPublicacaoEanteriorPorId(rede, idPost);
    if (busca == nullptr)
    {
        return nullptr;
    }
    Publicacao *post = busca->post;
    liberarListaPublicacao(busca);
    return post;
}

NoPublicacao *AcharPublicacaoEanteriorPorId(MiniRede &rede, int idPost)
{ // devolve o primeiro nó de uma lista com o post encontrado e o segundo nó com o post anterior ao encontrado ou nullptr se for o primeiro da lista
    Publicacao *post = rede.listaPublicacoes;
    Publicacao *postAnterior = nullptr;

    while (post != nullptr)
    {
        if (post->id == idPost)
            break;
        postAnterior = post;
        post = post->prox_global;
    }

    if (post == nullptr)
    {
        return nullptr; // post não encontrado
    }

    NoPublicacao *resultado = new NoPublicacao;
    resultado->post = post;
    if (postAnterior == nullptr)
    {
        resultado->prox = nullptr; // o post encontrado é o primeiro da lista, não tem anterior
    }
    else
    {
        NoPublicacao *postAnteriorNo = new NoPublicacao;
        postAnteriorNo->post = postAnterior;
        postAnteriorNo->prox = nullptr;
        resultado->prox = postAnteriorNo;
    }

    return resultado;
}

void buscarPublicacoesPorPalavra(MiniRede &rede, const char palavra[], std::ostream &saida)
{
    saida << "SEARCH_BEGIN\n";
    //percorre a lsita de publicacoes global e usa o strstr para ver se a palavra esta dentro de um post
    Publicacao *atual = rede.listaPublicacoes;
    bool encontrou = false;

    while (atual != nullptr)
    {
        if (strstr(atual->texto, palavra) != nullptr)
        {
            saida << "POST " << atual->id << " "
                  << atual->idAutor << " "
                  << atual->timestamp << " "
                  << atual->curtidas << " "
                  << atual->texto << "\n";
            encontrou = true;
        }
        atual = atual->prox_global;
    }

    if (!encontrou)
    {
        saida << "ERROR NO_RESULTS\n";
    }

    saida << "SEARCH_END\n";
}

//***Liberar memória

void liberarListaIntNode(IntNode *inicio)
{
    while (inicio != nullptr)
    {
        IntNode *temp = inicio;
        inicio = inicio->prox;
        delete temp; 
    }
}

void liberarFilaNotificacoes(Notificacao *inicio)
{
    while (inicio != nullptr)
    {
        Notificacao *temp = inicio;
        inicio = inicio->prox;
        delete temp; 
    }
}

void liberarArvore(NoUsuarioBST *raiz)
{
    // limpa a arvore binária usando percurso pos-ordem(esq,dir,raiz)
    if (raiz == nullptr)
        return;
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    if (raiz->user != nullptr)
    {
        // deleta as listas do usuario
        liberarListaIntNode(raiz->user->seguidos);
        liberarFilaNotificacoes(raiz->user->filaNotif.inicio);

        delete raiz->user;
    }
    delete raiz;
}

void liberarTabelaHash(NoUsuarioHash *hashUsernames[])
{
    // percorre cada uma das gavetas da tabela hash
    for (int i = 0; i < TAM_HASH; i++)
    {
        NoUsuarioHash *atual = hashUsernames[i];

        // limpa a lista encadeada de cada gaveta 
        while (atual != nullptr)
        {
            NoUsuarioHash *temp = atual;
            atual = atual->prox;
            delete temp;
        }
        hashUsernames[i] = nullptr;
    }
}

void liberarPublicacoes(Publicacao *inicio)
{
    //percorre a lista de post globais usando uma temporaria para ir apagando e avançando
    while (inicio != nullptr)
    {
        Publicacao *temp = inicio;
        inicio = inicio->prox_global;             
        liberarListaIntNode(temp->listaCurtidas); // limpa a lista de curtidas do post
        delete temp;                              
    }
}

void liberarListaComentarios(Comentario *inicio)
{
    while (inicio != nullptr)
    {
        Comentario *temp = inicio;
        inicio = inicio->prox;
        delete temp;
    }
}