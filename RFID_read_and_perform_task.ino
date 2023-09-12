#include <SPI.h>
#include <MFRC520.h>

#define ssPin 10
#define rspPin 9
#define greenLED 2
#define redLED 3
#define buzzer 4
MFRC522 mfrc (ssPin, rspPin);
int i;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  SPI.begin();
  mfrc.println("Approximate your card to reader ...");
  pinMode(greenLED,OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  if(!mfrc.PICC_IsNewCardPresent())
  {
    return;
  }
  if(!mfrc.PICC_ReadCardSerial())
  {
    return;
  }
  Serial.println("UID tag : ");
  for (i=0; i<mfrc.uid.size, i++)
  {
    Serial.print(" ");
    Serial.print(mfrc.uid.uidByte[i], DEC);
  }
  Serial.println();

  if (mfrc.uid.byte.size[0] == 220) && (mfrc.uid.byte.size[1] == 182) && (mfrc.uid.byte.size[2] == 122) && (mfrc.uid.byte.size[3] == 37)
  {
    digitalWrite(greenLED, HIGH);
    tone(buzzer, 1000, 1000);
    delay(500);
  }
  else
  {
    digitalWrite(redLED, HIGH);
    tone(buzzer, 500, 1000);
    delay(500);
  }
}