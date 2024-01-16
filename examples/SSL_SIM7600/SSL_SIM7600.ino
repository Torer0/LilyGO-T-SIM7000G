#include "ssl_web.h"

#define RXD2 27
#define TXD2 26
#define PKEY 5
#define RST 4
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
    Serial.println("TESTEANDO ANDO");
    send_at("AT+CGMR", 500);
    send_at("AT+CNETSTART", 1000);
    send_at("AT+CNSMOD=1", 1000);
    send_at("AT+CNSMOD?", 1000);
    send_at("AT+CGACT=1", 1000);
    send_at("AT+CGACT?", 1000);
    send_at("AT+CGDCONT=1", 1000);
    send_at("AT+CGSMS=2", 1000);
    send_at("AT+CGSMS?", 1000);
    send_at("AT+CNETIPADDR?", 1000);
    /* send_at("AT+CFUN=0\r\n");
    delay(3000);
    send_at("AT+CFUN=1\r\n");
    send_at("AT+NETCLOSE\r\n");
    send_at("AT+CFUN=1\r\n");
    send_at("AT+CRESET\r\n");
    delay(6000); */
    send_at("ATI\r\n", 2000);
    send_at("AT+CFUN=1\r\n", 10000);
    send_at("AT+CPIN?\r\n", 10000);
    send_at("AT+CSQ", 1000);
    send_at("AT+CREG?", 1000);
    send_at("AT+COPS?", 1000);
    send_at("AT+CPSI?", 500);
    send_at("AT+CGATT?", 1000);
    send_at("AT+CREG?", 1000);
    send_at("AT+CGDCONT?", 1000);
    send_at("AT+CGDCONT=1,\"IP\",\"claro.pe\"", 5000);
    send_at("AT+CGACT=1,1", 1000);
    send_at("AT+CGATT?", 1000);
    send_at("AT+CGPADDR=1", 500);
    send_at("AT+NETSTATE", 500);

    /* send_at("AT+CNACT=1,\"claro.pe\"\r\n",1000);
    send_at("AT+CSQ\r\n");
    send_at("AT+CREG?\r\n");
    send_at("AT+CPSI?\r\n");
    send_at("AT+CGREG?\r\n");
    send_at("AT+CGREG=2\r\n"); // enable network
    send_at("AT+CSQ\r\n");
    send_at("AT+CNSMOD=1\r\n"); // estado del SIM
    send_at("AT+CNSMOD=?\r\n");
    send_at("AT+CNSMOD=?\r\n");
    send_at("AT+CGSOCKCONT=1,\"IP\",\"claro.pe\"\r\n");
    send_at("AT+CGACT=1,1");
    send_at("AT+CSOCKSETPN=1\r\n");
    send_at("AT+CGDCONT?\r\n"); */
    /*  pinMode(RST, OUTPUT);
     pinMode(PKEY, OUTPUT);
     digitalWrite(PKEY, LOW);
     digitalWrite(RST, HIGH);
     delay(1000);
     digitalWrite(RST, LOW);
     delay(1000);
     digitalWrite(RST, HIGH);
     delay(1000);
     delay(1000);
     wRespon(15000); */
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
    send_at("AT+CCLK=\"23/12/29,12:34:56\"\r\n", 1000);
    send_at("AT+CCLK?\r\n", 1000);
    send_at("AT+CCERTLIST\r\n", 1000);
    send_at("AT+CCERTDELE=\"tinygsm_key.der\"\r\n", 1000);
    send_at("AT+CCERTLIST\r\n", 1000);
    send_at("AT+CCERTDOWN=\"tinygsm_key.der\",1837\r\n", 1000);
    wRespon(5000);
    SerialAT.write(root_ca);
    wRespon(1000);

    send_at("AT+CCERTLIST\r\n", 1000);

    // check network status
    // check_gprs();
    // send post
    // init ssl
    init_ssl();
    post_server_ssl();
}

void loop()
{
    // post_server_ssl();
    // delay(1000);
}

void send_at(char *_command, int time)
{
    SerialAT.println(_command);
    wRespon(time);
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
    Serial.print("Response: ");
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
                Serial.println("Ok");
                break;
            }
        }
        if (_i == true)
            break;
    }
    Serial.println("Respuesta finalizada");
}

void post_server_ssl()
{
    send_at("AT+CCHSEND=0\n\r", 1000);
    // len ++;
    // String _str_len="";
    //_str_len = "Content-Length: " + String(len);

    // SETEO DE LA SECUENCIA DE POST
    SerialAT.println("GET devices/api/v1.4/loraUplink HTTP/1.1\r\n");
    SerialAT.println("Authorization: Bearer N6zDqbcP9TTJFsD+CBky6i7DjPwZS3c9\r\n");
    SerialAT.println("Host: dev.api.devices.energyatech.com\r\n");
    SerialAT.println("Content-Type: text/plain\r\n");
    SerialAT.println("Content-Length: 2\r\n");
    SerialAT.println();
    SerialAT.println();
    SerialAT.println();
    wRespon(10000);
}

void check_gprs()
{
    Serial.println("Empezando TEST GPRS");
    send_at("ATI\r\n", 2000);
    send_at("AT+CPIN?\r\n", 1000);
    send_at("AT+CSQ\r\n", 1000);
    send_at("AT+CREG?\r\n", 1000);
    send_at("AT+CGREG\r\n", 1000);
    send_at("AT+COPS?\r\n", 1000);
    send_at("AT+CPSI?\r\n", 1000);
    send_at("AT+CGAUTH=1,0\r\n", 1000);
    SerialAT.println("AT+CGDCONT=1,\"IP\",\"claro.pe\"\r\n");
    wRespon(5000);
    send_at("AT+CGDCONT?\r\n", 1000);
    send_at("AT+CGACT=?\r\n", 1000);
    // send_at("AT+CCHSTART\r");
    init_ssl();
}

void init_ssl()
{
    send_at("AT+CSSLCFG=\"sslversion\",0,4\r\n", 1000);
    send_at("AT+CSSLCFG=\"authmode\",0,1\r\n", 1000);
    send_at("AT+CSSLCFG=\"cacert\",0,\"tinygsm_key.der\"\r\n", 1000);
    send_at("AT+CCHSET=1\r\n", 1000);
    send_at("AT+CCHSTART\r\n", 1000);
    send_at("AT+CCHSSLCFG=0,0\r\n", 1000);
    SerialAT.println("AT+CCHOPEN=0,\"dev.api.devices.energyatech.com\",443,2\r\n");
    wRespon(11000);
    /* res_command(20000, "+CCHOPEN: 0,0\r\n"); */
}