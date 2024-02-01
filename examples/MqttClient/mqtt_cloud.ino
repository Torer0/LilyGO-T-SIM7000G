#include <Arduino.h>
int led = 13;
unsigned int Counter = 0;
unsigned long datalength, CheckSum, RLength;
unsigned short topiclength;
unsigned char topic[30];
char str[250];
unsigned char encodedByte;
int X;
unsigned short MQTTProtocolNameLength;
unsigned short MQTTClientIDLength;
unsigned short MQTTUsernameLength;
unsigned short MQTTPasswordLength;

const char MQTTHost[30] = "masi.smartiotgroup.com";
const char MQTTPort[10] = "1883";
const char MQTTClientID[20] = "EnzoRivas";
const char MQTTTopic[30] = "eat/test";
const char MQTTProtocolName[10] = "MQIsdp";
const char MQTTLVL = 0x03;
const char MQTTFlags = 0x02;
const unsigned int MQTTKeepAlive = 60;
const char MQTTUsername[30] = "dxxkgkpp";
const char MQTTPassword[35] = "qAUZBdaSIULx";
const char MQTTQOS = 0x00;
const char MQTTPacketID = 0x0001;
void init()
{
    pinMode(led, OUTPUT);
    Serial.begin(9600);
    Serial.println("Arduino MQTT Tutorial, Valetron Systems @www.raviyp.com ");
    delay(3000);
}

void SendConnectPacket(void)
{
    Serial2.print("\r\nAT+CIPSEND\r\n");
    delay(3000);

    Serial2.write(0x10);

    MQTTProtocolNameLength = strlen(MQTTProtocolName);
    MQTTClientIDLength = strlen(MQTTClientID);
    MQTTUsernameLength = strlen(MQTTUsername);
    MQTTPasswordLength = strlen(MQTTPassword);
    datalength = MQTTProtocolNameLength + 2 + 4 + MQTTClientIDLength + 2 + MQTTUsernameLength + 2 + MQTTPasswordLength + 2;

    X = datalength;
    do
    {
        encodedByte = X % 128;
        X = X / 128;
        // if there are more data to encode, set the top bit of this byte
        if (X > 0)
        {
            encodedByte |= 128;
        }

        Serial2.write(encodedByte);
    } while (X > 0);

    Serial2.write(MQTTProtocolNameLength >> 8);
    Serial2.write(MQTTProtocolNameLength & 0xFF);
    Serial2.print(MQTTProtocolName);

    Serial2.write(MQTTLVL);   // LVL
    Serial2.write(MQTTFlags); // Flags
    Serial2.write(MQTTKeepAlive >> 8);
    Serial2.write(MQTTKeepAlive & 0xFF);

    Serial2.write(MQTTClientIDLength >> 8);
    Serial2.write(MQTTClientIDLength & 0xFF);
    Serial2.print(MQTTClientID);

    /* Serial.write(MQTTUsernameLength >> 8);
    Serial.write(MQTTUsernameLength & 0xFF);
    Serial.print(MQTTUsername);

    Serial.write(MQTTPasswordLength >> 8);
    Serial.write(MQTTPasswordLength & 0xFF);
    Serial.print(MQTTPassword); */

    Serial2.write(0x1A);
}
void SendPublishPacket(void)
{
    Serial2.print("\r\nAT+CIPSEND\r\n");
    delay(3000);

    memset(str, 0, 250);

    topiclength = sprintf((char *)topic, MQTTTopic);
    datalength = sprintf((char *)str, "%s%u", topic, Counter);
    delay(1000);
    Serial2.write(0x30);
    X = datalength + 2;
    do
    {
        encodedByte = X % 128;
        X = X / 128;
        // if there are more data to encode, set the top bit of this byte
        if (X > 0)
        {
            encodedByte |= 128;
        }
        Serial2.write(encodedByte);
    } while (X > 0);

    Serial2.write(topiclength >> 8);
    Serial2.write(topiclength & 0xFF);
    Serial2.print(str);

    Serial2.write(0x1A);
}
void SendSubscribePacket(void)
{
    Serial2.print("\r\nAT+CIPSEND\r\n");
    delay(3000);

    memset(str, 0, 250);
    topiclength = strlen(MQTTTopic);
    datalength = 2 + 2 + topiclength + 1;
    delay(1000);

    Serial2.write(0x82);
    X = datalength;
    do
    {
        encodedByte = X % 128;
        X = X / 128;
        // if there are more data to encode, set the top bit of this byte
        if (X > 0)
        {
            encodedByte |= 128;
        }
        Serial2.write(encodedByte);
    } while (X > 0);
    Serial2.write(MQTTPacketID >> 8);
    Serial2.write(MQTTPacketID & 0xFF);
    Serial2.write(topiclength >> 8);
    Serial2.write(topiclength & 0xFF);
    Serial2.print(MQTTTopic);
    Serial2.write(MQTTQOS);

    Serial2.write(0x1A);
}
void mqtt_cloud()
{

    /* Serial2.print("AT+CSTT=\"www\",\"\",\"\"\r\n");
    delay(1000);
    Serial2.print("AT+CIPMODE=0\r\n");
    delay(1000);
    Serial2.print("AT+CIICR\r\n");
    delay(5000);
    // Serial.print("AT+CIFSR\r\n");
    // delay(2000);
    Serial2.print("AT+CIPSTART=\"TCP\",\"m10.cloudmqtt.com\",\"17434\"\r\n");
    delay(4000); */
    SendConnectPacket();
    delay(5000);
    SendSubscribePacket();
    while (1)
    {
        /*  if (Serial2.available() > 0)
         {
             str[0] = Serial2.read();
             Serial2.write(str[0]);

             if (str[0] == '1')
                 //digitalWrite(led, HIGH);
             if (str[0] == '0')
                 //digitalWrite(led, LOW);
         } */
        delay(5000);
        SendPublishPacket();
        Counter++;
    }
}