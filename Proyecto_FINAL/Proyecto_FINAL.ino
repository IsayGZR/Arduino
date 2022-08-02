#include <Adafruit_PN532.h>
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h> 
LiquidCrystal_I2C lcd(0x27, 20, 4); 
Servo esc;
#define PN532_SCK  (9)
#define PN532_MOSI (10)
#define PN532_SS   (11)
#define PN532_MISO (12)
#define PN532_IRQ   (7)
#define PN532_RESET (2)  
int relevador =13;
int pulsador1 =6;
int pulsador2 =5;
int boton = 4;
int RED =14;
int GREEN =15;
int BLUE =16;
int velocidad=60;
int aumento = 1;
int puertas = 1;
int trampilla=0;
volatile float vel=0;
volatile int contador =0;
volatile float km=0;
volatile float kmR=0;
//Interupcion 0 en el Pin 2 Arduino Mega 
Adafruit_PN532 lector1(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);
Adafruit_PN532 lector2(PN532_IRQ, PN532_RESET);

void setup() {
  lector1.begin();
  lector2.begin();
  lector1.SAMConfig();
  lector2.SAMConfig();
  esc.attach(8); 
  pinMode(pulsador1,INPUT);
  pinMode(pulsador2,INPUT);
  pinMode(boton, INPUT);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(relevador,OUTPUT);
  esc.writeMicroseconds(1000);
  attachInterrupt(1,interrupcion1,FALLING);
}
void loop() {
  if(digitalRead(boton)== LOW){
      aumento++;
  }
  delay(200);
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength; 
  uint8_t success1;
  uint8_t uid1[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength1; 
  success = lector1.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  success1 = lector2.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid1, &uidLength1);
  uint32_t nfc1 = 0;
  uint32_t nfc2 = 0;
   if (success) {
    nfc1 = uid[3];
    nfc1 <<= 8; nfc1 |= uid[2];
    nfc1 <<= 8; nfc1 |= uid[1];
    nfc1 <<= 8; nfc1 |= uid[0];
  }
  if(success1){
    nfc2 = uid1[3];
    nfc2 <<= 8; nfc2 |= uid1[2];
    nfc2 <<= 8; nfc2 |= uid1[1];
    nfc2 <<= 8; nfc2 |= uid1[0];
  }
  if(nfc2 == 2984024409){
    puertas++;
  }
  delay(200);
  if(puertas%2 == 0){
    digitalWrite(relevador,HIGH);
    lcd.noDisplay();
    lcd.noBacklight();
    lcd.clear();
  }
  if(puertas%2 ==1){
    digitalWrite(relevador,LOW);
    lcd.clear();
  }
  if(nfc1 == 2451697515){
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);  
    lcd.print("Principal");
    lcd.setCursor(0, 1);
    lcd.print("RPM:");
    lcd.setCursor(0, 2);
    lcd.print("Vel:"); 
    lcd.setCursor(0,3);     
    digitalWrite(relevador, HIGH);
    digitalWrite(GREEN, LOW);
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, HIGH);
    trampilla=0;
  }
  //Invitado
  if(nfc1 == 1646521959){
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);  
    lcd.print("Invitado");
    lcd.setCursor(0, 1);
    lcd.print("RPM:");
    lcd.setCursor(0, 2);
    lcd.print("Vel:"); 
    lcd.setCursor(0,3);     
    digitalWrite(relevador, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, HIGH);
    trampilla=0;
  }
  //Parking
  if(nfc1 == 1646308887){
    lcd.init();
    lcd.backlight(); 
    lcd.setCursor(0,0); 
    lcd.print("Parking");
    lcd.setCursor(0, 1);
    lcd.print("RPM:");
    lcd.setCursor(0, 2);
    lcd.print("Vel:"); 
    lcd.setCursor(0,3);
    digitalWrite(relevador, HIGH);
    digitalWrite(GREEN, HIGH);
    digitalWrite(RED, HIGH);
    digitalWrite(BLUE, LOW);
    kmR=0;
    km=0;
    trampilla=0;
  }
  //Principal
  while(nfc1 == 2451697515){
    if(digitalRead(boton)== LOW){
      aumento++;
    }
     if(aumento%2 == 0){
      while(nfc1 == 2451697515){ 
        if(digitalRead(pulsador1) == LOW){
          velocidad++; 
          trampilla=200+trampilla;   
            if (velocidad >= 160){
              velocidad = 160;  
            }
        }
        if(digitalRead(pulsador2) == LOW){
          velocidad--; 
          trampilla=trampilla-200; 
            if (velocidad <= 60){
              velocidad = 60;  
            } 
        }
        if(trampilla>=20000){
          trampilla=20000;
        }
        if(trampilla <=0){
          trampilla=0;
        }
        esc.write(velocidad);
        delay(150);
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Principal"); 
        lcd.setCursor(0, 1);
        lcd.print("RPM:");
        lcd.setCursor(0, 2);
        lcd.print("Vel:");       
        lcd.setCursor(4, 1);
        lcd.print(trampilla);
        vel = trampilla*.008; 
        lcd.setCursor(4, 2);
        lcd.print(vel);
        lcd.print("Kh/h"); 
        lcd.setCursor(0,3);
        if(digitalRead(boton) == LOW){
          digitalWrite(relevador,LOW);
          lcd.clear();
          break;
        }
      }
      if(digitalRead(boton)== LOW){
        digitalWrite(relevador,LOW); 
        lcd.clear(); 
        break;
      }
    }
    delay(150);
  }
  //Invitado
  while(nfc1 == 1646521959){
    if(digitalRead(boton)== LOW){
      aumento++;
    }
     if(aumento%2 == 0){
      while(nfc1 == 1646521959){ 
        if(digitalRead(pulsador1) == LOW){
          velocidad++; 
          trampilla=200+trampilla;   
            if (velocidad >= 123){
              velocidad = 123;  
            }
        }
        if(digitalRead(pulsador2) == LOW){
          velocidad--;  
          trampilla=trampilla-200;
            if (velocidad <= 60){
              velocidad = 60;  
            } 
        }
        if(trampilla>=12600){
          trampilla=12600;
        }
        if(trampilla <=0){
          trampilla=0;
        }
        esc.write(velocidad);
        delay(150); 
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Invitado");
        lcd.setCursor(0,1);
        lcd.print("RPM:");
        lcd.setCursor(0, 2);
        lcd.print("Vel:");       
        lcd.setCursor(4, 1);
        lcd.print(trampilla);
        vel = trampilla*.008;  
        lcd.setCursor(4, 2);
        lcd.print(vel);
        lcd.print("km/h"); 
        lcd.setCursor(0,3);
        if(digitalRead(boton) == LOW){
          digitalWrite(relevador,LOW);
          lcd.clear();
          break;
        }
      }
      if(digitalRead(boton)== LOW){
        digitalWrite(relevador,LOW);
        lcd.clear();
        break;
      }
    }
    delay(150);
  }
  //Parking
  while(nfc1 == 1646308887){
    if(digitalRead(boton)== LOW){
      aumento++;
    }
     if(aumento%2 == 0){
      while(nfc1 == 1646308887){ 
        if(digitalRead(pulsador1) == LOW){
          velocidad++; 
          trampilla=200+trampilla;  
            if (velocidad >= 75){
              velocidad = 75;  
            }
        }
        if(digitalRead(pulsador2) == LOW){
          velocidad--;  
          trampilla=trampilla-200; 
            if (velocidad <= 60){
              velocidad = 60;  
            } 
        }
        if(trampilla>=3800){
          trampilla=3800;
        }
        if(trampilla <=0){
          trampilla=0;
        }
        esc.write(velocidad); 
        delay(150);
        lcd.clear();
        lcd.setCursor(0,0); 
        lcd.print("Parking");
        lcd.setCursor(0, 1);
        lcd.print("RPM:");
        lcd.setCursor(0, 2);
        lcd.print("Vel:");       
        lcd.setCursor(4, 1);
        lcd.print(trampilla);
        vel = trampilla*0.008;
        km=vel/1000;  
        kmR=km+kmR;
        lcd.setCursor(4, 2);
        lcd.print(vel);
        lcd.print("km/h"); 
        lcd.setCursor(0,3);
        while(kmR>=5){
          uint8_t success;
          uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
          uint8_t uidLength;
          success = lector1.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
          uint32_t nfc1 = 0;
          if(success){
              nfc1 = uid[3];
              nfc1 <<= 8; nfc1 |= uid[2];
              nfc1 <<= 8; nfc1 |= uid[1];
              nfc1 <<= 8; nfc1 |= uid[0];
          }
          if(nfc1 == 2451697515){
            kmR=0;
            km=0;
            lcd.clear();
            trampilla=0;
            break;
          }
          velocidad=60;
          esc.write(velocidad);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Distancia");
          lcd.setCursor(0,1);
          lcd.print("Maxima");
          lcd.setCursor(0,2);
          lcd.print("Permitida");
          lcd.setCursor(0,3);
          lcd.print("de Recorrido");
          
        }
        if(digitalRead(boton) == LOW){
          digitalWrite(relevador,LOW);
          lcd.clear();
          break;
        }
      }
      if(digitalRead(boton)== LOW){
        digitalWrite(relevador,LOW);
        lcd.clear();
        break;
      }
    }
    delay(150);
  }
  velocidad--;  
    if(velocidad <= 60){
      velocidad = 60;  
  }
  esc.write(velocidad);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,HIGH);
  digitalWrite(RED,HIGH);
  lcd.clear();
}
void interrupcion1(){
  contador++;
}
