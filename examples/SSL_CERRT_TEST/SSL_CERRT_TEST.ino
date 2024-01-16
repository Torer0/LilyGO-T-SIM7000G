#include "ssl_web.h"

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

void setup()
{
    // put your setup code here, to run once:
    Wire.begin();

    SerialMon.begin(115200);
    SerialAT.begin(115200, SERIAL_8N1, RXD2, TXD2);
    delay(1000);
    Serial.println("test at mulai");
    pinMode(RST, OUTPUT);
    pinMode(PKEY, OUTPUT);
    digitalWrite(PKEY, LOW);
    digitalWrite(RST, HIGH);
    delay(1000);
    digitalWrite(RST, LOW);
    delay(1000);
    digitalWrite(RST, HIGH);
    delay(1000);
    delay(1000);
    wRespon(15000);
    send_at("AT+CCLK=\"24/01/05,16:34:56\"");
    send_at("AT+CCLK?");
    send_at("AT+CIPCLOSE");
    send_at("AT+NETCLOSE");
    send_at("AT+CCHSTOP");
    send_at("AT+CGDCONT=1,\"IP\",\"claro.pe\"");
    send_at("AT+CIPMODE=1");
    // SerialAT.println("AT+NETOPEN");
    wRespon(1000);
    send_at("AT+IPADDR");
    // send_at("AT+CIPOPEN=0,\"TCP\",\"vsh.pp.ua\",80");

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

    send_at("AT+CCERTLIST");
    send_at("AT+CCERTDELE=\"tinygsm_key.der\"");
    send_at("AT+CCERTLIST");
    send_at("AT+CCERTDOWN=\"tinygsm_key.der\",1808");
    wRespon(5000);
    SerialAT.write(root_ca3);
    wRespon(1000);

    send_at("AT+CCERTLIST");
    // send_at("AT+CCERTDELE=\"ska_key.pem\"");
    // send_at("AT+CCERTLIST");

    check_gsm();
    post_server_ssl();
}

void loop()
{
    post_server_ssl();
    delay(1000);
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
    init_ssl();
}

void init_ssl()
{
    send_at("AT+CMGS");
    send_at("AT+CCHSSLCFG=0,0");
    send_at("AT+CSSLCFG=\"sslversion\",0,4");
    send_at("AT+CSSLCFG=\"authmode\",0,1");
    send_at("AT+CSSLCFG=\"cacert\",0,\"tinygsm_key.der\"");
    send_at("AT+CSSLCFG=\"ignorlocaltime\",0,1");
    send_at("AT+CSSLCFG=\"negotiatetime\",0,100000");
    send_at("AT+CCHSET=1");
    send_at("AT+CCHSTART");
    send_at("AT+CCHOPEN=0,\"vsh.pp.ua\",443,2");
    res_command(20000, "+CCHOPEN: 0,0");
    send_at("AT+CIPOPEN?");
    send_at("AT+CGDCONT=?");
    send_at("AT+CGDCONT=?");
}