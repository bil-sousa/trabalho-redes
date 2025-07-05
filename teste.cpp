#include <fstream>
#include <iostream>
using namespace std;

int main() {
    const char* nome_arquivo = "queimadura_milho.jpeg";
    ifstream file(nome_arquivo, ios::binary);
    if (!file.is_open()) {
        cerr << "ERRO ao abrir a imagem!" << endl;
        return 1;
    }
    cout << "Imagem aberta com sucesso!" << endl;
    file.close();
    return 0;
}