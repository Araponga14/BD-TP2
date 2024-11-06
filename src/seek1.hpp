#include "../lib/arvorebplus.hpp"

void seek1(int id_busca) {
    // Abertura do arquivo de dados organizado por hashing
    ifstream arquivo_Dados("../data/arquivo_de_dados.bin", ios::binary | ios::in);
    string indice_primario = "../data/indice_primario.bin";

    // Busca pelo registro com o ID especificado
    Registro* registro_busca = ABP_buscar_registro(indice_primario, arquivo_Dados, id_busca);

    if (registro_busca != nullptr) {
        cout << "\nRegistro de ID " << id_busca << ":" << endl;
        imprimir_registro(*registro_busca);
        cout << endl;


        // Libera a memória alocada para o registro
        delete registro_busca;
    } else {

        cout << "Registro " << id_busca << " não encontrado!" << endl;

    }

    // Fechamento do arquivo de dados organizado por hashing
    arquivo_Dados.close();
}
