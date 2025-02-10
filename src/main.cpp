/***************************************************************************
 *************************************************************************** 
 ** Prueba de conexion con un servidor MQTT utilizando una ESP32 Devkit V1**  
 ** Oscar Alberto Valles Limas                                            **
 ** Instituto tecnológico de Chihuahua                                    **
 ***************************************************************************
 ***************************************************************************
*/
#include <Arduino.h> //Libreria que agrega automáticamente platform.io

#include <WiFi.h> //Libreria para utilizar red en la ESP32 
#include <PubSubClient.h> //Libreria para la conexión con HiveMQ
#include <WiFiClientSecure.h> //Libreria para conexión a redes de internet 

const char* ssid = "IZZI-7E6C"; //Nombre de la red a conectarse 
const char* password = "50A5DC507E6C"; //Password de la red 

const char* mqtt_server = "3baa2ee357f2493dacd65f0f254ac39f.s1.eu.hivemq.cloud"; //URL de HiveMq
const int mqtt_port = 8883; //Puerto, por lo general ese es el predeterminado 
const char* mqtt_username = "Prueba"; //Username de la credencial 
const char* mqtt_password = "Itch1234"; //Password de la credencial 

// Definición de los canales a utilizar
const char* CONTROL_LED_TOPIC = "control-led"; //Nombre del topico que se va a utilizar en Hivemq para controlar el encendido de un led 

const int ledPin = 5;
//------------------------------------------------------------------------------------------
//Certificado para poder utilizar Hivemq, no modificar 
static const char* root_ca PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U
A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW
T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH
B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC
B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv
KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn
OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn
jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw
qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI
rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV
HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq
hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL
ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ
3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK
NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5
ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur
TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC
jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc
oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq
4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA
mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d
emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
char msg[50];
//-------------------------------------------------------------------------------
void setup_wifi() { //Configuracion del wifi 
  delay(10); //Un retraso de 10 milisegundos 
  Serial.println(); //Se imprime un espacio
  Serial.print("Connecting to "); //Se imprime un mensaje 
  Serial.println(ssid); //Se imprime el nombre de la red 
  WiFi.begin(ssid, password); //Se inicia la coneccion con la red
  while (WiFi.status() != WL_CONNECTED) { //While en la cual mientras se conecta a la red aparecen puntos de carga 
    delay(500); //Retardo de medio segundo 
    Serial.print("."); //Imprecion de puntos de carga 
  } //Final del while 
  Serial.println(""); //Se imprime una linea de espacio 
  Serial.println("WiFi connected"); //Aparece un mensaje una ves que la conexion se establece 
  Serial.println("IP address: "); //Se imprime un mensaje mostrando que lo siguiente sera una IP 
  Serial.println(WiFi.localIP()); //Se imprime la direccion IP
}

void callback(char* topic, byte* payload, unsigned int length) { //Funcion para impresion de las publicaciones 
  Serial.print("Mensaje recibido en el canal: "); //Impresion de mensaje
  Serial.println(topic); //Impresion de mensaje 

  if (strcmp(topic, CONTROL_LED_TOPIC) == 0) { //Condicional if para mostrar el estado del led 
    int estado = payload[0] - '0'; //Estado general 0 (Apagado\)
    digitalWrite(ledPin, estado); //Se lee el estado del led 
    Serial.print("Estado del LED: "); //Impresion de mensaje 
    Serial.println(estado); //Se imprimie el estado del led =  0 (Apagado), 1 (Encendido)
  } 
}

void reconnect() { //Funcion en caso de que se pierda la conxion con el servidor 
  while (!client.connected()) { //Mientras no exista conexion con el servidor va a realizar lo siguiente 
    Serial.print("Attempting MQTT connection..."); //Impresion de mensaje 
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) { //Funcion en la cual se intentan poner las credenciales de nuevo 
      Serial.println("connected"); //Mensaje de conexion 
      client.subscribe(CONTROL_LED_TOPIC); //Se vuelve a suscribir los topicos que se definieron con anterioridad 
    } else { //En caso de que no se establezca la conexion correctamente 
      Serial.print("failed, rc="); //Impresion de mensaje de error
      Serial.print(client.state()); //Impresion del estado de la conexion 
      Serial.println(" try again in 5 seconds"); //Se imprime un mensaje 
      delay(5000); //Retraso de 5 segundos para que vuelva a intentar una conexion 
    }
  }
}

void setup() { //Definicion de pines y funciones 
  pinMode(ledPin, OUTPUT); //Se define el pin del led como una salida
  Serial.begin(9600); //Baud rate 
  setup_wifi(); //Se llama o se invoca la funcion de wifi para su conexion 
  espClient.setCACert(root_ca); //Fimcopm de super su 
  client.setServer(mqtt_server, mqtt_port); //Funcion para definir o configurar la conexion con el servidor mqtt
  client.setCallback(callback); //Funcion del call back para imprimir todos los mensajes necesarios 
}

void loop() { //Definicion del loop para que re intente la conexion las veces que sean necesarias  
  if (!client.connected()) { //Funcion para la reconexion invocando la funcion que se configuro previamente 
    reconnect(); //Funcion reconectar 
  }
  client.loop(); //Apuntador al loop para que lo haga de manera infinita hasta que se conecte
}