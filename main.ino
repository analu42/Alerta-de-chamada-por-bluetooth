#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial bt(10, 11);            // RX, TX
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  // RS, E, D4, D5, D6, D7
const int BUZZER = 8;

String linha = "";
bool prefixoUtf = false;
bool emChamada = false;
unsigned long inicioChamada = 0;
unsigned long ultimoBip = 0;

void setup() {
  pinMode(BUZZER, OUTPUT);
  lcd.begin(16, 2);
  bt.begin(9600);
  mostrarEspera();
}

void loop() {
  while (bt.available()) {
    int c = bt.read();

    if (c == 0xC3) {              // primeiro byte de letra acentuada
      prefixoUtf = true;
      continue;
    }
    if (prefixoUtf) {             // segundo byte: troca pela letra simples
      linha += semAcento(c);
      prefixoUtf = false;
      continue;
    }

    if (c == '\n') {
      tratar(linha);
      linha = "";
    } else if (c != '\r') {
      linha += (char)c;
    }
  }

  if (emChamada) {
    unsigned long agora = millis();
    if (agora - ultimoBip >= 1000) {
      tone(BUZZER, 1000, 400);
      ultimoBip = agora;
    }
    if (agora - inicioChamada >= 20000) {
      emChamada = false;
      noTone(BUZZER);
      mostrarEspera();
    }
  }
}

char semAcento(int b) { //remove eventuais acentuações nos nomes 
  if (b >= 0x80 && b <= 0x85) return 'A';
  if (b == 0x87) return 'C';
  if (b >= 0x88 && b <= 0x8B) return 'E';
  if (b >= 0x8C && b <= 0x8F) return 'I';
  if (b == 0x91) return 'N';
  if (b >= 0x92 && b <= 0x96) return 'O';
  if (b >= 0x99 && b <= 0x9C) return 'U';
  if (b >= 0xA0 && b <= 0xA5) return 'a';
  if (b == 0xA7) return 'c';
  if (b >= 0xA8 && b <= 0xAB) return 'e';
  if (b >= 0xAC && b <= 0xAF) return 'i';
  if (b == 0xB1) return 'n';
  if (b >= 0xB2 && b <= 0xB6) return 'o';
  if (b >= 0xB9 && b <= 0xBC) return 'u';
  return '?';
}

void tratar(String msg) {
  msg.trim();
  if (msg.startsWith("CALL:")) {
    String nome = msg.substring(5);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Chamada ");
    lcd.setCursor(0, 1);
    lcd.print(nome.substring(0, 16));
    emChamada = true;
    inicioChamada = millis();
    ultimoBip = millis() - 1000;
  }
}

void mostrarEspera() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando...");
}
