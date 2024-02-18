#include <Servo.h>
#include <Fuzzy.h>
const int kyD = 12;
const int trigPin = 2;
const int echoPin = 3;
const int servoPin = 9;
int pos;
long waktu;

Servo s;
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput Sampah
FuzzySet *ada = new FuzzySet(0, 3, 3, 6);
FuzzySet *tidakAda = new FuzzySet(6, 20, 20, 100);

// FuzzyInput Ky
//FuzzySet *logam = new FuzzySet(20, 35, 35, 50);
//FuzzySet *bukanLogam = new FuzzySet(-5, 20, 20, 35);
FuzzySet *logam = new FuzzySet(1, 1, 1, 1);
FuzzySet *bukanLogam = new FuzzySet(0, 0, 0, 0);

// FuzzyOutput
FuzzySet *off = new FuzzySet(0, 0, 0, 0);
FuzzySet *kanan = new FuzzySet(1, 1, 1, 1);
FuzzySet *kiri = new FuzzySet(0.5, 0.5, 0.5, 0.5);

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(kyD, INPUT);
  s.attach(servoPin);
  konfigFuzzy();
  delay(1000);
}

void loop() {
  int jarak = bacaJarak();
  int logam = bacaLogam();
  float output = hasilFuzzy(jarak, logam);
  //debugging
  Serial.print(jarak);Serial.print("\t");
  Serial.print(logam);Serial.print("\t");
  Serial.print(output);Serial.println();
  delay(100);
}

int bacaLogam(){
  return digitalRead(kyD);
}

int bacaJarak(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  waktu = pulseIn(echoPin, HIGH);
  return waktu*0.034/2;
}

void keKanan(){
  pos = 30;
  s.write(pos);
  kembali(pos);
}

void keKiri(){
  pos = 150;
  s.write(pos);
  kembali(pos);
}

void kembali(int x){
  if(x == 150){
    for (pos = x; pos >= 90; pos -= 1) { 
    s.write(pos);
    delay(10);
    }
  } else if(x == 30){
    for (pos = x; pos <= 90; pos += 1) {
    s.write(pos);
    delay(10);
    }
  }
}

void konfigFuzzy(){
  FuzzyInput *sampah = new FuzzyInput(1);
    sampah->addFuzzySet(ada);
    sampah->addFuzzySet(tidakAda);
  fuzzy->addFuzzyInput(sampah);

  FuzzyInput *ky = new FuzzyInput(2);
    ky->addFuzzySet(logam);
    ky->addFuzzySet(bukanLogam);
  fuzzy->addFuzzyInput(ky);

  FuzzyOutput *servo = new FuzzyOutput(1);
    servo->addFuzzySet(off);
    servo->addFuzzySet(kanan);
    servo->addFuzzySet(kiri);
  fuzzy->addFuzzyOutput(servo);

  FuzzyRuleAntecedent *ifAdaAndLogam = new FuzzyRuleAntecedent();
  ifAdaAndLogam->joinWithAND(ada, logam);
  FuzzyRuleConsequent *thenKanan = new FuzzyRuleConsequent();
  thenKanan->addOutput(kanan);
  FuzzyRule *fuzzyRule01 = new FuzzyRule(1, ifAdaAndLogam, thenKanan);
  fuzzy->addFuzzyRule(fuzzyRule01);

  FuzzyRuleAntecedent *ifAdaAndBukanLogam = new FuzzyRuleAntecedent();
  ifAdaAndBukanLogam->joinWithAND(ada, bukanLogam);
  FuzzyRuleConsequent *thenKiri = new FuzzyRuleConsequent();
  thenKiri->addOutput(kiri);
  FuzzyRule *fuzzyRule02 = new FuzzyRule(2, ifAdaAndBukanLogam, thenKiri);
  fuzzy->addFuzzyRule(fuzzyRule02);

  FuzzyRuleAntecedent *ifTidakAda = new FuzzyRuleAntecedent();
  ifTidakAda->joinSingle(tidakAda);
  FuzzyRuleConsequent *thenOff = new FuzzyRuleConsequent();
  thenOff->addOutput(off);
  FuzzyRule *fuzzyRule03 = new FuzzyRule(3, ifTidakAda, thenOff);
  fuzzy->addFuzzyRule(fuzzyRule03); 
}

float hasilFuzzy(int x, int y){
  fuzzy->setInput(1, x);
  fuzzy->setInput(2, y);
  fuzzy->fuzzify();
  float output = fuzzy->defuzzify(1);
  if(output == 1){
    keKanan(); return output;
  } else if(output == 0.5){
    keKiri(); return output;
  } else if (output == 0){
    return output;
  }
}