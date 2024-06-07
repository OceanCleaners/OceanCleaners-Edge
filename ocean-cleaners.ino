#include <LiquidCrystal.h>

#define LED_OK 4
#define LED_ALERTA 3
#define LED_PROBLEMA 2

#define LDR_SENSOR A0
#define TMP_SENSOR A1
#define HMD_POTENCIOMETRO A2
#define BUZZER 5

#define TEMPO_ATUALIZACAO 5000

// LCD 
LiquidCrystal lcd(
  13, // Register Select 
  12, // Leitura e Escrita
  11, // Enable
  10, // Pin de dados 4
  9, // Pin de dados 5
  8, // Pin de dados 6
  7 // Pin de dados 7
);

float turbidez = 0;
float ph = 0;
float temperatura = 0;

long ultimaAtualizacaoLCD = 0;
int etapaAtualIdx = 0;
String etapas[] = {
  "turbidez",
  "temperatura",
  "ph"
};

void setup() {
  lcd.begin(16, 2);
  pinMode(LED_OK, OUTPUT);
  pinMode(LED_ALERTA, OUTPUT);
  pinMode(LED_PROBLEMA, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(HMD_POTENCIOMETRO, INPUT);

  Serial.begin(9600);
}

void loop() {
  long agora = millis();

  temperatura = lerTemperatura();
  turbidez = lerTurbidez();
  ph = lerPh();

  if (agora - ultimaAtualizacaoLCD >= TEMPO_ATUALIZACAO) {
    ultimaAtualizacaoLCD = agora;

    String etapaAtual = etapas[etapaAtualIdx];
    Serial.print("Etapa: ");
    Serial.println(etapaAtual);

    switch (etapaAtualIdx) {
    case 0:
      etapaAtualIdx += 1;
      atualizarDisplayTurb();
      break;
    case 1:
      etapaAtualIdx += 1;
      atualizarDisplayTemp();
      break;
    case 2:
      etapaAtualIdx = 0;
      atualizarDisplayPh();
      break;
    }
  }

  // LED OK
  if (turbidez > 800) {
    digitalWrite(LED_OK, HIGH);

    // Reset
    resetarPadroes(LED_ALERTA, LED_PROBLEMA, true);
  }

  // LED Alerta
  if (
    (turbidez >= 300 && turbidez <= 600) ||
    (temperatura > 28 || temperatura < 24)
  ) {
    digitalWrite(LED_ALERTA, HIGH);

    if (temperatura < 10 || temperatura > 15) {
      tone(BUZZER, 1000);
    } else noTone(BUZZER);

    // Reset
    resetarPadroes(LED_OK, LED_PROBLEMA, false);
  }

  // LED Problema  
  if (
    turbidez < 300 ||
    (ph < 7 || ph > 8.5)
  ) {
    digitalWrite(LED_PROBLEMA, HIGH);
    tone(BUZZER, 1000);

    // Reset
    resetarPadroes(LED_OK, LED_ALERTA, false);
  }
}

void atualizarDisplayTurb() {
  if (
    (turbidez <= 300)
  ) {
    lcd.clear();
    lcd.print("Turbidez Alta");
    lcd.setCursor(0, 1);
    lcd.print("Turbi = ");
    lcd.print(turbidez);
  } else if (
    turbidez >= 300 && turbidez <= 600
  ) {
    lcd.clear();
    lcd.print("Turbidez Mod.");
    lcd.setCursor(0, 1);
    lcd.print("Turbi = ");
    lcd.print(turbidez);
  } else {
    lcd.clear();
    lcd.print("Turbidez OK");
    lcd.setCursor(0, 1);
    lcd.print("Turbi = ");
    lcd.print(turbidez);
  }
}

void atualizarDisplayTemp() {
  if (temperatura > 28) {
    lcd.clear();
    lcd.print("Temp. Alta");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  } else if (temperatura < 24) {
    lcd.clear();
    lcd.print("Temp. Baixa");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  } else {
    lcd.clear();
    lcd.print("Temp. OK");
    lcd.setCursor(0, 1);
    lcd.print("Temp. = ");
    lcd.print(temperatura);
    lcd.println("C");
  }
}
void atualizarDisplayPh() {
  if (ph < 7) {
    lcd.clear();
    lcd.print("pH Baixo");
    lcd.setCursor(0, 1);
    lcd.print("pH = ");
    lcd.print(ph);
  } else if (ph > 8) {
    lcd.clear();
    lcd.print("pH Alto");
    lcd.setCursor(0, 1);
    lcd.print("pH = ");
    lcd.print(ph);
  } else {
    lcd.clear();
    lcd.print("pH OK");
    lcd.setCursor(0, 1);
    lcd.print("pH = ");
    lcd.print(ph);
  }
}

float lerTurbidez() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    turbidez = analogRead(LDR_SENSOR);
    turbidez = map(turbidez, 54, 974, 0, 900);

    total += turbidez;
    delay(10);
  }
  return total / 5;
}

float lerTemperatura() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    // Temperatura [-∞...∞]
    temperatura = (analogRead(TMP_SENSOR) / 1023.0) * 5000;
    temperatura = (temperatura - 500) * 0.1;

    total += temperatura;
    delay(10);
  }
  return total / 5;
}

float lerPh() {
  int total = 0;
  for (int i = 0; i < 5; i++) {
    // pH [1...14]
    ph = analogRead(HMD_POTENCIOMETRO);
    ph = map(ph, 0, 1023, 1, 14);

    total += ph;
    delay(10);
  }
  return total / 5;
}

void resetarPadroes(int led1, int led2, bool resetBuzzer) {
  if (resetBuzzer) noTone(BUZZER);

  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);
}