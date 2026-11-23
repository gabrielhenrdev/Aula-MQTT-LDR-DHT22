#include <Arduino.h>
#include <WiFi.h>
#include "internet.h"
#include "senha.h"

const unsigned long tempoConexao = 10000;
const unsigned long tempoReconexao = 5000;
unsigned long tempoultimaConexao = 0;

int minuto;
int segundo;
int hora;
int hora12;
int dia;
int diaAno;
int mes;
int ano;

bool am;
bool pm;

void conexaoWIFI() {

    Serial.printf("Conectando ao WiFi: %s\n", SSID);

    WiFi.begin(SSID, SENHA);

    unsigned long tempoWifi = millis();
    unsigned long tempoPonto = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - tempoWifi < tempoConexao) {

        if (millis() - tempoPonto >= 500) {

            tempoPonto = millis();

            Serial.print("-");
        }
    }

    if (WiFi.status() == WL_CONNECTED) {

        Serial.println("\nConectado ao WiFi!");

        Serial.print("Endereco IP: ");
        Serial.println(WiFi.localIP());

    } else {

        Serial.println("\nFalha ao se conectar.");
    }
}

void reconexaoWIFI() {

    unsigned long tempoAtual = millis();

    if (tempoAtual - tempoultimaConexao >= tempoReconexao) {

        tempoultimaConexao = tempoAtual;

        if (WiFi.status() != WL_CONNECTED) {

            Serial.println("Voce perdeu a conexao!");

            conexaoWIFI();

            if (WiFi.status() == WL_CONNECTED) {

            }
        }
    }
}