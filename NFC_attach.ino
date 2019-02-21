#include<SPI.h>
#include<MFRC522.h>
#include<Wire.h>
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h>
#define UID "5d6b6889" // 取得した識別子を記述
String Message;
String NUMBER;
int mode;//A0可変抵抗器、３モード設定
int pointDeduct = 1;//A1ポイント減算
int pointAdd = 1;//A2ポイント加算

constexpr uint8_t RST_PIN = 9;//RSTピンの指定
constexpr uint8_t SS_PIN = 10;//SSピンの指定
MFRC522 mfrc522(SS_PIN, RST_PIN);//RC522と接続
MFRC522::MIFARE_Key key;//認証キーの指定

Adafruit_SSD1306 display(-1);// ディスプレイ変数の宣言




void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);// ディスプレイの初期化
  while (!Serial);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_DumpVersionToSerial();
  Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  resetDisplay()
  printlnText("Nocard", 2);
  display.display();//ディスプレイ情報の更新


  pinMode(3, OUTPUT);//青
  pinMode(4, OUTPUT);//赤
  pinMode(A0, INPUT);//アナログ、モード設定
  pinMode(A1, INPUT_PULLUP);//ポイント-
  pinMode(A2, INPUT_PULLUP);//ポイント+
  pinMode(A3, INPUT_PULLUP); //送信


  Message = "No mode";
  NUMBER = "000";
}

void resetDisplay () {
  display.clearDisplay(); // 一度初期化
  display.setCursor(0, 0); // 一度初期化
  display.setTextColor(WHITE);
}

void printText (String text, int textSize) {
  display.setTextSize(textSize)
  display.print(text)
}

void printlnText (String text, int textSize) {
  display.setTextSize(textSize)
  display.println(text)
}

void loop() {
  digitalWrite(3, HIGH); //初期設定
  digitalWrite(4, LOW); //初期設定
  noTone(6);//初期設定

  if (
    ! mfrc522.PICC_IsNewCardPresent() || // Mifareカードのデータ読み込み
    ! mfrc522.PICC_ReadCardSerial()) { // Mifareカードの確認
      return; // どちらかを満たさない場合はloop関数を繰り返す
    }



  String strBuf[mfrc522.uid.size];
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    strBuf[i] =  String(mfrc522.uid.uidByte[i], HEX);
    if (strBuf[i].length() == 1) {
      strBuf[i] = "0" + strBuf[i];
    }
  }
  String strUID = strBuf[0] + strBuf[1] + strBuf[2] + strBuf[3];
  Serial.println(strUID);


  resetDisplay()

  if ( strUID == UID ) {
    printlnText("collect", 2);
    printlnText("cardUID = " + strUID, 1);
    display.display();

    tone(6, 1700);
    delay(30);
    noTone(6);
    delay(30);
    tone(6, 1700, 90);
    delay(500);

    blank(strUID);
  } else {
    printlnText("error!", 2);
    printlnText("cardUID = " + strUID, 2);
    display.display();

    Serial.println("else");
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    tone(6, 500);
    delay(1000);

    resetDisplay()
    printlnText("Nocard", 2);
    display.display();//ディスプレイ情報の更新
  }
}

void blank(String strUID) {
  int submit = 1;//A3送信


  while (submit == 0) {
    mode = analogRead(A0);
    pointDeduct = digitalRead(A1);
    pointAdd = digitalRead(A2);
    submit = digitalRead(A3);

    resetDisplay()
    printlnText(Message, 2);
    printText("NUMBER = ", 1);
    printlnText(NUMBER, 2);
    display.display();//ディスプレイ情報の更新
    delay(300);


    if ( mode <= 85 ) {
      Message = "Reset";
    } else if ( 85 < mode && mode < 170 ) {
      Message = "Point";
    } else if ( 170 <= mode ) {
      Message = "GameT";
    }


    if (pointDeduct == 0) {
      NUMBER -= 10;
    }

    if (pointAdd == 0) {
      NUMBER += 10;
    }

    if (submit == 0) {
      ESPserial(strUID,Message,NUMBER);
      break; 
    }
  }
}

int ESPserial ( String strUID, String Message, int NUMBER) {
  delay(1000);
}
