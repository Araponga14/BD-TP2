#include <limits>
#include "findrec.hpp"
#include "seek1.hpp"
#include "seek2.hpp"

using namespace std;

int main() {
    int op;
    cout << "Escolha uma das opções de consulta:" << endl;
    cout << "(1) seek1;" << endl << "(2) seek2;" << endl << "(3) findrec;" << endl << "(-1) sair." << endl << "Digite a opção: ";
    cin >> op;
    cout << endl;
    while (op != -1) {
        if (op == 1) {
            int id;
            cout << "Seek1 - digite o ID a ser buscado: ";
            cin >> id;
            seek1(id);
        } else if (op == 2) {
            string titulo;
            cout << "Seek2 - digite o título a ser buscado: ";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            getline(cin, titulo);
            seek2(titulo);
        } else if (op == 3) {
            int id;
            cout << "Findrec - digite o ID a ser buscado: ";
            cin >> id;
            findrec(id);
        } else if (op == -1){
            break;
        }
        cout <<  "Digite a opção: ";
        cin >> op;
        cout << endl;
    }

    return 0;
}