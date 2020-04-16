#include <WiFi.h>
#include "FirebaseESP32.h"
#include <DHT.h>

#define FIREBASE_HOST "https://proyecto-iot-bc8cb.firebaseio.com"
#define FIREBASE_AUTH "9Niqfu8TTAV2TQsLjt3rgaI5QwILQtoTuS6hm0JO"
#define WIFI_SSID "ARRIS-A0A2"
#define WIFI_PASSWORD "AA94724020679540"

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
FirebaseData firebaseData;
String ruta = "/ESP32";

const int P0 = 26; 
//int sensorValue = 0;
const int buzzer = 21;
//int nivelAgua = 0;

int digitalPin = 5;
int digitalVal;

#include "SR04.h"
#define TRIG_PIN 12
#define ECHO_PIN 13
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long distancia;

const int outputVentilador = 4;
const int outputRed = 2;
const int outputGreen = 15;
const int outputBlueH1 = 25;
const int outputOranH2 = 33;
const int outputGreenSa = 32;

String outputEstadoVentilador = "off";
String outputEstadoRed = "off";
String outputEstadoGreen = "off";
String outputEstadoBlueH1 = "off";
String outputEstadoOranH2 = "off";
String outputEstadoGreenSa = "off";

int redpin = 19; 
int bluepin =18; 


WiFiServer server(80);
String header;

void setup() {
  /*
   * Inicia Conexion con Firebase
   */
  Serial.begin(115200);

  pinMode(outputVentilador, OUTPUT);
  digitalWrite(outputVentilador, LOW);

  pinMode(outputRed, OUTPUT);
  digitalWrite(outputRed, LOW);

  pinMode(outputGreen, OUTPUT);
  digitalWrite(outputGreen, LOW);

  pinMode(outputBlueH1, OUTPUT);
  digitalWrite(outputBlueH1, LOW);

  pinMode(outputOranH2, OUTPUT);
  digitalWrite(outputOranH2, LOW);

  pinMode(outputGreenSa, OUTPUT);
  digitalWrite(outputGreenSa, LOW);

  pinMode(buzzer, OUTPUT);
  
  pinMode(digitalPin, INPUT);
  dht.begin();

  pinMode(redpin, OUTPUT); 
  pinMode(bluepin, OUTPUT);
  
  Serial.print("Conectado al Wi-Fi");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("Wifi conectado");
  Serial.print("Conectado con IP: ");
  Serial.println(WiFi.localIP());
  server.begin();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

 /*
  * Termina conexion con Firebase
  */
}

