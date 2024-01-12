#include <SPI.h>             // import SPI
#include <MFRC522.h>         // import RFID library
#include <EEPROM.h>          // import EEPROM library
#include <BlinkControl.h>    // import Blink Control
#include <SoftwareSerial.h>  // import Software Serial
#include <SerialTransfer.h>  // import Serial Transfert (for transferring struct i guess)

#define RST_PIN 9     // Reset pin on RFID
#define SS_PIN 10     // SS pin on RFID
#define LED_ORANGE 7  // LED orange pin
#define LED_RED 8     // LED red pin
#define LED_BLUE 6    // LED blue pin
#define LED_GREEN 4   // LED green pin
#define BUZZER 5      // Buzzer pin

// An RFID Object
MFRC522 mfrc522(SS_PIN, RST_PIN);
String latest = "";  // Latest RFID Uiid

bool eepromIsEmpty;           // Hold state of EEPROM emptyness
bool isInitializing = true,   // Hold the state of initialization
  espIsConnected = false,     // Hold ESP connection state
  addNewCredentials = false;  // Hold the state of new credential addings

// Custom strucutre for wifi credentials
struct InformationWifi {
  char ssid[20];
  char pass[20];
};

char counter = 0;

SoftwareSerial espSerial(2, 3);  // RX,TX // Normal Software serial

void setup() {

  /* DEBUT INITIALISATION*/

  //pin modes
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_ORANGE, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  // Initializing serial
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  SPI.begin();  // Begin SPI communication for RFID

  mfrc522.PCD_Init();  // Initialize RFID

  espSerial.begin(9600);  // SoftwareSerial

  /*END INITIALISATION*/
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);

  //ProvideCredentialToESP8266();
}

void loop() {
  /*
  * Ecouter occasionellement le port Serial au cas où on veux ajouter de nouveaux WiFi
  * Si le donnée réçu par le RX est égal à 'n' alors ça veut dire que l'utilisateur veut ajouter
  * de nouvelles informations de Wifi.
  */
  /*if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\r');
    Serial.print("Data available en loop: ");
    Serial.println(data);
    if (data == "n\n") {
      Serial.println("DATA is n");
      addNewCredentials = true;  // Got data from Serial and this data is 'n' meaning there is activity on the computer side so activate new credential mode
    }
  }*/

  // Read data from the ESP8266
  if (espSerial.available() > 0) {
    String giot = espSerial.readStringUntil('\r');
    espIsConnected = true;
    digitalWrite(LED_GREEN,HIGH);
    Serial.print("giot: ");
    Serial.println(giot);
  }
  if (espIsConnected) {
    readRFID();
  }


  /*
  * Ici le programme entre mode initialisation
  * C'est la partie où le programme vérifie si le EEPROm contient les informations du Wifi 
  * sur lequel doit se connecter l'esp8266.
  * 
  * Il y a deux cas, celui de l'EEPROM vide et celui de l'EEPROM non vide mais l'utilisateur veut changer de Wifi
  */
  /*
  if(isInitializing){ // Si le programme entre en mode initialisation

    digitalWrite(LED_RED,LOW);
    espIsConnected = false;
    struct InformationWifi datas; // Struct pour stocker celui retiré de l'EEPROM
    int address = 0;// Adresse de lecture
    
    eepromIsEmpty = checkEmptyEEPROM();
    // Vérifier si l'EEPROM est vide.

    if(eepromIsEmpty || addNewCredentials){ // Si l'EEPROM est vide où si on veut changer de WiFi

      while(!Serial.available()){
        digitalWrite(LED_BLUE,!digitalRead(LED_BLUE));
        delay(100);
      }
      
      String data = Serial.readStringUntil('\r');
      digitalWrite(LED_BLUE,LOW);

      //Split String here
      String voloany = getValue(data,'!',0);
      String faharoa = getValue(data,'!',1);

      char dutu[20];
      char duta[20];
      voloany.toCharArray(dutu,20);
      faharoa.toCharArray(duta,20);
      
      //Ajouter les données dans un struct
      struct InformationWifi infoWifi;
      strcpy(infoWifi.ssid,dutu);
      strcpy(infoWifi.pass,duta);

      Serial.print("Première partie: ");Serial.println(infoWifi.ssid);
      Serial.print("Deuxième partie: ");Serial.println(infoWifi.pass);

      // ----------- Tafiditra ao am struct ny credentials zay ---------------------------

      if(addNewCredentials && !eepromIsEmpty){ 

        Serial.println("Wifi Modification asked");
        
        clearEEPROM();
        Serial.println("EEPROM cleared");
        addNewCredentials = false; // Ceci annule la modification de WiFi

      } 
      
      else if(eepromIsEmpty){
        Serial.println("EEPROM is empty");
      }

      // Ajouter le struct dans l'EEPROM ici.
      EEPROM.put(0, infoWifi);
      
    }

    // Recupérer les informations dans l'EEPROM car à ce stade, l'EEPROM ne doit pas être vide
    EEPROM.get(address, datas);
  
    /*Serial.print("Première partie: ");Serial.println(datas.ssid);
    Serial.print("Deuxième partie: ");Serial.println(datas.pass);

    //Concat credentials into One String
    String credentials = String(datas.ssid)+String("!")+String(datas.pass);

    Serial.print("Concatenated: ");Serial.println(credentials);

    while(!Serial.available()){
      digitalWrite(LED_GREEN,!digitalRead(LED_GREEN));
      delay(300);
    }

    // Envoyer vers l'esp8266
    if(containsChar(credentials,'!')){
        espSerial.print(credentials);
    }
    
    // Blinker le LED vert en attendant que l'esp se connecte au Wifi
    while(!espSerial.available()){
      digitalWrite(LED_GREEN,!digitalRead(LED_GREEN));
      delay(100);
    }
    String fromEsp = espSerial.readStringUntil('\r');
    Serial.println(fromEsp);
    
    espIsConnected  = true; // L'esp8266 est connecté
    isInitializing = false;
    Serial.println("Tonga am farany");

  }else if(espIsConnected){

    if(addNewCredentials){
      isInitializing = true;
      return;
    }else{
      //readRFID();
      counter += 1;
      digitalWrite(LED_RED,!digitalRead(LED_RED));
      espSerial.println(counter);
      delay(300);
    }

  }*/
  //
}

