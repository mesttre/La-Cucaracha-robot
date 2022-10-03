#include "aspirador_lib.h"

using namespace std;                                   // Permite usar string, ifstream diretamente ao invés de std::string
using namespace android::base;                         // Permite usar GetBoolProperty ao invés de android::base::GetBoolProperty

namespace devtitans::aspirador {                       // Entra no pacote devtitans::smartlamp

int Aspirador::connect() {
    char dirPath[] = "/sys/kernel/aspirador";
    struct stat dirStat;
    if (stat(dirPath, &dirStat) == 0)
        if (S_ISDIR(dirStat.st_mode))
            return 1;                                  // Se o diretório existir, retorna 1

    // Diretório não existe, vamos verificar a propriedade
    bool allowSimulated = GetBoolProperty("devtitans.aspirador.allow_simulated", true);
    if (!allowSimulated)
        return 0;                                      // Dispositivo não encontrado
    else
        return 2;                                      // Usando valores simulados
}

string Aspirador::readFileValue(string file) {
    int connected = this->connect();

    /*
    if (connected == 2) {                               // Usando valores simulados
        if (file == "status")
            return this->simLedValue;
        else if (file == "threshold")
            return this->simThresholdValue;
        else {
            // "ldr" (luminosity): Gera um número aleatório entre 0 e 100
            random_device dev;
            mt19937 rng(dev());
            uniform_int_distribution<mt19937::result_type> dist100(0,100);
            return (int) dist100(rng);
        }
    }

    else */ if (connected == 1) {                          // Conectado. Vamos solicitar o valor ao dispositivo
        string szValue = "";
        string szBuffer;
        string filename = string("/sys/kernel/aspirador/") + file;
        ifstream file(filename);                        // Abre o arquivo do módulo do kernel

        if (file.is_open()) {                           // Verifica se o arquivo foi aberto com sucesso

//            while ( getline(file, szBuffer )) {
//                szValue += szBuffer;                              // Lê um inteiro do arquivo
//            }
            getline(file, szBuffer );
            //szValue = szBuffer.substr(0, szBuffer.length() - 3);
szValue = szBuffer;
            file.close();
            return szValue;
        }
    }

    // Se chegou aqui, não foi possível conectar ou se comunicar com o dispositivo
    return "";
}

bool Aspirador::writeFileValue(string file, string value) {
    int connected = this->connect();

    /*
    if (connected == 2) {                                // Usando valores simulados
        if (file == "led") {
            this->simLedValue = value;
            return true;
        }
        else if (file == "threshold") {
            this->simThresholdValue = value;
            return true;
        }
    }

    else */ if (connected == 1) {                          // Conectado. Vamos solicitar o valor ao dispositivo
        string filename = string("/sys/kernel/aspirador/") + file;
        ofstream file(filename, ios::trunc);            // Abre o arquivo limpando o seu conteúdo

        if (file.is_open()) {                           // Verifica se o arquivo foi aberto com sucesso
            file << value;                              // Escreve o ledValue no arquivo
            file.close();
            return true;
        }
    }

    // Se chegou aqui, não foi possível conectar ou se comunicar com o dispositivo
    return false;
}

string Aspirador::getStatus() {
    string status = this->readFileValue("status");
    RGS0 = 1;
    sscanf(status.c_str(), "%*s %*s %*s %i %*s %i %*s %i %*s %i %*s %i %*s %i %*s %i %*s %i %*s %i %*s %i %*s", &RGS0, &v_1, &v_2, &SB3, &AA4, &AV5, &RE6, &REFT7, &RD8, &RDFT9);
    //cout<<"tmp"<<RGS0<<v_1<<v_2<<SB3<<AA4<<AV5<<RE6<<REFT7<<RD8<<RDFT9<<endl;
    return status;
}


int Aspirador::getStatusTracionaRoda(int iRoda) {
    this->getStatus();
    if(iRoda == 0){
        return REFT7;
    }else{
        return RDFT9;
    }
}

int Aspirador::setRoda(int iRoda, int iTraciona, int iVelocidade){
    string roda;
    string traciona;
    switch(iRoda){
        case 0 : roda = "D";break;
        case 1 : roda = "E";break;
        case 2 : roda = "T";break;
        default: return 3;
    }
    switch(iTraciona){
        case 0 : traciona = "F";break;
        case 1 : traciona = "T";break;
        default: return 3;
    }

    string param = ""+roda+" "+traciona+" "+to_string(iVelocidade) + " \n";
    writeFileValue("roda", param);
    return 0;
}

int Aspirador::getStatusVelocidadeRoda(int iRoda){
    this->getStatus();
    if(iRoda == 0){
        return RE6;
    }else{
        return RD8;
    }
}

/*
bool Aspirador::setLed(int ledValue) {
    return this->writeFileValue("led", ledValue);
}

int Aspirador::getLuminosity() {
    return this->readFileValue("ldr");
}

int Aspirador::getThreshold() {
    return this->readFileValue("threshold");
}

bool Aspirador::setThreshold(int thresholdValue) {
    return this->writeFileValue("threshold", thresholdValue);
} */

string Aspirador::getFullStatus(){
    return getStatus();
}

} // namespace
