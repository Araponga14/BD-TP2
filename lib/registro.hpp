#ifndef H_REGISTRO
#define H_REGISTRO

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <string>

using namespace std;

// Definição do registro como estrutura de dados
// Cada campo representa uma propriedade do registro
struct Registro {
    int id;
    string titulo;
    int ano;
    string autores;
    int citacoes;
    string atualizacao;
    string snippet;
    int tamanho;  // Tamanho do registro em bytes
};

// Função para remover caracteres não imprimíveis de uma string
// Substitui caracteres inválidos por espaços em branco
string remove_caracteres(string str) {
    for (int i = 0; i < str.length(); i++) {
        if (!isprint(str[i])) {
            str[i] = ' ';  // substitui por um espaço em branco
        }
    }
    return str;
}

// Função para criar e inicializar um registro a partir de dados
// Recebe dados como strings, converte e atribui aos campos da estrutura Registro
Registro* criar_registro(string id, string titulo, string ano, string autores, string citacoes, string atualizacao, string snippet) {
    Registro* registro = new Registro();
    registro->id = stoi(id);  // Converte string para int
    registro->titulo = titulo;
    registro->ano = stoi(ano);
    registro->autores = autores;
    registro->citacoes = stoi(citacoes);
    registro->atualizacao = atualizacao;
    registro->snippet = snippet;
    // Calcula o tamanho total do registro, somando tamanhos dos campos de string e de inteiros
    registro->tamanho = registro->titulo.size() + registro->autores.size() + registro->atualizacao.size() + registro->snippet.size() + 4 * sizeof(int) + 4;
    return registro;
}

// Função para converter uma linha de entrada em um registro
// Lê cada campo, trata o caso de valores "NULL" e usa aspas como delimitadores
Registro* converter_linha_registro(string entry_linha) {
    vector<string> registro;  // Armazena os campos do registro temporariamente
    string campo = "";

    // Remove caracteres não imprimíveis antes de iniciar o parsing
    string linha = remove_caracteres(entry_linha);

    // Loop para separar cada campo da linha de entrada
    for (int i = 0; i < linha.size() - 1; i++) {
        if (linha[i] == '"') {  // Início de um campo com aspas
            i++;
            // Lê até o final do campo (identificado por aspas e ponto e vírgula)
            while (!(linha[i] == '"' && linha[i + 1] == ';') && i < linha.size() - 2) {
                campo += linha[i];
                i++;
            }
            registro.push_back(campo);
            campo = "";
        } else if (linha[i] == 'N' || (linha[i] == ';' && linha[i + 1] == ';')) {
            // Tratamento para campos "NULL" (vazio)
            registro.push_back("NULL");
        }
    }

    // Verifica se a linha contém o número correto de campos antes de criar o registro
    if (registro.size() != 7) {
        return NULL;
    }
    try {
        // Cria o registro usando os dados processados
        return criar_registro(registro[0], registro[1], registro[2], registro[3], registro[4], registro[5], registro[6]);
    } catch (const std::exception& e) {
        return NULL;  // Retorna NULL em caso de erro de conversão
    }
}

// Função para imprimir os dados de um registro formatados no console
void imprimir_registro(Registro registro) {
    cout << "\tID: " << registro.id << endl;
    cout << "\tTitulo: " << registro.titulo << endl;
    cout << "\tAno: " << registro.ano << endl;
    cout << "\tAutores: " << registro.autores << endl;
    cout << "\tCitacoes: " << registro.citacoes << endl;
    cout << "\tAtualizacao: " << registro.atualizacao << endl;
    cout << "\tSnippet: " << registro.snippet << endl;
    cout << "\tTamanho do registro: " << registro.tamanho << " bytes" << endl;
}

#endif