void loop() {
/*
 * Inicia Temperatura
 */
 // Esperamos 5 segundos entre medidas
  delay(5000);
 
  // Leemos la humedad relativa
  float h = dht.readHumidity();
  // Leemos la temperatura en grados centígrados (por defecto)
  float t = dht.readTemperature();
  // Leemos la temperatura en grados Fahreheit
  float f = dht.readTemperature(true);
 
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    return;
  }
 
  // Calcular el índice de calor en Fahreheit
  float hif = dht.computeHeatIndex(f, h);
  // Calcular el índice de calor en grados centígrados
  float hic = dht.computeHeatIndex(t, h, false);
 
  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.print(" C ");
  Serial.print("Índice de calor: ");
  Serial.print(hic);
  Serial.println(" *C ");
 /*
  * Termina temperatura
  */

  /*
   * Inicia nivel de agua
   */
   /*sensorValue = analogRead(Agua); 
   nivelAgua = sensorValue*100/1024;
   Serial.print("Sensor cubierto = " ); 
   Serial.print(nivelAgua); 
   Serial.println("%");
   delay(500);*/
   Serial.print("Nivel de agua: ");
   Serial.println(analogRead(P0));
   delay(500);
   /*
    * Termina nivel de agua
    */

    /*
     * Inicia Sensor poximidad
     */
   distancia=sr04.Distance();
   Serial.print(distancia);
   Serial.println("cm");
   delay(500);

   if(distancia<10){
    if(distancia>1){
      digitalWrite(buzzer, HIGH);
    }
  }else{
    digitalWrite(buzzer, LOW);
  }
   /*
    * Termina Sensor proximidad
   */
   
   /*   
    * Inicia detector de fuego
    */
    digitalVal = digitalRead(digitalPin);
    /*
     * Termina detector de fuego
     */
  
  Firebase.setFloat(firebaseData, ruta + "/sensorTemperatura/temperatura", t);
  Firebase.setFloat(firebaseData, ruta + "/sensorTemperatura/humedad", h);
  Firebase.setFloat(firebaseData, ruta + "/sensorTemperatura/indiceCalor", hic);
  Firebase.setInt(firebaseData, ruta + "/sensorNivelAgua/nivel", P0);
  Firebase.setInt(firebaseData, ruta + "/sensorProximidad/distancia", distancia);
  if(digitalVal == HIGH) // si la flama es detectada
    {
      Firebase.setString(firebaseData, ruta + "/sensorFuego/hay", "Cuidado con el fuego");
      Serial.println("Fuego");
      digitalWrite(18,HIGH);
      digitalWrite(19,HIGH);
    }else if(digitalVal == LOW) {
      Firebase.setString(firebaseData, ruta + "/sensorFuego/hay", "Tu tranquilo");
      Serial.println("No Fuego");
      digitalWrite(18,LOW);
      digitalWrite(19,LOW);
    }
    delay(500);
  

  Firebase.getFloat(firebaseData, ruta + "/sensorTemperatura/temperatura");
  float temp = firebaseData.floatData();
  String temp2 = (String) temp;
  Firebase.getFloat(firebaseData, ruta + "/sensorTemperatura/humedad");
  float hum = firebaseData.floatData();
  String hum2 = (String) hum;
  Firebase.getFloat(firebaseData, ruta + "/sensorTemperatura/indiceCalor");
  float ind = firebaseData.floatData();
  String ind2 = (String) ind;
  Firebase.getString(firebaseData, ruta + "/sensorFuego/hay");
  String fue = firebaseData.stringData();
  Firebase.getInt(firebaseData, ruta + "/sensorNivelAgua/nivel");
  int niv = firebaseData.floatData();
  String niv2 = (String) niv;
  Firebase.getInt(firebaseData, ruta + "/sensorProximidad/distancia");
  int dis = firebaseData.floatData();
  String dis2 = (String) dis;
  

   //----------------------------------------------
   //--------------Streaming de datos--------------
   //----------------------------------------------

  Firebase.readStream(firebaseData);    
  if (firebaseData.streamAvailable()) 
    {  
        Serial.println(firebaseData.stringData()); // El tipo de dato debe coincidir con el dato cargado en la nube
        delay(500);                                 // El delay es para que solo se haga una lectura, caso contrario repetira la lectura varias veces,
    }


  WiFiClient client = server.available();
  if(client){
    Serial.println("Nuevo cliente");
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
            }

            if(header.indexOf("GET /red/on") >= 0 ){
              Serial.println("GPIO red on");
              outputEstadoRed = "on";
              digitalWrite(outputRed, 255);
              digitalWrite(outputGreen, 255);
            }else if(header.indexOf("GET /red/off") >= 0){
              Serial.println("GPIO red off");
              outputEstadoRed = "off";
              digitalWrite(outputRed, LOW);
              digitalWrite(outputGreen, LOW);
            }

            if(header.indexOf("GET /BlueH1/on") >= 0 ){
              Serial.println("GPIO BlueH1 on");
              outputEstadoBlueH1 = "on";
              digitalWrite(outputBlueH1, HIGH);
            }else if(header.indexOf("GET /BlueH1/off") >= 0){
              Serial.println("GPIO BlueH1 off");
              outputEstadoBlueH1 = "off";
              digitalWrite(outputBlueH1, LOW);
            }

             if(header.indexOf("GET /OranH2/on") >= 0 ){
              Serial.println("GPIO OranH2 on");
              outputEstadoOranH2 = "on";
              digitalWrite(outputOranH2, HIGH);
            }else if(header.indexOf("GET /OranH2/off") >= 0){
              Serial.println("GPIO OranH2 off");
              outputEstadoOranH2 = "off";
              digitalWrite(outputOranH2, LOW);
            }

              if(header.indexOf("GET /GreenSa/on") >= 0 ){
              Serial.println("GPIO GreenSa on");
              outputEstadoGreenSa = "on";
              digitalWrite(outputGreenSa, HIGH);
            }else if(header.indexOf("GET /GreenSa/off") >= 0){
              Serial.println("GPIO GreenSa off");
              outputEstadoGreenSa = "off";
              digitalWrite(outputGreenSa, LOW);
            }


            
            /*
             * A partir de aqui se comienza a crear la pantalla que se mostrara
             */
             client.println("<!DOCTYPE html><html>");
             client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
             client.println("<meta charset=\"UTF-8\">");
             client.println("<link rel=\"stylesheet\" href=\"https://use.fontawesome.com/releases/v5.7.2/css/all.css\" >");
             client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css\">");
             //client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.4.1/css/bootstrap.min.css\">");
             client.println("<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.4.1/js/bootstrap.min.js\"></script>");
             client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.4.1/jquery.min.js\"></script>");
             //CSS para estilos de los botones cuando este en ON y OFF
             //
             client.println("<style>html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}");
             client.println(".texto { font-size: 3.0rem;");
             client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
             client.println("body {background: rgba(212,228,239,1);background: -moz-linear-gradient(left, rgba(212,228,239,1) 0%, rgba(134,174,204,1) 100%);background: -webkit-gradient(left top, right top, color-stop(0%, rgba(212,228,239,1)), color-stop(100%, rgba(134,174,204,1)));background: -webkit-linear-gradient(left, rgba(212,228,239,1) 0%, rgba(134,174,204,1) 100%);background: -o-linear-gradient(left, rgba(212,228,239,1) 0%, rgba(134,174,204,1) 100%);background: -ms-linear-gradient(left, rgba(212,228,239,1) 0%, rgba(134,174,204,1) 100%);background: linear-gradient(to right, rgba(212,228,239,1) 0%, rgba(134,174,204,1) 100%);filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#d4e4ef', endColorstr='#86aecc', GradientType=1 );}");
             client.println(".button2 {background-color: #555555;}</style></head>");

             client.println("<body style=\"margin: 5%\"><h1>JaquiAnLui</h1>");

             client.println("<a href=\"192.168.0.12\" class=\"btn btn-warning btn-block btn-lg\">Actualizar notificaciones</a></br>");
              
             if(distancia<10){
              client.println("<div class=\"alert alert-danger\" role=\"alert\">Cuidado alguien se acerca</div>");
             }   
             if (t > 30){
              client.println("<div class=\"alert alert-success\" role=\"alert\">Esto esta muy caliente</div>");
             }
             if (digitalVal == HIGH){
              client.println("<div class=\"alert alert-danger\" role=\"alert\">Cuidado tu casa se esta incendiando, <b>CORRE!!!</b></div></br>");
             }
            
            
            
             
            client.println("<div class=\"container mt-2 \">");
              client.println("<section class=\"row\">");
                  client.println("<aside class=\"col-12 col-sm-12 col-md-2 col-lg-4 col-xl-4 justify-content-center\">");
                     client.println("<!--Borrar a partir de aqui-->");
                      client.println("<div class=\"card mb-2\" style=\"height: 100%;\">");
                          client.println("<div class=\"card-body\" style=\"background-color: #fafafa; border-radius: 10px;\">");
                                 client.println("<div class=\"justify-content-center\">");
                                 client.println("</br>");
                                 
                                 //A partir de aqui se programan los botones del motor
                                 client.println("<h2>Ventilador</h2>");
                                 if(outputEstadoVentilador=="off"){
                                  client.println("<a href=\"/12/on\" class=\"btn btn-primary btn-block\">Encender</a></br>");
                                 }else {
                                  client.println("<a href=\"/12/off\" class=\"btn btn-danger btn-block\">Apagar</a></br>");
                                 }
                                 //Aqui termina botones motor

                                 //A partir de aqui se programan los botones de luz roja
                                 client.println("<h2>Luces exteriores</h2>");
                                 if(outputEstadoRed=="off"){
                                  client.println("<a href=\"/red/on\" class=\"btn btn-primary btn-block\">Encender</a></br>");
                                 }else {
                                  client.println("<a href=\"/red/off\" class=\"btn btn-danger btn-block\">Apagar</a></br>");
                                 }
                                 //Aqui termina botones luz roja

                                 //A partir de aqui se programan los botones de luz azul
                                 client.println("<h2>Habitación principal</h2>");
                                 if(outputEstadoBlueH1=="off"){
                                  client.println("<a href=\"/BlueH1/on\" class=\"btn btn-primary btn-block\">Encender</a></br>");
                                 }else {
                                  client.println("<a href=\"/BlueH1/off\" class=\"btn btn-danger btn-block\">Apagar</a></br>");
                                 }
                                 //Aqui termina botones luz azul

                                 //A partir de aqui se programan los botones de luz naranja
                                 client.println("<h2>Habitación huéspedes</h2>");
                                 if(outputEstadoOranH2=="off"){
                                  client.println("<a href=\"/OranH2/on\" class=\"btn btn-primary btn-block\">Encender</a></br>");
                                 }else {
                                  client.println("<a href=\"/OranH2/off\" class=\"btn btn-danger btn-block\">Apagar</a></br>");
                                 }
                                 //Aqui termina botones luz naranja

                                 //A partir de aqui se programan los botones de luz verde
                                 client.println("<h2>Sala</h2>");
                                 if(outputEstadoGreenSa=="off"){
                                  client.println("<a href=\"/GreenSa/on\" class=\"btn btn-primary btn-block\">Encender</a></br>");
                                 }else {
                                  client.println("<a href=\"/GreenSa/off\" class=\"btn btn-danger btn-block\">Apagar</a></br>");
                                 }
                                 //Aqui termina botones luz verde
                                
                                  client.println("</div>");
                                client.println("</div>");
                          client.println("</div>");
                          client.println("</div>");
                     client.println("</div>");
                  client.println("</aside>");
                    client.println("</br>");
                    client.println("</br>");
                    
           client.println("<main class=\"col-12 col-sm-12 col-md-10 col-lg-8 col-xl-8\">");
              client.println("<div class=\"card mb-2\">");
                    client.println("<div class=\"card-body \" style=\"background-color: #fafafa; border-radius: 10px;\">");
                        client.println("<div class=\"row \">");
                            client.println("<div class=\"col-12\" style=\"margin: 10%;\">");
                                   client.println("<span class=\"text\">Datos de temperatura</span>");
                                   client.println("<p><i class=\"fas fa-thermometer-half\" style=\"color:#059e8a;\"></i><span> Temperatura: </span><span>"+temp2+"</span></p>");
                                   client.println("<p><i class=\"fas fa-tint\" style=\"color:#00add6;\"></i><span> Humedad: </span><span>"+hum2+" %</span></p>");
                                   client.println("<p><i class=\"fas fa-sun\" style=\"color:#F5720B;\"></i><span> Inidice de calor: </span><span>"+ind2+" %</span></p></br>");
                      
                                   client.println("<span class=\"text\">Datos Fuego</span>");
                                   if(digitalVal == HIGH) // si la flama es detectada
                                   {
                                     client.println("<p><i class=\"fas fa-fire\" style=\"color:#F5720B;\"></i><span> Estado: </span><span>"+fue+"</span></p></br>");
                                   }else if(digitalVal == LOW) {
                                     client.println("<p><i class=\"fas fa-fire\" style=\"color:#B6ACA4;\"></i><span> Estado: </span><span>"+fue+"</span></p></br>");
                                   }
                      
                                   client.println("<span class=\"text\">Datos de nivel de Agua</span>");
                                   client.println("<p><i class=\"fas fa-water\" style=\"color:#00add6;\"></i><span> Nivel: </span><span>"+niv2+" %</span></p></br>");
                      
                                   client.println("<span class=\"text\">Datos de movimiento</span>");
                                   client.println("<p><i class=\"fas fa-walking\"></i><span> Distancia: </span><span>"+dis2+" cm</span></p></br>");
                      
                                   client.println("<a href=\"192.168.0.12\" class=\"btn btn-info btn-block\">Actualizar</a>");
                        client.println("</div>");
                  client.println("</div>");
              client.println("</div>");
        client.println("</main>");
    client.println("</section>");
client.println("</div>");
              
             client.println("</body></html>");

             client.println();
             break;
          }else {
            currentLine = "";
          }
        }else if(c != '\r'){
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Cliente desconectado");
    Serial.println("");
  }    
}
