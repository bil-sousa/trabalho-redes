#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET sock;
    sockaddr_in servidor_addr;

    // Caminho absoluto para a imagem que quer enviar
    const char* nome_arquivo = "queimadura_milho.jpeg";

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cerr << "Erro ao iniciar Winsock: " << WSAGetLastError() << endl;
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cerr << "Erro ao criar socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(12345);
    servidor_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&servidor_addr, sizeof(servidor_addr)) < 0) {
        cerr << "Erro ao conectar: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    ifstream file(nome_arquivo, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Erro ao abrir imagem!" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    streamsize tamanho = file.tellg();
    file.seekg(0, ios::beg);
    vector<char> buffer(tamanho);

    if (!file.read(buffer.data(), tamanho)) {
        cerr << "Erro ao ler imagem!" << endl;
        file.close();
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    file.close();

    uint32_t tamanho_net = htonl(static_cast<uint32_t>(tamanho));
    send(sock, reinterpret_cast<char*>(&tamanho_net), sizeof(tamanho_net), 0);

    int total_enviado = 0;
    while (total_enviado < tamanho) {
        int enviado = send(sock, buffer.data() + total_enviado, tamanho - total_enviado, 0);
        if (enviado <= 0) {
            cerr << "Erro ao enviar imagem!" << endl;
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        total_enviado += enviado;
    }

    cout << "Imagem enviada com sucesso!" << endl;

    char resposta[1024];
    int recebidos = recv(sock, resposta, sizeof(resposta) - 1, 0);
    if (recebidos > 0) {
        resposta[recebidos] = '\0';
        cout << "Classificação: " << resposta << endl;
    } else {
        cerr << "Erro ao receber resposta." << endl;
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
