#ifndef H_HASHING
#define H_HASHING

#include "bucket.hpp"
#define NUM_BUCKETS 15000

using namespace std;

// Função para calcular o índice de hash a partir de um ID
// Multiplica o ID por uma constante e aplica módulo com o número total de buckets.
int funcao_hash(int id) {
    int index = (67 * id) % NUM_BUCKETS;
    return index;
}

// Função que inicializa a tabela hash criando todos os buckets vazios
// Escreve cada bucket inicializado no arquivo de dados.
void init(ofstream& dataFile) {
    for (int i = 0; i < NUM_BUCKETS; i++) {
        criarBucket(dataFile);
    }
}

// Função para inserir um registro em um bloco específico da tabela hash
// Essa função copia cada campo do registro para o bloco e atualiza o cabeçalho.
void inserir_registro_bloco(ifstream& leitura, ofstream& escrita, Bloco* bloco, Registro* registro, int ultimo_bloco, int index_bucket) {

    int posicao = bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros];
    memcpy(&bloco->dados[posicao], &registro->id, sizeof(int));
    posicao += sizeof(int);

    memcpy(&bloco->dados[posicao], registro->titulo.c_str(), registro->titulo.size() + 1);
    posicao += registro->titulo.size() + 1;

    memcpy(&bloco->dados[posicao], &registro->ano, sizeof(int));
    posicao += sizeof(int);

    memcpy(&bloco->dados[posicao], registro->autores.c_str(), registro->autores.size() + 1);
    posicao += registro->autores.size() + 1;

    memcpy(&bloco->dados[posicao], &registro->citacoes, sizeof(int));
    posicao += sizeof(int);

    memcpy(&bloco->dados[posicao], registro->atualizacao.c_str(), registro->atualizacao.size() + 1);
    posicao += registro->atualizacao.size() + 1;

    memcpy(&bloco->dados[posicao], registro->snippet.c_str(), registro->snippet.size() + 1);
    posicao += registro->snippet.size() + 1;

    bloco->cabecalho->num_registros++;
    bloco->cabecalho->espaco_livre -= registro->tamanho;
    bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros] = posicao;

    char buffer[TAM_BLOCO];
    memcpy(buffer, bloco->cabecalho, sizeof(BlocoCabecalho));
    memcpy(buffer + sizeof(BlocoCabecalho), bloco->dados, TAM_BLOCO - sizeof(BlocoCabecalho));

    escrita.seekp(index_bucket * TAM_BLOCO * NUM_BLOCOS + (ultimo_bloco * TAM_BLOCO));
    escrita.write(reinterpret_cast<char*>(buffer), TAM_BLOCO);
}

// Função para inserir um registro no bucket correto e retornar o endereço no arquivo
// Caso não haja espaço no bloco atual, tenta no próximo.
int inserir_registro_bucket(Registro *registro, ifstream &entrada, ofstream &saida) {   
    int indice_bucket = funcao_hash(registro->id);
    int ultimo_bloco = 0; 
    int inicio_bucket = indice_bucket * TAM_BLOCO * NUM_BLOCOS; 
    entrada.seekg(inicio_bucket);

    for (int i = 0; i < NUM_BLOCOS; i++) {
        Bloco* bloco = new Bloco();
        bloco->cabecalho = new BlocoCabecalho();
        entrada.read(reinterpret_cast<char*>(bloco->cabecalho), sizeof(BlocoCabecalho));
        entrada.read(reinterpret_cast<char*>(bloco->dados), TAM_BLOCO - sizeof(BlocoCabecalho));

        int tam = bloco->cabecalho->espaco_livre;
        if (tam >= registro->tamanho) {   
            int addr = inicio_bucket + (ultimo_bloco * TAM_BLOCO) + sizeof(BlocoCabecalho) +
                       bloco->cabecalho->pos_registros[bloco->cabecalho->num_registros];

            inserir_registro_bloco(entrada, saida, bloco, registro, ultimo_bloco, indice_bucket);
            desalocar_bloco(bloco); 
            return addr;
        } else {
            ultimo_bloco++;
            desalocar_bloco(bloco);
        }
        if(i + 1 >= NUM_BLOCOS){
            cout << "Erro: Não há espaço disponível para inserir o registro" << endl;
            cout << "Registros inseridos: " << registro->id -1 << endl;
            cout << "Não foi possivel gerar os arquivos de indice" << endl;
            exit(1);
        }
    }
    return -1;
}

// Função para buscar um registro específico no arquivo de dados
// Busca o registro no bloco apropriado com base no índice hash e no ID.
Registro* buscar_registro(ifstream& leitura, int id_busca) {
    for (int ultimo_bloco = 0; ultimo_bloco < NUM_BLOCOS; ultimo_bloco++) {
        Bloco* bloco = criar_bloco();
        int index_bucket = funcao_hash(id_busca);
        leitura.seekg(index_bucket * TAM_BLOCO * NUM_BLOCOS + (ultimo_bloco * TAM_BLOCO));
        leitura.read(reinterpret_cast<char*>(bloco->cabecalho), sizeof(BlocoCabecalho));
        leitura.read(reinterpret_cast<char*>(bloco->dados), TAM_BLOCO - sizeof(BlocoCabecalho));

        if (bloco->cabecalho->num_registros > 0) {
            Registro* registro = new Registro();
            for (int i = 0; i < bloco->cabecalho->num_registros; i++) {
                int posicao = bloco->cabecalho->pos_registros[i];

                memcpy(&registro->id, &bloco->dados[posicao], sizeof(int));
                if(registro->id == id_busca) {
                    posicao += sizeof(int);
                    registro->titulo = string((char *)&bloco->dados[posicao]);
                    posicao += registro->titulo.size() + 1;

                    memcpy(&registro->ano, &bloco->dados[posicao], sizeof(int));
                    posicao += sizeof(int);

                    registro->autores = string((char *)&bloco->dados[posicao]);
                    posicao += registro->autores.size() + 1;

                    memcpy(&registro->citacoes, &bloco->dados[posicao], sizeof(int));
                    posicao += sizeof(int);

                    registro->atualizacao = string((char *)&bloco->dados[posicao]);
                    posicao += registro->atualizacao.size() + 1;

                    registro->snippet = string((char *)&bloco->dados[posicao]);
                    posicao += registro->snippet.size() + 1;

                    registro->tamanho = registro->titulo.size() + sizeof(int) + registro->autores.size() +
                                        sizeof(int) + sizeof(int) + registro->atualizacao.size() + 
                                        registro->snippet.size() + 4;

                    cout << "\\nQuantidade de blocos lidos para encontrar o registro: " << ultimo_bloco + 1 << endl;
                    cout << "Total de blocos no arquivo de dados: " << NUM_BLOCOS * NUM_BUCKETS << endl;

                    desalocar_bloco(bloco);
                    return registro;
                }
            }
        }
        desalocar_bloco(bloco);
    }

    return nullptr;
}

#endif // H_HASHING
