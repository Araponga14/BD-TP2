#ifndef H_ABP
#define H_ABP

#include "registro.hpp"
#define MAX_KEYS 510

using namespace std;

// Estrutura para armazenar registros de chave-valor na árvore
struct RegArvore {
    int chave;  // chave do registro
    int valor;  // valor associado, usado como offset para localização de dados

    // Construtor para inicializar chave e valor
    RegArvore(int chave, int valor) : chave(chave), valor(valor) {}
    RegArvore() : chave(0), valor(0) {}
};

// Template para estrutura do nó da árvore B+
template <typename T>
struct No {
    size_t grau;                // Grau da árvore B+
    size_t quant_elementos;     // Quantidade de elementos no nó
    RegArvore* item;            // Array de RegArvore (chave-valor) para armazenar dados no nó
    No<RegArvore>** filhos;     // Ponteiros para filhos do nó
    No<RegArvore>* pai;         // Ponteiro para o nó pai
    bool folha;                 // Indica se o nó é uma folha (sem filhos)

public:
    // Construtor do nó, inicializa arrays de itens e filhos
    No(size_t _grau) {
        this->grau = _grau;
        this->quant_elementos = 0;

        RegArvore* item_ = new RegArvore[grau-1];
        for(int i=0; i<grau-1; i++){
            item_[i] = RegArvore(0,0);
        }
        this->item = item_;

        No<RegArvore>** filhos_ = new No<RegArvore>*[grau];
        for(int i=0; i<grau; i++){
            filhos_[i] = nullptr;
        }
        this->filhos = filhos_;
        this->folha = false;

        this->pai = nullptr;
    }
};

// classe principal da arvore B+
class ArvoreBPlus {
    
public:
    No<RegArvore>* raiz;
    size_t grau;

public:
    ArvoreBPlus(size_t _grau) {
        this->raiz = nullptr;
        this->grau = _grau;
    }

    void desalocar_arvore(No<RegArvore>* node) {
        if (node == nullptr) {
            return;
        }

        if (!node->folha) {
            for (size_t i = 0; i < node->quant_elementos + 1; i++) {
                desalocar_arvore(node->filhos[i]);
            }
        }

        delete[] node->item;
        delete[] node->filhos;
        delete node;
    }

    No<RegArvore>* get_raiz(){
        return this->raiz;
    }

    No<RegArvore>* ABP_busca(No<RegArvore>* node, RegArvore key){
        if(node == nullptr) {
            return nullptr;
        }
        else{
            No<RegArvore>* cursor = node;

            int altura = 1;
            while(!cursor->folha){
                for(int i=0; i<cursor->quant_elementos; i++){ 
                    if(key.chave < cursor->item[i].chave){
                        cursor = cursor->filhos[i];
                        break;
                    }
                    if(i == (cursor->quant_elementos)-1){
                        cursor = cursor->filhos[i+1];
                        break;
                    }
                }
                altura++;
            }

            for(int i=0; i<cursor->quant_elementos; i++){
                if(cursor->item[i].chave == key.chave){
                    cout << "\n\nQuantidade de blocos lidos para encontrar o registro no arquivo de índice: " << altura + 1 << endl;
                    return cursor;
                }
            }
            return nullptr;
        }
    }

    No<RegArvore>* ABP_range_search(No<RegArvore>* node, RegArvore key){
    if(node == nullptr) {
        return nullptr;
    }
    else{
        No<RegArvore>* cursor = node; 

        while(!cursor->folha){
            for(int i=0; i<cursor->quant_elementos; i++){ 
                if(key.chave < cursor->item[i].chave){
                    cursor = cursor->filhos[i];
                    break;
                }
                if(i == (cursor->quant_elementos)-1){
                    cursor = cursor->filhos[i+1];
                    break;
                }
            }
        }
        return cursor;
    }
}

