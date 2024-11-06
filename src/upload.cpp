#include <iostream>
#include <fstream>
#include <cstring>
#include "../lib/hashing.hpp"
#include "../lib/arvorebplus.hpp"
#include "../lib/bucket.hpp"
#include <chrono>
#include <unistd.h>

using namespace std;

int main(int argc, char const *argv[]) {   
    // Criação do arquivo de dados binário onde os registros serão armazenados
    ofstream arquivo_de_dados("../data/arquivo_de_dados.bin", ios::binary | ios::out);
    if (!arquivo_de_dados) {
        cerr << "Erro ao criar o arquivo de dados!" << endl;
        return 1;  // Sai do programa caso o arquivo não seja criado
    }

    // Abertura do arquivo CSV de entrada que contém os registros para leitura
    ifstream arquivo_csv("../data/artigo.csv", ios::in);
    if(!arquivo_csv){
        cerr << "Erro ao abrir o arquivo de entrada!" << endl;
        return 1;  // Sai do programa caso o arquivo de entrada não seja aberto
    }

    // Inicializa a base de dados para o arquivo binário, criando buckets e blocos
    init(arquivo_de_dados);
    cout << "Base criada!" << endl;

    // Abertura do arquivo binário de dados para leitura após a criação
    ifstream arquivo_Dados("../data/arquivo_de_dados.bin", ios::binary | ios::in);
    if(!arquivo_Dados){
        cerr << "Erro ao abrir o arquivo de dados!" << endl;
        return 1;  // Sai do programa se o arquivo de dados não puder ser aberto
    }

    cout << "Inserindo registros no arquivo de dados..." << endl;

    // Criação de duas árvores B+, uma para o índice primário (ID) e outra para o índice secundário (Título)
    ArvoreBPlus arvore_primaria(MAX_KEYS);  // Índice primário (ID)
    ArvoreBPlus arvore_secundaria(MAX_KEYS);  // Índice secundário (Título)

    // Leitura dos registros do arquivo CSV
    if (arquivo_csv.is_open()) {
        string line;
        while (getline(arquivo_csv, line)) {  // Leitura de cada linha do CSV
            Registro* r = converter_linha_registro(line);  // Converte a linha para o formato de registro
            if(r != NULL) {  // Verifica se o registro foi convertido corretamente
                // Insere o registro no arquivo de dados e obtém o endereço do registro no arquivo
                int addr = inserir_registro_bucket(r, arquivo_Dados, arquivo_de_dados);

                // Criação dos registros de índice (RegArvore) para as chaves primária (ID) e secundária (Título)
                RegArvore* reg1 = new RegArvore(r->id, addr);  // Índice primário baseado no ID
                RegArvore* reg2 = new RegArvore(titulo_para_int(r->titulo), addr);  // Índice secundário baseado no título

                // Inserção dos índices nas árvores B+ correspondentes
                arvore_primaria.insert(reg1);
                arvore_secundaria.insert(reg2);

                // Libera a memória alocada para os registros de índice
                delete reg1;
                delete reg2;
            }
            delete r;  // Libera a memória do registro original
        }
    }
    cout << "Arquivo de dados inserido com sucesso!" << endl;

    // Serializa as árvores B+ (primária e secundária) para arquivos binários
    arvore_primaria.salvar_arvore(arvore_primaria, "../data/indice_primario.bin");
    arvore_secundaria.salvar_arvore(arvore_secundaria, "../data/indice_secundario.bin");
    cout << "Índice primário e secundário criados." << endl;

    // Desaloca a memória das árvores B+
    arvore_primaria.desalocar_arvore(arvore_primaria.get_raiz());
    arvore_secundaria.desalocar_arvore(arvore_secundaria.get_raiz());

    // Fecha os arquivos de entrada e saída
    arquivo_csv.close();  // Fecha o arquivo CSV de entrada
    arquivo_Dados.close();  // Fecha o arquivo binário de dados para leitura
    arquivo_de_dados.close();  // Fecha o arquivo binário de dados para escrita e leitura

    cout << "Done!" << endl;

    return 0;
}
