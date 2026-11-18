
const int sensorChuvaDigital = 2;
const int sensorChuvaAnal = A0;
const int ledinhoV = 4;
const int ledinhoA = 9;
const int buzina = 3;
const int trig = 7;
const int echo = 8;

long duracao;
float distancia;

bool chovendo = false;
bool contandoTempo = false;
bool alertaEmitido = false;
bool alerta = false;

unsigned long inicioContagem = 0;

void setup() {
  Serial.begin(9600);

  pinMode(sensorChuvaDigital, INPUT);
  pinMode(ledinhoV, OUTPUT);
  pinMode(ledinhoA, OUTPUT);
  pinMode(buzina, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  int leituraDigital = digitalRead(sensorChuvaDigital);
  int anal = analogRead(sensorChuvaAnal);

  chovendo = (leituraDigital == 0);

  digitalWrite(ledinhoA, chovendo ? HIGH : LOW);

  digitalWrite(trig, LOW);
  delayMicroseconds(2);

  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duracao = pulseIn(echo, HIGH, 30000);

  if (duracao > 0) {
    distancia = duracao * 0.0343 / 2;
  } else {
    distancia = -1;
  }

  bool abaixoLimite = (duracao > 0 && distancia < 5);

  digitalWrite(ledinhoV, abaixoLimite ? HIGH : LOW);

if (abaixoLimite) {

  if (!contandoTempo) {
    inicioContagem = millis();
    contandoTempo = true;
  }

  if (millis() - inicioContagem >= 5000 && !alertaEmitido) {
    alerta = true;
    alertaEmitido = true;

    digitalWrite(buzina, HIGH);
  }

} else {
  contandoTempo = false;
  alertaEmitido = false;
  alerta = false;

  digitalWrite(ledinhoV, LOW);
  digitalWrite(buzina, LOW);
}


  // Envio dos dados para a Raspberry Pi
  Serial.print(chovendo ? 1 : 0);
  Serial.print(",");

  Serial.print(anal);
  Serial.print(",");

  Serial.print(distancia);
  Serial.print(",");

  Serial.println(alerta ? 1 : 0);

  delay(200);
}
