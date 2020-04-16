// Inluir Librerias
#include <WiFi.h>

/*
 * Conexion de red 
 * ssid es el nombre de la red de internet a la que estamos conectados
 * password es la contraseña de la red
 */
const char* ssid     = "ARRIS-A0A2";
const char* password = "AA94724020679540";

/*
 * Se elige el puerto a traves del cual se correra la aplicacion
 */
WiFiServer server(80);

/*
 * Se declara una variable para alamcenar la solicitud HTTP
 */
 String header;

/*
 * Se declara el estado inicial de las variables
 */
 String outputEstadoVentilador = "off";
 String outputEstadoBuzzer = "off";
 String outputEstadoRGB = "off";

 /*
  * Asignando variables a pines ESP32
  */
  const int outputVentilador = 12;
  const int outputBuzzer = 18;
  const int outputBlue = 21;
  const int outputRed = 19;
  const int outputFuego = 5;
  int digitalVal;

void setup(){
  Serial.begin(115200);
  //Inicalizar variables declaras en OUTPUT
  pinMode(outputVentilador, OUTPUT);
  pinMode(outputBuzzer, OUTPUT);
  pinMode(outputBlue, OUTPUT);
  pinMode(outputRed, OUTPUT);
  pinMode(outputFuego, INPUT);
  

  //Iniciar variables declaras como LOW es decir apagado
  digitalWrite(outputVentilador, LOW);
  digitalWrite(outputBuzzer, LOW);
  digitalWrite(outputBlue, LOW);
  digitalWrite(outputRed, LOW);

  //Conectarse a WiFi
  Serial.print("Conectandose a: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  //Mostrar direccion Ip y encender servidor web
  Serial.println("");
  Serial.println("Wifi conectado");
  Serial.println("Direccion IP");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  digitalVal = digitalRead(outputFuego);
  WiFiClient client = server.available(); //Lista de clientes 
  if(client){
    Serial.println("Nuevo cliente.");
    String currentLine = "";
    while(client.connected()){
      if(client.available()){
        char c = client.read();
        Serial.write(c);
        header += c;
        if(c == '\n'){
          if(currentLine.length() == 0){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if(header.indexOf("GET /12/on") >= 0 ){
              Serial.println("GPIO 12 on");
              outputEstadoVentilador = "on";
              digitalWrite(outputVentilador, HIGH);
            }else if(header.indexOf("GET /12/off") >= 0){
              Serial.println("GPIO 12 off");
              outputEstadoVentilador = "off";
              digitalWrite(outputVentilador, LOW);
            }else if(header.indexOf("GET /18/on") >= 0){
              Serial.println("GPIO 18 on");
              outputEstadoBuzzer = "on";
              digitalWrite(outputBuzzer, HIGH);
              delay(50);
              digitalWrite(outputBuzzer, HIGH);
              delay(100);
              digitalWrite(outputBuzzer, HIGH);
              delay(150);
              digitalWrite(outputBuzzer, HIGH);
              delay(100);
              digitalWrite(outputBuzzer, HIGH);
              delay(50);
            }else if(header.indexOf("GET /18/off") >= 0){
              Serial.println("GPIO 18 off");
              outputEstadoBuzzer = "off";
              digitalWrite(outputBuzzer, LOW);
            }else if(/*header.indexOf("GET /rgb/on") >= 0*/digitalVal == HIGH){
              Serial.println("GPIO rgb on");
              outputEstadoRGB = "on";
              digitalWrite(19,HIGH);
              /*for(int i=0; i<=10; i++){
                delay(50);
                digitalWrite(21, HIGH);
                delay(50);
                digitalWrite(21, LOW);
                delay(50);
                digitalWrite(19,HIGH);
                delay(50);
                digitalWrite(19, LOW);
              }*/
            }else if(/*header.indexOf("GET /rgb/off") >= 0*/digitalVal == LOW){
              Serial.println("GPIO rgb off");
              outputEstadoRGB = "off";
              digitalWrite(21, 0);
              digitalWrite(19, 0);
            }

            /*
             * A partir de aqui se comienza a crear la pantalla que se mostrara
             */
             client.println("<!DOCTYPE html><html>");
             client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
             client.println("<link rel=\"icon\" href=\"data:,\">");
             //CSS para estilos de los botones cuando este en ON y OFF
             //
             client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
             client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
             client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
             client.println(".button2 {background-color: #555555;}</style></head>");
             
             //Pagina Web 
             client.println("<body><h1>JaquiAnLui</h1>");

             //A partir de aqui se programan los botones del motor
             client.println("<p>El ventilador esta en estado: " + outputEstadoVentilador + "</p>");
             //Si el estado es OFF pasar el boton en ON
             if(outputEstadoVentilador=="off"){
              client.println("<p><a href=\"/12/on\"><button class=\"button\">Encender</button></a></p>");
             }else {
              client.println("<p><a href=\"/12/off\"><button class=\"button button2\">Apagar</button></a></p>");
            }
            //Aqui termina botones motor

            //A partir de aqui se programan los botones del buzzer
            client.println("<p>El Buzzer esta en estado: " + outputEstadoBuzzer + "</p>");
             //Si el estado es OFF pasar el boton en ON
             if(outputEstadoBuzzer=="off"){
              client.println("<p><a href=\"/18/on\"><button class=\"button\">Encender</button></a></p>");
             }else {
              client.println("<p><a href=\"/18/off\"><button class=\"button button2\">Apagar</button></a></p>");
            }
            //Aqui termina botones de Buzzer

            //A partir de aqui se programan los botones del buzzer
            /*client.println("<p>El RGB esta en estado: " + outputEstadoRGB + "</p>");
             //Si el estado es OFF pasar el boton en ON
             if(outputEstadoRGB=="off"){
              client.println("<p><a href=\"/rgb/on\"><button class=\"button\">Encender</button></a></p>");
             }else {
              client.println("<p><a href=\"/rgb/off\"><button class=\"button button2\">Apagar</button></a></p>");
            }*/
            if(digitalVal == HIGH){
              client.println("<label>Fuego Cuidado</label>");
            } else if(digitalVal == LOW) {
              client.println("<label>Todo tranquilo no te preocupes</label>");
            }
            //Aqui termina botones de Buzzer
            
            client.println("</body></html>");

            //
            client.println();
            //
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r'){
          currentLine += c;
        }
      }
    }
    //
    header = "";
    //
    client.stop();
    Serial.println("Cliente desconectado");
    Serial.println("");
  }
}

 
