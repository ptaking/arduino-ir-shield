//**************************************************************//
//  Name    : Arduino IR Shield                                 //
//  Author  : Tuan Anh Pham                                     //
//  Date    : 15 Feb, 2018                                      //
//  Version : 1.0                                               //
//                     Happy Lunar New Year                     //
//****************************************************************

#include <IRremote.h>
// Define Pins
int latchPin = 7;
int dataPin = 5;
int clockPin = 6;
int recvPin = 4;
int statusPin = 13; // Builtin LED

// Declare variables
byte btnPressed = 72;  //01001000
int codeType = -1; // The type of code
int codeTypeArray[7];
unsigned long codeValue; // The code value if not raw
unsigned long codeValueArray[7];
unsigned int rawCodes[RAWBUF]; // The durations if raw
int codeLen; // The length of the code
int codeLenArray[7];
int toggle = 0; // The RC5/6 toggle state
int programBtn = 255;
int programMode = 1;
int lastBtnState[7];
unsigned long previousMillis = 0;
int ledState = LOW;

// Initialize IR Module
IRrecv irRecv(recvPin);
IRsend irSend;

decode_results results;

void setup() {
  // Serial for debugging
  Serial.begin(9600);
  // Start the receiver
  irRecv.enableIRIn(); 

  // Define Pin modes
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(dataPin, INPUT);
  pinMode(statusPin, OUTPUT);
  Serial.println(programMode);
  Serial.println(programBtn);
  // Predefined LG Smart TV codes, the reason why I made this.
  codeValueArray[0] = 0x20DF3EC1; // Home button
  codeTypeArray[0] = NEC;
  codeLenArray[0] = 32;
  codeValueArray[1] = 0x20DF02FD; // Up button
  codeTypeArray[1] = NEC;
  codeLenArray[1] = 32;
  codeValueArray[2] = 0x20DF14EB; // Return button
  codeTypeArray[2] = NEC;
  codeLenArray[2] = 32;
  codeValueArray[3] = 0x20DFE01F; // Left button
  codeTypeArray[3] = NEC;
  codeLenArray[3] = 32;
  codeValueArray[4] = 0x20DF22DD; // OK button
  codeTypeArray[4] = NEC;
  codeLenArray[4] = 32;
  codeValueArray[5] = 0x20DF609F; // Right button
  codeTypeArray[5] = NEC;
  codeLenArray[5] = 32;
  codeValueArray[6] = 0x20DF827D; // Down button
  codeTypeArray[6] = NEC;
  codeLenArray[6] = 32;
}

void loop() {
  digitalWrite(latchPin, 1);
  delayMicroseconds(20);  
  digitalWrite(latchPin, 0); 
  btnPressed = shiftIn(dataPin, clockPin);

  switch (btnPressed) {
    case 33:
      Serial.println("Program button pressed");
      programMode = 2;
      irRecv.enableIRIn();
      break;
    case 160:
      Serial.println("Button 1 pressed");
      pressed(0);
      break;
    case 34:
      Serial.println("Button 2 pressed");
      pressed(1);
      break;
    case 36:
      Serial.println("Button 3 pressed");
      pressed(2);
      break;
    case 96:
      Serial.println("Button 4 pressed");
      pressed(3);
      break;
    case 48:
      Serial.println("Button 5 pressed");
      pressed(4);
      break;
    case 40:
      Serial.println("Button 6 pressed");
      pressed(5);
      break;
    case 0:
      Serial.println("Button 7 pressed");
      pressed(6);
      break;
    default:
      for (int i = 0; i <= 6; i++) {
        lastBtnState[i] = LOW;
      }
      if (programMode != 2) {
        digitalWrite(statusPin, LOW);  
      }
      break;
  }
  
  if (irRecv.decode(&results) && programBtn != 255 && programMode == 2) {
    storeCode(&results, programBtn);
    irRecv.resume();
  }
  if (programMode == 2) {
    if (programBtn == 255) {
      blinkLed(250);
    } else {
      blinkLed(100);
    }
  }
}

