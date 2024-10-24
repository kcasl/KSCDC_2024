void setup() {

  Serial.begin(9600);

}

 

void loop() {

  int val_0 = analogRead(A0);

  int val_1 = analogRead(A1);

  Serial.print("VRx : ");

  Serial.print(val_0);

  Serial.print("    ");

  Serial.print("VRy : ");

  Serial.println(val_1);

  delay(200);

}