void readRFID() {  // Fontion qui capture l'UUID d'une carte RFID

  // Check if there is a new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Check if the card is readable
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  String content = "";  // Hold the content of the new card
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();  // Uppercase the RFID card ID read
  if (content.equals(latest)) {
    // The card has already been read
    // Do something about it,like blink a red LED twice followed by an annoying buzz signal (or something like that).
  } else {
    latest = content;  // Replace the old value with the new one
    blinky();          // Blink orange LED
    Serial.println(latest);
    espSerial.print(latest);  // Send the value to the esp8266
  }
}

void blinky() {  // Fonction pour blinker un LED

  digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE));
  digitalWrite(BUZZER, !digitalRead(BUZZER));
  delay(50);
  digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE));
  digitalWrite(BUZZER, !digitalRead(BUZZER));
  delay(50);
  digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE));
  digitalWrite(BUZZER, !digitalRead(BUZZER));
  delay(50);
  digitalWrite(LED_ORANGE, !digitalRead(LED_ORANGE));
  digitalWrite(BUZZER, !digitalRead(BUZZER));
  delay(50);
}

bool checkEmptyEEPROM() {  // Fonction pour vérifier si l'EEPROM est vide.

  bool isEmpty = false;  // Hold EEPROM state
  int data;              // Hold data
  int zeroCounter = 0;   // Hold the number of Empty slots in the EEPROM memory

  // Iterate through the first 8 slots of the EEPROM to count the empty ones
  for (int i = 0; i < 8; i++) {
    data = EEPROM.read(i);
    if (data == 0) zeroCounter += 1;
    if (zeroCounter == 8) isEmpty = true;
  }

  return isEmpty;
}

String getValue(String data, char separator, int index) {  // Séparer un String
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void clearEEPROM() {
  for (int i = 0; i < EEPROM.length(); i++) {
    EEPROM.update(i, 0);
  }
}

bool containsChar(String str, char i) {
  return str.indexOf(i) != -1;
}
