#ifndef H_BLOCO
#define H_BLOCO

#include "registro.hpp"
#define TAM_BLOCO 4096

using namespace std;

// definição do cabeçalho do bloco
struct BlocoCabecalho {
    int num_registros;          // Contador de registros no bloco.
    int espaco_livre;            // Espaço livre restante no bloco.
    int pos_registros[18];       // Array de posições de registros, para localizar onde cada registro começa no bloco.
};

// definição do bloco
struct Bloco {
    BlocoCabecalho* cabecalho;   // Ponteiro para o cabeçalho do bloco.
    unsigned char dados[TAM_BLOCO]; // Array de bytes que representa os dados do bloco.
};

// função para criar um Cabeçalho
BlocoCabecalho* criar_cabecalho() {
    BlocoCabecalho* cabecalho = new BlocoCabecalho();
    cabecalho->num_registros = 0; // Inicializa o número de registros como zero.
    for (int i = 0; i < 18; i++) {
        cabecalho->pos_registros[i] = 0; // Inicializa todas as posições de registros com zero.
    }
    cabecalho->espaco_livre = TAM_BLOCO - sizeof(int) * 2 - sizeof(int) * 18; // Calcula o espaço livre inicial no bloco.

    return cabecalho;
}

// função para criar um bloco
Bloco* criar_bloco() {
    Bloco* bloco = new Bloco();
    bloco->cabecalho = criar_cabecalho(); // Cria e atribui um cabeçalho ao bloco.
    for(int i = 0; i < TAM_BLOCO; i++) {
        bloco->dados[i] = 0; // Inicializa o array de dados do bloco com zeros.
    }
    return bloco;
}

// função para desalocar um bloco
void desalocar_bloco(Bloco* bloco) {
    delete bloco->cabecalho; // Libera a memória alocada para o cabeçalho.
    delete bloco;             // Libera a memória alocada para o bloco.
}

#endif