    // Função para buscar um registro em uma árvore B+
    int range_search(int start, int end, RegArvore* result_data, int arr_length) {
        int index=0;

        No<RegArvore>* start_node = ABP_range_search(this->raiz, RegArvore(start, 0));
        No<RegArvore>* cursor = start_node;
        RegArvore temp = cursor->item[0];

        while(temp.chave<=end){
            if(cursor == nullptr){
                break;
            }
            for(int i=0; i< cursor->quant_elementos;i++){
                temp = cursor->item[i];
                if((temp.chave >= start)&&(temp.chave <= end)){
                    result_data[index] = temp;
                    index++;
                }
            }
            cursor = cursor->filhos[cursor->quant_elementos];
        }
        return index;
    }

    No<RegArvore>* search(int chave) {  // função para procurar uma chave
        
        return ABP_busca(this->raiz, RegArvore(chave, 0)) ;
    }

    int buscar_indice(RegArvore* arr, RegArvore data, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data.chave < arr[i].chave){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }
        return index;
    }

    // Função para inserir um item em um nó
    RegArvore* inserir_item(RegArvore* arr, RegArvore data, int len){
        int index = 0;
        for(int i=0; i<len; i++){
            if(data.chave < arr[i].chave){
                index = i;
                break;
            }
            if(i==len-1){
                index = len;
                break;
            }
        }

        for(int i = len; i > index; i--){
            arr[i] = arr[i-1];
        }

        arr[index] = data;

        return arr;
    }

    // Função para inserir um filho em um nó
    No<RegArvore>** child_insert(No<RegArvore>** child_arr, No<RegArvore>*child,int len,int index){
        for(int i= len; i > index; i--){
            child_arr[i] = child_arr[i - 1];
        }
        child_arr[index] = child;
        return child_arr;
    }

    // Função para inserir um item e um filho em um nó
    No<RegArvore>* child_item_insert(No<RegArvore>* node, RegArvore data, No<RegArvore>* child){
        int item_index=0;
        int child_index=0;
        for(int i=0; i< node->quant_elementos; i++){
            if(data.chave < node->item[i].chave){
                item_index = i;
                child_index = i+1;
                break;
            }
            if(i==node->quant_elementos-1){
                item_index = node->quant_elementos;
                child_index = node->quant_elementos+1;
                break;
            }
        }
        for(int i = node->quant_elementos;i > item_index; i--){
            node->item[i] = node->item[i-1];
        }
        for(int i=node->quant_elementos+1;i>child_index;i--){
            node->filhos[i] = node->filhos[i-1];
        }

        node->item[item_index] = data;
        node->filhos[child_index] = child;

        return node;
    }

    // Função para inserir um item em um nó folha
    void InsertPar(No<RegArvore>* par, No<RegArvore>* child, RegArvore data) {
        // checar overflow
        No<RegArvore>* cursor = par;
        if (cursor->quant_elementos < this->grau - 1) { // sem overflow, apenas insira na posição correta
            // insere item e rearranja
            cursor = child_item_insert(cursor, data, child);
            cursor->quant_elementos++;
        }
        else { // overflow
            // cria novo nó
            auto* novo_no = new No<RegArvore>(this->grau);
            novo_no->pai = cursor->pai;

            // copia item
            RegArvore* item_copy = new RegArvore[cursor->quant_elementos + 1];
            for (int i = 0; i < cursor->quant_elementos; i++) {
                item_copy[i] = cursor->item[i];
            }
            item_copy = inserir_item(item_copy, data, cursor->quant_elementos);

            auto** child_copy = new No<RegArvore>*[cursor->quant_elementos + 2];
            for (int i = 0; i < cursor->quant_elementos + 1; i++) {
                child_copy[i] = cursor->filhos[i];
            }
            child_copy[cursor->quant_elementos + 1] = nullptr;
            child_copy = child_insert(child_copy, child, cursor->quant_elementos + 1, buscar_indice(item_copy, data, cursor->quant_elementos + 1));

            // divide nós
            cursor->quant_elementos = (this->grau) / 2;
            if ((this->grau) % 2 == 0) {
                novo_no->quant_elementos = (this->grau) / 2 - 1;
            }
            else {
                novo_no->quant_elementos = (this->grau) / 2;
            }

            for (int i = 0; i < cursor->quant_elementos; i++) {
                cursor->item[i] = item_copy[i];
                cursor->filhos[i] = child_copy[i];
            }
            cursor->filhos[cursor->quant_elementos] = child_copy[cursor->quant_elementos];

            for (int i = 0; i < novo_no->quant_elementos; i++) {
                novo_no->item[i] = item_copy[cursor->quant_elementos + i + 1];
                novo_no->filhos[i] = child_copy[cursor->quant_elementos + i + 1];
                novo_no->filhos[i]->pai = novo_no;
            }
            novo_no->filhos[novo_no->quant_elementos] = child_copy[cursor->quant_elementos + novo_no->quant_elementos + 1];
            novo_no->filhos[novo_no->quant_elementos]->pai = novo_no;

            RegArvore paritem = item_copy[this->grau / 2];

            delete[] item_copy;
            delete[] child_copy;

            // checa se o nó é raiz
            if (cursor->pai == nullptr) { // se não houver nó pai (caso raiz)
                auto* Newpai = new No<RegArvore>(this->grau);
                cursor->pai = Newpai;
                novo_no->pai = Newpai;

                Newpai->item[0] = paritem;
                Newpai->quant_elementos++;

                Newpai->filhos[0] = cursor;
                Newpai->filhos[1] = novo_no;

                this->raiz = Newpai;
            }
            else { // se já houver nó pai
                InsertPar(cursor->pai, novo_no, paritem);
            }
        }
    }

    // Função para inserir um item em um nó folha
    void insert(RegArvore* data) {
        RegArvore reg(data->chave, data->valor);

        if (this->raiz == nullptr) { // se a árvore estiver vazia
            this->raiz = new No<RegArvore>(this->grau);
            this->raiz->folha = true;
            this->raiz->item[0] = *data;
            this->raiz->quant_elementos = 1;
        } else { // se a árvore não estiver vazia
            No<RegArvore>* cursor = this->raiz;

            // encontre o nó folha para inserir a chave
            cursor = ABP_range_search(cursor, reg);

            // checa overflow
            if (cursor->quant_elementos < (this->grau - 1)) { // sem overflow, apenas insira na posição correta
                // insere item e rearranja
                cursor->item = inserir_item(cursor->item, *data, cursor->quant_elementos);
                cursor->quant_elementos++;
                // edita ponteiro de próximo nó
                cursor->filhos[cursor->quant_elementos] = cursor->filhos[cursor->quant_elementos - 1];
                cursor->filhos[cursor->quant_elementos - 1] = nullptr;
            } else { //overflow 
                // cria novo nó
                auto* novo_no = new No<RegArvore>(this->grau);
                novo_no->folha = true;
                novo_no->pai = cursor->pai;

                //  copia item
                RegArvore* item_copy = new RegArvore[cursor->quant_elementos + 1];
                for (int i = 0; i < cursor->quant_elementos; i++) {
                    item_copy[i] = cursor->item[i];
                }

                // insere item
                item_copy = inserir_item(item_copy, *data, cursor->quant_elementos);

                // divide nós
                cursor->quant_elementos = (this->grau) / 2;
                if ((this->grau) % 2 == 0) {
                    novo_no->quant_elementos = (this->grau) / 2;
                } else {
                    novo_no->quant_elementos = (this->grau) / 2 + 1;
                }

                for (int i = 0; i < cursor->quant_elementos; i++) {
                    cursor->item[i] = item_copy[i];
                }
                for (int i = 0; i < novo_no->quant_elementos; i++) {
                    novo_no->item[i] = item_copy[cursor->quant_elementos + i];
                }

                cursor->filhos[cursor->quant_elementos] = novo_no;
                novo_no->filhos[novo_no->quant_elementos] = cursor->filhos[this->grau - 1];
                cursor->filhos[this->grau - 1] = nullptr;

                delete[] item_copy;

                // checa se o nó é raiz
                RegArvore paritem = novo_no->item[0];

                if (cursor->pai == nullptr) { // se não houver nó pai (caso raiz)
                    auto* Newpai = new No<RegArvore>(this->grau);
                    cursor->pai = Newpai;
                    novo_no->pai = Newpai;

                    Newpai->item[0] = paritem;
                    Newpai->quant_elementos++;

                    Newpai->filhos[0] = cursor;
                    Newpai->filhos[1] = novo_no;

                    this->raiz = Newpai;
                } else { // se já houver nó pai
                    InsertPar(cursor->pai, novo_no, paritem);
                }
            }
        }
    }

    // Função para serializar uma árvore B+ em um arquivo binário
    void salvar_arvore(const ArvoreBPlus& tree, const string& filename) {
        ofstream file(filename, ios::binary | ios::out);
        if (!file) {
            cerr << "Error opening file for serialization: " << filename << endl;
            return;
        }

        // Escrever o grau da árvore no arquivo
        size_t grau = tree.grau;
        file.write(reinterpret_cast<char*>(&grau), sizeof(grau));

        // Serializar a árvore recursivamente, começando pelo nó raiz
        salvar_no(file, tree.raiz);

        file.close();
    }

    // Função recursiva para serializar um nó e seus filhos
    void salvar_no(ofstream& file, const No<RegArvore>* node) {
        // Escrever as informações do nó no arquivo (folha e quant_elementos)
        bool folha = node->folha;
        size_t quant_elementos = node->quant_elementos;
        file.write(reinterpret_cast<const char*>(&folha), sizeof(folha));
        file.write(reinterpret_cast<const char*>(&quant_elementos), sizeof(quant_elementos));

        // Escrever os itens do nó no arquivo (registros)
        file.write(reinterpret_cast<const char*>(node->item), sizeof(RegArvore) * (node->grau - 1));

        if (!folha) {
            // Serializar os nós filhos recursivamente
            for (size_t i = 0; i <= quant_elementos; ++i) {
                salvar_no(file, node->filhos[i]);
            }
        }
    }

    // Função para desserializar uma árvore B+ de um arquivo binário
    ArvoreBPlus carregar_arvore(const string& filename) {
        ifstream file(filename, ios::binary | ios::in);
        if (!file) {
            cerr << "Error opening file for deserialization: " << filename << endl;
            return ArvoreBPlus(0);  // Retornar uma árvore B+ vazia
        }

        // Ler o grau da árvore do arquivo
        size_t grau;
        if (!file.read(reinterpret_cast<char*>(&grau), sizeof(grau))) {
            cerr << "Error reading grau from file: " << filename << endl;
            file.close();
            return ArvoreBPlus(0);  // Retornar uma árvore B+ vazia
        }

        // Criar uma nova árvore B+ com o grau fornecido
        ArvoreBPlus tree(grau);

        // Desserializar a árvore recursivamente, começando pelo nó raiz
        tree.raiz = carregar_no(file, nullptr, grau);
        if (!tree.raiz) {
            cerr << "Error deserializing raiz node from file: " << filename << endl;
            file.close();
            return ArvoreBPlus(0);  // Retornar uma árvore B+ vazia
        }

        file.close();

        return tree;
    }

    // Função para destruir um nó e seus filhos recursivamente
    void desalocar_no(No<RegArvore>* node) {
        if (node) {
            if (!node->folha) {
                for (size_t i = 0; i <= node->quant_elementos; ++i) {
                    desalocar_no(node->filhos[i]);
                }
                delete[] node->filhos;
            }
            delete[] node->item;
            delete node;
        }
    }

    // Função recursiva para desserializar um nó e seus filhos
    No<RegArvore>* carregar_no(ifstream& file, No<RegArvore>* pai, size_t grau) {
        // Ler as informações do nó do arquivo
        bool folha;
        size_t quant_elementos;
        if (!file.read(reinterpret_cast<char*>(&folha), sizeof(folha)) ||
            !file.read(reinterpret_cast<char*>(&quant_elementos), sizeof(quant_elementos))) {
            cerr << "Error reading node information from file." << endl;
            return nullptr;
        }

        // Criar um novo nó
        auto* node = new No<RegArvore>(grau);
        node->folha = folha;
        node->quant_elementos = quant_elementos;
        node->pai = pai;

        // Ler os itens do nó do arquivo
        node->item = new RegArvore[grau - 1];
        if (!file.read(reinterpret_cast<char*>(node->item), sizeof(RegArvore) * (grau - 1))) {
            cerr << "Error reading node items from file." << endl;
            delete[] node->item;
            delete node;
            return nullptr;
        }

        if (!folha) {
            // Desserializar os nós filhos recursivamente
            node->filhos = new No<RegArvore>*[grau];
            for (size_t i = 0; i <= quant_elementos; ++i) {
                node->filhos[i] = carregar_no(file, node, grau);
                if (!node->filhos[i]) {
                    cerr << "Error deserializing child node from file." << endl;
                    desalocar_no(node); // Liberar a memória alocada
                    return nullptr;
                }
            }
        }

        return node;
    }

};


