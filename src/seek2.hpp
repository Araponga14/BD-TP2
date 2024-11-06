#include "../lib/arvorebplus.hpp"

void seek2(const string& titulo) {
    // Abertura do arquivo de dados organizado por hashing
    ifstream arquivo_Dados("../data/arquivo_de_dados.bin", ios::binary | ios::in);
    string indice_secundario = "../data/indice_secundario.bin";

    // Busca pelo registro com o título especificado
    Registro* registro_busca = ABP_buscar_registro(indice_secundario, arquivo_Dados, titulo_para_int(remove_caracteres(titulo)));

    if(registro_busca != nullptr) {

        cout << "\nRegistro com título " << titulo << " encontrado! " << endl;
        cout << "Campos do Registro:" << endl;
        imprimir_registro(*registro_busca);
        cout << endl;

    } else {

        cout << "Registro com título " << titulo << " não encontrado!" << endl;

    }

    delete registro_busca;

    arquivo_Dados.close();
}