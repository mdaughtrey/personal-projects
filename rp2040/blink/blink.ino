

void setup() {
    Serial.begin(115200);
	pinMode(25, OUTPUT);
}

void loop() {
    Serial.println("Sure, whatever.");
	digitalWrite(25, HIGH);
	delay(100);
	digitalWrite(25, LOW);
	delay(100);
}