byte shiftIn(int myDataPin, int myClockPin) { 
  int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(0.2);
    temp = digitalRead(myDataPin);
    if (temp) {
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
    }
    else {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }

    //Debuging print statements
    //Serial.print(pinState);
    //Serial.print("     ");
    //Serial.println (dataIn, BIN);

    digitalWrite(myClockPin, 1);

  }
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}

void storeCode(decode_results *results, int btn) {
  codeType = results->decode_type;
  //int count = results->rawlen;
  if (codeType == UNKNOWN) {
    Serial.println("Received unknown code, saving as raw");
    codeLen = results->rawlen - 1;
    // To store raw codes:
    // Drop first value (gap)
    // Convert from ticks to microseconds
    // Tweak marks shorter, and spaces longer to cancel out IR receiver distortion
    for (int i = 1; i <= codeLen; i++) {
      if (i % 2) {
        // Mark
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        rawCodes[i - 1] = results->rawbuf[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(rawCodes[i - 1], DEC);
    }
    codeValueArray[btn] = rawCodes;
    codeLenArray[btn] = codeLen;
    codeTypeArray[btn] = codeType;
    Serial.println("");
  }
  else {
    if (codeType == NEC) {
      Serial.print("Received NEC: ");
      if (results->value == REPEAT) {
        // Don't record a NEC repeat value as that's useless.
        Serial.println("repeat; ignoring.");
        return;
      }
    } 
    else if (codeType == SONY) {
      Serial.print("Received SONY: ");
    } 
    else if (codeType == PANASONIC) {
      Serial.print("Received PANASONIC: ");
    }
    else if (codeType == JVC) {
      Serial.print("Received JVC: ");
    }
    else if (codeType == RC5) {
      Serial.print("Received RC5: ");
    } 
    else if (codeType == RC6) {
      Serial.print("Received RC6: ");
    } 
    else {
      Serial.print("Unexpected codeType ");
      Serial.print(codeType, DEC);
      Serial.println("");
    }
    Serial.print(results->value, HEX);
    Serial.print(" @ ");
    Serial.println(results->bits);
    codeValue = results->value;
    codeLen = results->bits;
    codeValueArray[btn] = codeValue;
    codeLenArray[btn] = codeLen;
    codeTypeArray[btn] = codeType;
  }
  programMode = 1;
  programBtn = 255;
  digitalWrite(statusPin, LOW);
}

void sendCode(byte repeat, int btn) {
  codeValue = codeValueArray[btn];
  codeLen = codeLenArray[btn];
  codeType = codeTypeArray[btn];
  if (codeType == NEC) {
    if (repeat) {
      irSend.sendNEC(REPEAT, codeLen);
      Serial.println("Sent NEC repeat");
    } 
    else {
      irSend.sendNEC(codeValue, codeLen);
      Serial.print("Sent NEC ");
      Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == SONY) {
    irSend.sendSony(codeValue, codeLen);
    Serial.print("Sent Sony ");
    Serial.println(codeValue, HEX);
  } 
  else if (codeType == PANASONIC) {
    irSend.sendPanasonic(codeValue, codeLen);
    Serial.print("Sent Panasonic");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == JVC) {
    irSend.sendJVC(codeValue, codeLen, false);
    Serial.print("Sent JVC");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == RC5 || codeType == RC6) {
    if (!repeat) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    }
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Serial.print("Sent RC5 ");
      Serial.println(codeValue, HEX);
      irSend.sendRC5(codeValue, codeLen);
    } 
    else {
      irSend.sendRC6(codeValue, codeLen);
      Serial.print("Sent RC6 ");
      Serial.println(codeValue, HEX);
    }
  } 
  else if (codeType == UNKNOWN /* i.e. raw */) {
    // Assume 38 KHz
    irSend.sendRaw(rawCodes, codeLen, 38);
    Serial.println("Sent raw");
  }
}

void pressed(int btn) {
  digitalWrite(statusPin, HIGH);
  if (programMode == 2) {
    programBtn = btn;
  } else {
    sendCode(lastBtnState[btn] == HIGH, btn);
  }
  lastBtnState[btn] = HIGH;
}

void blinkLed(long sec) {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= sec) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(statusPin, ledState);
  }
}