// Função para contar o número de nós de uma árvore B+
int contar_nos(No<RegArvore>* node) {
    if (node == nullptr) {
        return 0;
    }

    int count = 1; // conta o próprio nó

    if (!node->folha) {
        for (size_t i = 0; i <= node->quant_elementos; i++) {
            count += contar_nos(node->filhos[i]); // conta os nós filhos recursivamente
        }
    }

    return count;
}

// Função para buscar um registro em uma árvore B+ e retornar o registro (ou nullptr se não encontrado)
Registro* ABP_buscar_registro(string index_filename, ifstream& dataFile, int id_busca) {
    ArvoreBPlus bpt = bpt.carregar_arvore(index_filename);
    No<RegArvore>* node = bpt.search(id_busca);
    Registro* registro = nullptr;

    if (node != nullptr) {
        RegArvore* reg = nullptr;

        // Busca o registro no nó
        for (int i = 0; i < node->quant_elementos; i++) {
            // Se encontrou o registro
            if (node->item[i].chave == id_busca) {
                reg = &node->item[i];
                break;
            }
        }

        // Se não encontrou o registro
        if (reg == nullptr) {
            bpt.desalocar_arvore(bpt.get_raiz()); // Libera a memória alocada para a árvore B+ (e seus nós)
            return nullptr;
        }

        registro = new Registro();
        dataFile.seekg(reg->valor);
        dataFile.read(reinterpret_cast<char*>(&registro->id), sizeof(int));

        // Deserializa os demais campos do registro
        getline(dataFile, registro->titulo, '\0');
        dataFile.read(reinterpret_cast<char*>(&registro->ano), sizeof(int));
        getline(dataFile, registro->autores, '\0');
        dataFile.read(reinterpret_cast<char*>(&registro->citacoes), sizeof(int));
        getline(dataFile, registro->atualizacao, '\0');
        getline(dataFile, registro->snippet, '\0');

        registro->tamanho = sizeof(int) + registro->titulo.size() + 1 +
                            sizeof(int) + registro->autores.size() + 1 +
                            sizeof(int) + registro->atualizacao.size() + 1 +
                            registro->snippet.size() + 1;

        int totalNos = contar_nos(bpt.get_raiz());
        cout << "Quantidade total de blocos do arquivo de índice primário: " << totalNos << endl;

        // Não é necessário liberar a memória para a árvore B+ aqui, pois ela será desalocada mais tarde

    } else {
        cout << "Node não encontrado para a chave: " << id_busca << endl;
    }

    delete node; // Libera a memória alocada para o nó
    return registro; // Retorna o registro encontrado ou nullptr se não encontrado
}


//Função para converter o título em um inteiro
int titulo_para_int(string titulo) {
    int chave = 0;
    int g = 31;
    int tam = titulo.size();

    for (int i = 0; i < tam; i++)
        chave = g * chave + (int)titulo[i];

    if (chave < 0)
        return (chave * -1) + titulo.size();
    else
        return chave + titulo.size();
}

#endif