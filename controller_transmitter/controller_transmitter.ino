#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> 

RF24 radio(9, 10); // CE, CSN

const byte address[6] = "00001";


bool button_1_state = false;
bool button_2_state = false;
const byte interruptPin1 = 2;
const byte interruptPin2 = 3;
int hover_val, thrust, rudder;

//String payload;
byte payload[3];
unsigned long timer, transmit_timer = 0;

void setup() {
  Serial.begin(115200);
  pinMode(interruptPin1, OUTPUT);
  pinMode(interruptPin2, OUTPUT);
  /*attachInterrupt(digitalPinToInterrupt(interruptPin1), hover_ctl, RISING); 
  attachInterrupt(digitalPinToInterrupt(interruptPin2), frwd_ctl, RISING);*/
  delay(10);
  radio.begin();
  delay(5);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
}

void loop() {
  timer = millis();
  button_state();

 
  if(button_1_state == 0){
    payload[0] = 0;
    payload[1] = 0;
    payload[2] = 0;
    radio.write(payload, sizeof(payload));
    //transmit(msg);
  }
  if(button_1_state == 1 && button_2_state == 0){
    hover_val = analogRead(A1);
    //Serial.println(hover_val);
    //payload = check_size(hover_val);
    hover_val = map(hover_val, 0, 1023, 0, 254);
    payload[0] = 1;
    payload[1] = 0;
    payload[2] = hover_val;
    radio.write(payload, sizeof(payload));
   // transmit(payload);
  }

  if(button_1_state == 1 && button_2_state == 1){
    thrust = analogRead(A4);//Thrust
    thrust = map(thrust, 0, 1023, 0, 254);
    rudder = analogRead(A3);//Rudder
    rudder = map(rudder_val, 0, 1023, 0, 254);
    payload[0] = 2;
    payload[1] = thrust;
    payload[2] = rudder;
    radio.write(payload, sizeof(payload));
  }
  //Serial.println(rudder);
  
}

void button_state(){
  int but1 = digitalRead(2);
  int but2 = digitalRead(3);
  if (but1 ==1){
    if(button_1_state == false){
      button_1_state = true;}
    else{button_1_state = false;}
  }
  if (but2 == 1){
    if(button_2_state == false){button_2_state = true;}
    else{button_2_state = false;}
  }
}

void transmit (const char payload){
    if (transmit_timer - timer >= 1000){
      const char text[] = "Hello World";
      Serial.println("sending msg");
      radio.write(&payload, sizeof(payload));
      //Serial.println(msg);
      transmit_timer = timer; 
    };
}

String check_size(int val){
    String val_upd;
   
    if (val <10){
      val_upd = "000" + String(val);
      
    }else if (val <100){
      val_upd = "00" + String(val);
   
    }
    else if (val <1000){
      val_upd = "0" + String(val);
    }
    else{
      val_upd = String(val);
    }
    return val_upd;
}
