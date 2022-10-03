#pragma once                           // Inclui esse cabeçalho apenas uma vez

#include <iostream>
#include <string>
#include <fstream>                     // Classe ifstream
#include <android-base/properties.h>   // Função GetBoolProperty
#include <sys/stat.h>                  // Função e struct stat
#include <random>                      // Geração de números aleatórios (valores simulados)
#include <stdio.h>
using namespace std;                   // Permite usar string diretamente ao invés de std::string

namespace devtitans::aspirador {       // Pacote Smartlamp

class Aspirador {
    public:
/**
  *  Verifica se o diretório /sys/kernel/aspirador existe. Se existir
  *  o dispositivo Aspirador está conectado via USB. Caso contrário,
  *  verifica a propriedade devtitans.aspirador.allow_simulated
  *  para ver se valores simulados podem ser usados.
  *
  *  Retorna:
  *      0x00: dispositivo não encontrado
  *      0x01: sucesso
  *      0xFF: simulado (disp. não encontrado)
  */
 int connect();

 /**
  * Comando para tracionar as rodas do dispositivo
  * iRoda:
  *    0x00 - ligar ou desligar a roda esquerda,
  *    0x01 - ligar ou desligar a roda direita.
  *    0x02 - ligar ou desligar as duas rodas.
  * iTraciona:
  *    0x00 - tracionada para frente,
  *    0x01 - tracionada para trás.
  * iVelocidade:
  *  0x00 - desligada a roda.
  *  0x01 ou mais - velocidade da roda.
  *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - o comando para ligar ou desligar a roda foi executado com
  *                sucesso.
  *     0x01 - o comando para tracionar a roda não foi executado com sucesso
  *     0x03 - Parâmetro inválido
  */
  int setRoda(int iRoda, int iTraciona, int iVelocidade);

  /**
   * Método para verificar o status de tracionamento da roda
   *
   * iRoda:
   *    0x00 - ligar ou desligar a roda esquerda,
   *    0x01 - ligar ou desligar a roda direita.
   *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0xFE - Parâmetro inválido
  *     0x00 - Roda está tracionada para frente
  *     0x01 - Roda está tracionada para trás
  *
  */
  int getStatusTracionaRoda(int iRoda);

  /**
   * Método para verificar velocidade da roda
   *
   * iRoda:
   *    0x00 - ligar ou desligar a roda esquerda,
   *    0x01 - ligar ou desligar a roda direita.
   *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0xFE - Parâmetro inválido
  *     0x00 - Roda está desligada
  *     0x01 ou mais - Velocidade da roda
  *
  */
  int getStatusVelocidadeRoda(int iRoda);

 /**
  * Comando para ligar/desligar o motor do aspirador de pó
  * Entrada:
  *     - iLigaDesliga:
  *                0x00 - desligar o motor
  *                 0x01 - ligar o motor.
  *
  * Retorna:
  *     0x00 - o comando para ligar ou desligar o motor do aspirador de pó foi
  *                executado com sucesso.
  *     0x01 - o comando para ligar ou desligar o motor do aspirador de pó
  *                não foi executado com sucesso.
  *     0x03 - Parâmetro inválido
  */
  int setAspirador(int iLigaDesliga);

 /**
  * Método para verificar status de funcionamento do motor do aspirador
  *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - se o motor do aspirador estiver desligado.
  *     0x01 - se o motor do aspirador estiver ligado.
  *     0x03 - Parâmetro inválido
  */
  int getStatusAspirador();

 /**
  * Comando para ligar/desligar a vassoura
  * Entrada:
  *     - iLigaDesliga:
  *                0x00 - desligar a vassoura
  *                 0x01 - ligar a vassoura.
  *
  * Retorna:
  *     0x00 - o comando para ligar ou desligar a vassoura foi executado com
  *                sucesso.
  *     0x01 - o comando para ligar ou desligar a vassoura não foi executado
  *                com sucesso.
  *     0x03 - Parâmetro inválido
  */
  int setVassoura(int iLigaDesliga);


  /**
  * Método para verificar status do sensor ultrassonico
  *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - se o sensor ultrassônico não identificar obstáculos.
  *     0x01 ou mais - se o sensor ultrassônico identificar obstáculo.
  *
  */
  int getStatusSensorUltrassonico();

  /**
  * Método para verificar status do sensor de batida
  *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - se o sensor de batida não for acionado.
  *     0x01 - se o sensor de batida for acionado.
  *
  */
  int getStatusSensorBatida();

  /**
  * Método para verificar status do sensor de luminosidade
  *
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - se o sensor não identificar luminosidade.
  *     0x01 ou mais - se o sensor identificar luminosidade.
  *
  */
  int getStatusSensorLuminosidade();

  string getFullStatus();

private:
	int RGS0;
	int v_1;
	int v_2;
	int SB3;
	int AA4;
	int AV5;
	int RE6;
	int REFT7;
	int RD8;
	int RDFT9;
 /**
  * Busca o status de funcionamento do dispositivo aspirador de pó
  *
  * Entrada:
  *      aStatusSensor - ["RGS",0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  *                                  0x00]
  * Retorna:
  *     0xFF - Não foi possível se comunicar com o dispositivo
  *     0x00 - comando executado com sucesso
  *     0x01 - comando não foi executado com sucesso
  *     0x03 - Parâmetro inválido
  */
  string getStatus();
 int getValuesStatus();
  /**
   * Métodos para ler e escrever valores nos arquivos "status",
   *  do diretório /sys/kernel/aspirador.
   */
  string readFileValue(string file);
  bool writeFileValue(string file, string value);
};

} // namespace
