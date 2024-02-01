
#define RXD2 27
#define TXD2 26
#define PKEY 32
#define RST 33
unsigned long cur_time, old_time;
#define TINY_GSM_MODEM_SIM7600
#include <TinyGsmClient.h>
#define SerialMon Serial
#define TINY_GSM_DEBUG SerialMon
#define SerialAT Serial2

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

bool hold = 0;

#include <Wire.h>
String str_dt;
// void wRespon(long waktu);
// void request_i2c();
// void send_at(char *_command);
int len = 0;
void SIM800_purgeSerial()
{
    while (SerialAT.available())
        SerialAT.read();
}

void setup()
{
    // put your setup code here, to run once:
    SerialMon.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(1000);
    Serial.println("TESTEO");
    // SIM800_purgeSerial();
    send_at("AT");
    send_at("ATI");
    send_at("AT+CREG?");
    send_at("AT+CCLK?");
    send_at("AT+SAPBR=0,1");
    send_at("AT+CTZU=0");
    send_at("AT+CTZU?");
    send_at("AT+CCLK?");
    send_at("AT+CWDHCP?");
    send_at("AT+CIPCLOSE");
    send_at("AT+NETCLOSE");
    send_at("AT+CCHSTOP");
    send_at("AT+HTTPTERM");
    init_gprs();
    //  SIM800_purgeSerial();
    //   send_at("AT+CIPOPEN=0,\"TCP\",\"vsh.pp.ua\",80");

    /* send_at("AT+CCLK=\"24/01/03,12:34:56\"");
send_at("AT+CCLK?"); */
    /*SerialAT.println("AT");
      wRespon(1000);
      SerialAT.println("AT+IPREX?");
      wRespon(1000);
      SerialAT.println("AT+IPREX=115200");
      wRespon(1000);
      SerialAT.println("AT&W");
      wRespon(1000);
      SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
    */

    /*send_at("AT+CCERTLIST");
      send_at("AT+CCERTDELE=\"ska_key.der\"");
      send_at("AT+CCERTLIST");

      send_at("AT+CCERTDOWN=\"ska_key.der\",1937");
      wRespon(5000);
      SerialAT.write(root_ca);
      wRespon(1000);
    */

    // send_at("AT+CCERTDELE=\"ska_key.pem\"");
    // send_at("AT+CCERTLIST");

    // check_gsm();
    // init_https();
    // init_mqtt();
    init_tcp_mqtt();
    // init_tcp_https();
}
void init_gprs()
{

    send_at("AT+SAPBR=3,1,\"APN\",\"claro.pe\"");
    delay(10000);
    // SIM800_purgeSerial();
    send_at("AT+SAPBR=1,1");
    delay(20000);
    // SIM800_purgeSerial();
    send_at("AT+SAPBR=2,1");
    delay(10000);
    // SIM800_purgeSerial();
}
void loop()
{
}

void send_at(char *_command)
{
    SerialAT.println(_command);
    wRespon(500);
}

void wRespon(long waktu)
{
    cur_time = millis();
    old_time = cur_time;
    while (cur_time - old_time < waktu)
    {
        cur_time = millis();
        while (SerialAT.available() > 0)
        {
            Serial.print(SerialAT.readString());
        }
    }
}

void res_serv(long waktu)
{
    unsigned long cur_time_res, old_time_res;
    char c;
    cur_time_res = millis();
    old_time_res = cur_time_res;
    while (cur_time_res - old_time_res < waktu)
    {
        cur_time_res = millis();
        while (SerialAT.available())
        {
            c = SerialAT.read();
            Serial.print(c);
            if (c == 'K')
                break;
        }
        if (c == 'K')
            break;
    }
}

void transmitirDatosHex(const uint8_t *datos, size_t longitud)
{
    char longitudStr[10];
    itoa(longitud, longitudStr, 10);
    char comando[20];
    strcpy(comando, "AT+CIPSEND=");
    strcat(comando, longitudStr);

    SerialAT.println(comando);
    wRespon(10000);
    for (size_t i = 0; i < longitud; ++i)
    {

        SerialAT.write(datos[i]);
    }
    wRespon(10000);
}

void res_command(long waktu, char *res)
{
    unsigned long cur_time_res, old_time_res;
    String buf_res = "";
    int _i = 0;
    Serial.println();
    Serial.print("tunggu: ");
    Serial.println(waktu);
    cur_time_res = millis();
    old_time_res = cur_time_res;
    while (cur_time_res - old_time_res < waktu)
    {
        cur_time_res = millis();
        while (SerialAT.available())
        {
            _i = SerialAT.find(res);
            if (_i == true)
            {
                Serial.println("respon ok");
                break;
            }
        }
        if (_i == true)
            break;
    }
    Serial.println("tunggu selesai");
}

void rst()
{
    digitalWrite(RST, HIGH);
    delay(1000);
    digitalWrite(RST, LOW);
    delay(1000);
    digitalWrite(RST, HIGH);
    delay(1000);
}

void post_server_ssl()
{
    send_at("AT+CCHSEND=0,106");
    // len ++;
    // String _str_len="";
    //_str_len = "Content-Length: " + String(len);
    SerialAT.println("GET /TinyGSM/logo.txt HTTP/1.1");
    SerialAT.println("Host: vsh.pp.ua");
    SerialAT.println("Content-Type: text/plain");
    SerialAT.println("Content-Length: 2");
    SerialAT.println();
    SerialAT.println();
    SerialAT.println();
    wRespon(1000);
}

