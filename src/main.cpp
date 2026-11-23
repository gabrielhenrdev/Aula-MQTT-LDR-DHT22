
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "internet.h"
#include <ezTime.h>

Timezone sp;

const int LED = 2;

const char *mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char *mqtt_client_id = "esp-gabrielh";
const char *mqtt_topic_sub = "PedraoArduino/pub";
const char *mqtt_topic_pub = "PedraoArduino/sub";

void conectaMqtt();
void retornoMqtt(char *topic, byte *payload, unsigned int length);

WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(5000);

    pinMode(LED, OUTPUT);

    conexaoWIFI();

    sp.setLocation("America/Sao_Paulo");

    waitForSync();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(retornoMqtt);
}

void loop()
{
    reconexaoWIFI();

    client.loop();

    if (!client.connected())
    {
        conectaMqtt();
    }

    // Ler da serial

    if (Serial.available() > 0)
    {
        String textodig = Serial.readStringUntil('\n');
        textodig.trim();

        if (textodig.length() == 0)
        {
            Serial.println("Mensagem Vazia ");
            return;
        }

        // Montar um Json

        JsonDocument doc;

        doc["disp"] = "Esp-gabrielh";
        doc["msg"] = textodig;
        doc["timestamp"] = sp.dateTime("d-m-Y H:i:s");

        String stringJson;
        serializeJson(doc, stringJson);

        // Publica no MQTT

        client.publish(mqtt_topic_pub, stringJson.c_str());

        Serial.print("Enviado: ");
        Serial.println(stringJson);
    }
}

void conectaMqtt()
{
    while (!client.connected())
    {
        Serial.println("Conectando ao MQTT ");

        if (client.connect(mqtt_client_id))
        {
            Serial.println("Conectado com Sucesso! ");
            client.subscribe(mqtt_topic_sub);
        }
        else
        {
            Serial.println("Erro ao conectar! ");
            Serial.println(client.state());
            Serial.println("Tentando novamente em 5s ");
            delay(5000);
        }
    }
}

void retornoMqtt(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Mensagem recebida em ");
    Serial.print(topic);
    Serial.print(": ");

    String msgRecebida = "";

    for (int i = 0; i < length; i++)
    {
        msgRecebida += (char)payload[i];
    }

    Serial.println(msgRecebida);

    // Interpretar o Json

    JsonDocument doc;

    DeserializationError erro = deserializeJson(doc, msgRecebida);

    if (erro)
    {
        Serial.println("Erro ao decodificar o Json: ");
        Serial.println(erro.c_str());
        return;
    }

    // Acessar os campos do Json

    const char *dispositivo = doc["disp"];
    const char *timestamp = doc["timestamp"];
    const char *msg = doc["msg"];

    Serial.println("Dados decodificados ");
    Serial.print("Esp: ");
    Serial.println(dispositivo);
    Serial.print("Mensagem: ");
    Serial.println(msg);
    Serial.print("Esp: ");
    Serial.println("-----------------");


    // ----------- Controle LED
if (String(msg) == "L")
{
  digitalWrite(LED, HIGH);
  Serial.println("LED ligado!");
}
else if (String(msg) == "D")
{
  digitalWrite(LED, LOW);
  Serial.println("LED desligado!");
}
}