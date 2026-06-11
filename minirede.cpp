#include "minirede.h"

void inicializarMiniRede(MiniRede& rede) {
    // TODO
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
    // TODO
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