void check_gsm()
{
    send_at("AT+CSQ");
    send_at("AT+CREG?");
    /* send_at("AT+CGDCONT?");
    send_at("AT+CGSOCKCONT=1,\"IP\",\"entel.pe\"");
    send_at("AT+CGPADDR"); */
    // init_ssl();
}

void init_ssl()
{
    send_at("AT+CSSLCFG=\"sslversion\",0,4");
    send_at("AT+CSSLCFG=\"authmode\",0,1");
    send_at("AT+CSSLCFG=\"ignorelocaltime\",0,1");
    send_at("AT+CSSLCFG=\"negotiatetime\",0,300");
    send_at("AT+CCERTDOWN=\"apieat.der\",1825");
    wRespon(5000);
    // SerialAT.write(root_ca);
    // wRespon(10000);
    send_at("AT+CSSLCFG=\"ciphersuites\",0,0xFFFF");
    send_at("AT+CSSLCFG=\"cacert\",0,\"apieat.der\"");
    send_at("AT+CCERTLIST");
    send_at("AT+CCERTDELE=\"apieat.der\"");
    //  configurar GPRS netword
    send_at("AT+CGDCONT=1,\"IP\",\"claro.pe\"");
    // init ssl
    send_at("AT+CCHSTART");
    send_at("AT+CSSLCFG=0");
    send_at("AT+CCHSSLCFG=0,0");
    SerialAT.println("AT+CCHOPEN=0,\"dev.api.devices.energyatech.com\",443,2\r\n");
    wRespon(11000);
}
void init_http()
{
    send_at("AT+HTTPINIT");
    send_at("AT+HTTPPARA=\"CID\",1");
    send_at("AT+HTTPPARA=\"URL\",\"google.com\"");
    send_at("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
    SerialAT.println("AT+HTTPACTION=0");
    wRespon(10000);
    send_at("AT+HTTPREAD");
    send_at("AT+HTTPTERM");
    send_at("AT+SAPBR=0,1");
}
void init_https()
{
    send_at("AT+HTTPINIT");
    send_at("AT+HTTPPARA=\"CID\",1");
    send_at("AT+HTTPPARA=\"URL\",\"chinawok.com.pe:443\"");
    send_at("AT+HTTPPARA=REDIR,1");
    send_at("AT+HTTPSSL=1");
    send_at("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
    SerialAT.println("AT+HTTPACTION=0");
    wRespon(120000);
    send_at("AT+HTTPREAD");
    send_at("AT+HTTPTERM");
    send_at("AT+SAPBR=0,1");
}

void init_mqtt()
{
    send_at("AT+CMQTTSTART");
    send_at("AT+SMCONF=\"CLEANSS\",1");
    send_at("AT+SMCONN");
    send_at("AT+SMSUB=\"Topic1test\",1");
    send_at("AT+SMPUB=\"Topic1test\",1,0,\"hello world\"");
    SerialAT.println("AT+SMUNSUB=\"Topic1test\"");
    SerialAT.println("AT+SMDISC");
    SerialAT.println("AT+SAPBR=0,1");
}
void init_tcp_https()
{
    send_at("AT+CIPSHUT");
    send_at("AT+CIPMUX=0");
    send_at("AT+CIPRXGET=1");
    send_at("AT+CIPSTART=\"TCP\",\"dev.api.devices.energyatech.com\",\"443\"");
    delay(3000);
    send_at("AT+CIPSTATUS");
    delay(3000);
    send_at("AT+CIPSEND=15");
    SerialMon.println("GET HTTP/1.1");
    SerialMon.println(char(0x1A));
    wRespon(10000);
    send_at("AT+SAPBR=0,1");
}
void init_tcp_mqtt()
{
    send_at("AT+CIPSHUT");
    send_at("AT+CIPMUX=0");
    send_at("AT+CIPRXGET=1");
    send_at("AT+CIPSTART=\"TCP\",\"masi.smartiotgroup.com\",\"1883\"");
    delay(3000);
    send_at("AT+CIPSTATUS");
    delay(3000);
    uint8_t datoshex[] = {0x10, 0x2c, 0x00, 0x06, 0x4d, 0x51, 0x49, 0x73, 0x64, 0x70, 0x03, 0xC2, 0x00, 0x3c, 0x00, 0x06, 0x40, 0x42, 0x43, 0x44, 0x45, 0x46, 0x00, 0x08, 0x6a, 0x7a, 0x6b, 0x68, 0x79, 0x69, 0x76, 0x79, 0x00, 0x0c, 0x36, 0x66, 0x75, 0x43, 0x54, 0x47, 0x79, 0x7a, 0x32, 0x5f, 0x4a, 0x58, 0x30, 0x13, 0x00, 0x08, 0x61, 0x62, 0x63, 0x64, 0x66, 0x66, 0x66, 0x66, 0x30, 0x31, 0x32, 0x33, 0x54, 0x35, 0x47, 0x44, 0x30, 0x1a};
    uint8_t datoshex2[] = {0x82, 0x0D, 0x00, 0x01, 0x00, 0x08, 0x76, 0x61, 0x6C, 0x65, 0x74, 0x72, 0x6F, 0x6E, 0x00, 0x1A};
    // send_at("AT+CIPSEND=54");
    SerialMon.println(datoshex[65]);
    transmitirDatosHex(datoshex, sizeof(datoshex));
    transmitirDatosHex(datoshex2, sizeof(datoshex2));
    // send_at("AT+SAPBR=0,1");
}