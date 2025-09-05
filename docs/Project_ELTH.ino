#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);     
SoftwareSerial bt(2, 3);                 

const int tempPin = A0;                 
const int ledBluePin = 6;               
const int ledYellowPin = 5;              
const int ledRedPin = 4;                 
const int buzzerPin = 7;                

bool useFahrenheit = false;              
String input = "";                       
float alertThreshold = 30.0;            
bool alertShown = false;                 

void setup() {
  bt.begin(9600);                        
  Serial.begin(9600);                    
  
  lcd.init();                            
  lcd.backlight();                      
  lcd.print("Initializing...");          
  delay(1000);                          
  lcd.clear();                          
  
  pinMode(ledBluePin, OUTPUT);
  pinMode(ledYellowPin, OUTPUT);
  pinMode(ledRedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
  digitalWrite(ledBluePin, LOW);
  digitalWrite(ledYellowPin, LOW);
  digitalWrite(ledRedPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void loop() {

  while (bt.available()) {
    char c = bt.read();          
    if (c == '\n') {             
      processCommand(input);     
      input = "";               
    } else {
      input += c;               
    }
  }

  
  float voltage = analogRead(tempPin) * 5.0 / 1023.0; 
  float tempC = voltage * 100.0;                      
  float tempDisplay = tempC;                          
  String unit = "C";                                  

 
  if (useFahrenheit) {
    tempDisplay = tempC * 9.0 / 5.0 + 32.0;
    unit = "F";
  }

  
  if (tempC > alertThreshold && !alertShown) {
    showAlert(tempC);  
    alertShown = true; 
  } 
  
  else if (tempC <= alertThreshold && alertShown) {
    removeAlert();    
    alertShown = false; 
  }

  
  lcd.setCursor(0, 0);               
  lcd.print("Temp: ");
  lcd.print(tempDisplay, 1);         
  lcd.print(" ");
  lcd.print(unit);
  lcd.print("     ");               

  bt.print("Temp: ");
  bt.print(tempDisplay, 1);
  bt.print(" ");
  bt.print(unit);
  bt.print("\n");

  
  if (tempC < 30.0) {
    digitalWrite(ledBluePin, HIGH);   
    digitalWrite(ledYellowPin, LOW);
    digitalWrite(ledRedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  } 
  else if (tempC >= 30.0 && tempC <= 32.5) {
    digitalWrite(ledBluePin, LOW);
    digitalWrite(ledYellowPin, HIGH);  
    digitalWrite(ledRedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  } 
  else if (tempC > 32.5) {
    digitalWrite(ledBluePin, LOW);
    digitalWrite(ledYellowPin, LOW);
    digitalWrite(ledRedPin, HIGH);     
    digitalWrite(buzzerPin, HIGH);
  }

  delay(1000);                       
}


void processCommand(String cmd) {
  cmd.trim();                          
  cmd.toUpperCase();                   
  if (cmd == "SET_C") {
    useFahrenheit = false;           
  } 
  else if (cmd == "SET_F") {
    useFahrenheit = true;             
  } 
}

void showAlert(float tempC) {
  bt.print("ALERT! Temperature too high: ");
  bt.print(tempC);
  bt.print(" C\n");
     
}

void removeAlert() {
    lcd.setCursor(0, 1);
    lcd.print("                  ");   
}
