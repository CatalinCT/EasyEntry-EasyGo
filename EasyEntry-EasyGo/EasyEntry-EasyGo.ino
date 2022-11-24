/*RST/Reset   RST          D9
 * SPI SS      SDA(SS)      D10
 * SPI MOSI    MOSI         D11
 * SPI MISO    MISO         D12
 * SPI SCK     SCK          D13*/
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

#define RST_PIN 9  // Configurable, see typical pin layout above
#define SS_PIN 10  // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

String read_rfid;
int numbers_of_cards = 2;
String service_card = "201cf12a";
String cards[2][20] = { "651e921",
                        "46d322b334b80" };
bool lock_stat = false;            //false - usi deblocate  true - usi blocate
bool engine_stat = true;           //false - motor oprit  true - motor pornit
const int face_input_ok = A0;      //input from Raspberry Pi face ok
const int face_input_not_ok = A1;  //input from Raspberry Pi face not ok
const int face_trig = 8;           //output to Raspberry Pi face_req start
const int lock = 2;
const int engine = 4;
const int hazard_lights = 3;
/*********************************************************************************************************************/
/*
 * Initializare.
 */
void setup() {
  Serial.begin(9600);
  //while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("EasyEntry-EasyGo Starting...");
  delay(500);
  Serial.println("Usile sunt DEBLOCATE");
  Serial.println("MOTORUL este ACTIV");
  delay(500);
  Serial.println("Se asteapta card...");

  //Choose which lock below:
  pinMode(face_trig, OUTPUT);
  digitalWrite(face_trig, LOW);
  pinMode(lock, OUTPUT);
  digitalWrite(lock, HIGH);
  pinMode(engine, OUTPUT);
  digitalWrite(engine, HIGH);
}

/*
 * Salveaza ID card citit intr-un string
 */
void read_card(byte *buffer, byte bufferSize) {
  read_rfid = "";
  for (byte i = 0; i < bufferSize; i++) {
    read_rfid = read_rfid + String(buffer[i], HEX);
  }
}

/*
 * Compara ID card citit cu cardurile salvate local
 */
bool compare(String read_rfid, String array[]) {
  int aux = 0;
  for (int i = 0; i < numbers_of_cards; i++) {
    if (read_rfid == array[i]) {
      aux++;
    }
  }

  if (aux > 0) {
    Serial.println("Card verificat OK");
    return true;
  }

  else {
    Serial.println("Card verificat NOT_OK");
    delay(1500);
    return false;
  }
}

/*
 * Incuiere/Descuiere usi
 */
void locking() {
  if (lock_stat == false) {
    digitalWrite(lock, LOW);
    delay(500);
    digitalWrite(lock, HIGH);
    lock_stat = true;
    Serial.println("Usi Blocate!");
    return;
  }
  if (lock_stat == true) {
    digitalWrite(lock, LOW);
    delay(500);
    digitalWrite(lock, HIGH);
    lock_stat = false;
    Serial.println("Usi Deblocate");
    return;
  }
}

/*
 * Verificare Face ID
 */
bool face_check() {
  if (lock_stat == true) {
    return false;
  }
  digitalWrite(face_trig, HIGH);
  int aux = 0;
  int value_ok = analogRead(face_input_ok);
  int value_not_ok = analogRead(face_input_not_ok);
  Serial.println("Waiting Face ID");
  Serial.print("input ok:");
  Serial.println(value_ok);
  Serial.print("input not ok:");
  Serial.println(value_not_ok);
  delay(1000);

  if (value_ok > 1000) {
    Serial.println("Face ID OK");
    digitalWrite(face_trig, LOW);
    return true;
  }
  if (value_not_ok > 1000) {
    Serial.println("Face ID NOT_OK");
    digitalWrite(face_trig, LOW);
    return false;
  }

  while (value_ok < 1000 && value_not_ok < 1000 && aux < 1000) {
    value_ok = analogRead(face_input_ok);
    value_not_ok = analogRead(face_input_not_ok);
    Serial.println("Waiting Face ID");
    Serial.print("input ok:");
    Serial.println(value_ok);
    Serial.print("input not ok:");
    Serial.println(value_not_ok);
    delay(1000);
    if (value_ok > 1000) {
      Serial.println("Face ID OK");
      digitalWrite(face_trig, LOW);
      return true;
    }
    if (value_not_ok > 1000) {
      Serial.println("Face ID NOT_OK");
      digitalWrite(face_trig, LOW);
      return false;
    }
    aux++;
    if (!mfrc522.PICC_ReadCardSerial())
      break;
  }

  Serial.println("Face ID ERROR");
  digitalWrite(face_trig, LOW);
  return false;
}

/*
 * Activare/dezactivare motor
 */
void engine_locking() {
  if (lock_stat == true && engine_stat == true) {
    digitalWrite(engine, LOW);
    engine_stat = false;
    Serial.println("Motor Blocat/Oprit!");
    return;
  }
  if (lock_stat == true && engine_stat == false) {
    Serial.println("Motor deja oprit");
    return;
  }
  if (lock_stat == false && engine_stat == false) {
    bool value = face_check();
    if (value == true) {
      digitalWrite(engine, HIGH);
      engine_stat = true;
      Serial.println("Motor Pornit!");
      return;
    }
    if (value == false) {
      digitalWrite(engine, LOW);
      engine_stat = false;
      Serial.println("Motor ramane Oprit!");
      return;
    }
  }
}

void engine_locking_service() {
  if (lock_stat == true && engine_stat == true) {
    digitalWrite(engine, LOW);
    engine_stat = false;
    Serial.println("Motor Blocat/Oprit!");
    return;
  }
  if (lock_stat == true && engine_stat == false) {
    Serial.println("Motor deja oprit");
    return;
  }
  if (lock_stat == false && engine_stat == false) {

    digitalWrite(engine, HIGH);
    engine_stat = true;
    Serial.println("SERVICE MODE!");
    return;
  }
}



void loop() {

  // Cauta carduri noi
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  // Citeste card
  if (!mfrc522.PICC_ReadCardSerial())
    return;

  read_card(mfrc522.uid.uidByte, mfrc522.uid.size);  //conversie si salvare ID card
  Serial.println(read_rfid);                         //afiseaza ID card
  
  if (read_rfid == service_card) {
    locking();
    engine_locking_service();
  }

  if (compare(read_rfid, *cards) == true) {
    //Serial.println("Incepere rutină");
    locking();
    engine_locking();
  }
}