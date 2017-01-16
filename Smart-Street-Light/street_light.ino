int in1 = 2;
int in2 = 3;
int in3 = 4;
int out1 = 6;
int out2 = 7;
int out3 = 8;
void setup() {
  // put your setup code here, to run once:
  pinMode(in1, INPUT);
  pinMode(in2, INPUT);
  pinMode(in3, INPUT);
  pinMode(out1, OUTPUT);
  pinMode(out2, OUTPUT);
  pinMode(out3, OUTPUT);
  digitalWrite(out1,LOW);
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(digitalRead(in1) == LOW){
    digitalWrite(out1, HIGH);
    delay(2000);
  }
  digitalWrite(out1, LOW);

  while(digitalRead(in2)==LOW){
    digitalWrite(out2, HIGH);
    delay(2000);
  }
  digitalWrite(out2, LOW);

  while(digitalRead(in3) == LOW){
    digitalWrite(out3, HIGH);
    delay(2000);
  }

  digitalWrite(out1, LOW);
  digitalWrite(out2, LOW);
  digitalWrite(out3, LOW);
}
