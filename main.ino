/*
  Avisador de chamadas do WhatsApp
  Arduino UNO + HC-05 + LCD 16x2 (sem I2C) + buzzer

  Recebe pelo Bluetooth linhas no formato:  CALL:Nome\n
  Mostra "Chamada WhatsApp" e o nome no LCD e toca o buzzer por 20 s.
  Letras acentuadas (UTF-8) são convertidas para a letra simples,
  porque o LCD 16x2 não exibe acentos.

  Ligações:
    HC-05 TX  -> pino 10
    HC-05 RX  <- pino 11 (com divisor de tensão 1k + 2k)
    Buzzer    -> pino 8
    LCD: RS=7, E=6, D4=5, D5=4, D6=3, D7=2
*/

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

    if (c == 0xC3) {              // primeiro byte de letra acentuada (UTF-8)
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
    if (agora - ultimoBip >= 1000) {       // bip a cada 1 segundo
      tone(BUZZER, 1000, 400);
      ultimoBip = agora;
    }
    if (agora - inicioChamada >= 20000) {  // para depois de 20 segundos
      emChamada = false;
      noTone(BUZZER);
      mostrarEspera();
    }
  }
}

// Converte o segundo byte de uma letra acentuada UTF-8 (após 0xC3)
// na letra simples correspondente.
char semAcento(int b) {
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
    lcd.print("Chamada WhatsApp");
    lcd.setCursor(0, 1);
    lcd.print(nome.substring(0, 16));
    emChamada = true;
    inicioChamada = millis();
    ultimoBip = millis() - 1000;  // força o primeiro bip na hora
  }
}

void mostrarEspera() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aguardando...");
}
