#include "aspirador_client.h"

using namespace std;                  // Permite usar o cout e endl diretamente ao invés de std::cout

namespace devtitans::aspirador {      // Entra no pacote devtitans::hello

void AspiradorClient::start(int argc, char **argv) {
    cout << "Cliente Aspirador!" << endl;

    if (argc < 2) {
        cout << "Sintaxe: " << argv[0] << "  " << endl;
        cout << "    Comandos: get-status" << endl;
        exit(1);
    }

    Aspirador aspirador;             // Classe da biblioteca Smartlamp

    // Comandos get-led e set-led
    if (!strcmp(argv[1], "get-status")) {
        cout << "Full status: [ " << aspirador.getFullStatus() << "]" << endl;
    }
    else if (!strcmp(argv[1], "get-roda-T")) {
        cout << "roda status 0: [ " << aspirador.getStatusTracionaRoda(0)<<"|"<<aspirador.getStatusVelocidadeRoda(0) << "]" << endl;
        cout << "roda status 1: [ " << aspirador.getStatusTracionaRoda(1)<<"|"<<aspirador.getStatusVelocidadeRoda(1) << "]" << endl;
    }

    else if (!strcmp(argv[1], "set-roda-Tst")){
         if (argc < 3) {
                cout<<"defina a velocidade"<<endl;
        }
        cout << "set roda tst:" << aspirador.setRoda(2, 0, stoi(argv[2])) << endl;
    }
/*
    else if (!strcmp(argv[1], "set-led")) {
        int ledValue = atoi(argv[2]);
        if (smartlamp.setLed(ledValue))
            cout << "Valor do Led setado para " << ledValue << endl;
        else
            cout << "Erro ao setar valor do Led para " << ledValue << endl;
    }

    // Comando get-luminosity
    else if (!strcmp(argv[1], "get-luminosity")) {
        cout << "Luminosidade atual: " << smartlamp.getLuminosity() << endl;
    }

    // Comandos get-threshold e set-threshold
    else if (!strcmp(argv[1], "get-threshold")) {
        cout << "Valor do Threshold: " << smartlamp.getThreshold() << endl;
    }
    else if (!strcmp(argv[1], "set-threshold")) {
        int thresholdValue = atoi(argv[2]);
        if (smartlamp.setThreshold(thresholdValue))
            cout << "Valor do Threshold setado para " << thresholdValue << endl;
        else
            cout << "Erro ao setar valor do Threshold para " << thresholdValue << endl;
    }

    else {
        cout << "Comando inválido." << endl;
        exit(1);
    } */
}

} // namespace



// MAIN

using namespace devtitans::aspirador; // Permite usar HelloCpp diretamente ao invés de devtitans::hello::HelloCpp

int main(int argc, char **argv) {
    AspiradorClient client;               // Variável hello, da classe HelloCpp, do pacote devtitans::hello
    client.start(argc, argv);             // Executa o método printHello
    return 0;
}
