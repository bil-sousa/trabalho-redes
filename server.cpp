#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET servidor, cliente;
    sockaddr_in servidor_addr, cliente_addr;
    int cliente_len = sizeof(cliente_addr);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Erro ao iniciar Winsock: " << WSAGetLastError() << endl;
        return 1;
    }

    servidor = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor == INVALID_SOCKET) {
        cerr << "Erro ao criar socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_addr.s_addr = INADDR_ANY;
    servidor_addr.sin_port = htons(12345);

    if (bind(servidor, (sockaddr*)&servidor_addr, sizeof(servidor_addr)) < 0) {
        cerr << "Erro no bind: " << WSAGetLastError() << endl;
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    listen(servidor, 1);
    cout << "Servidor aguardando conexão...\n";

    cliente = accept(servidor, (sockaddr*)&cliente_addr, &cliente_len);
    if (cliente == INVALID_SOCKET) {
        cerr << "Erro no accept: " << WSAGetLastError() << endl;
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    uint32_t tamanho_net;
    int recebidos = recv(cliente, reinterpret_cast<char*>(&tamanho_net), sizeof(tamanho_net), 0);
    if (recebidos != sizeof(tamanho_net)) {
        cerr << "Erro ao receber o tamanho da imagem.\n";
        closesocket(cliente);
        closesocket(servidor);
        WSACleanup();
        return 1;
    }
    uint32_t tamanho = ntohl(tamanho_net);
    cout << "Tamanho da imagem recebido: " << tamanho << " bytes\n";

    vector<char> buffer(tamanho);
    int total_recebido = 0;
    while (total_recebido < tamanho) {
        int r = recv(cliente, buffer.data() + total_recebido, tamanho - total_recebido, 0);
        if (r <= 0) {
            cerr << "Erro ao receber dados da imagem.\n";
            closesocket(cliente);
            closesocket(servidor);
            WSACleanup();
            return 1;
        }
        total_recebido += r;
    }

    ofstream out("recebida.jpg", ios::binary);
    out.write(buffer.data(), buffer.size());
    out.close();
    cout << "Imagem salva como recebida.jpg\n";

    int result = system("python C:\\Users\\sousa\\OneDrive\\Área de Trabalho\\redes_computadores\\test_c++\\output\\classificador.py recebida.jpg");


    if (result != 0) {
        cerr << "Erro ao executar script Python.\n";
        closesocket(cliente);
        closesocket(servidor);
        WSACleanup();
        return 1;
    }

    ifstream resultado("resultado.txt");
    string linha, resposta;
    while (getline(resultado, linha)) {
        resposta += linha + "\n";
    }
    resultado.close();

    send(cliente, resposta.c_str(), resposta.size(), 0);
    cout << "Resultado enviado ao cliente.\n";

    closesocket(cliente);
    closesocket(servidor);
    WSACleanup();
    return 0;
}