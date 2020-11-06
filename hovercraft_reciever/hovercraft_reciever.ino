#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <Servo.h>

RF24 radio(D0, D8); // CE, CSN
const byte address[6] = "00001";

Servo esc_hover;
Servo esc_thrust_r;
Servo esc_thrust_l;

int thrust_forward = 0, hover_val= 0, thrust_differential = 0;
int thrust_r_val = 0, thrust_l_val = 0;
byte payload [3];
int button_state = 0;

void setup()
{
  Serial.begin(115200);  // Debugging only
 
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  esc_hover.attach(D2,1000,2000);
  esc_thrust_r.attach(D1,1000,2000);
  esc_thrust_l.attach(D3,1000,2000);

    
}

void loop()
{
  unsigned long timer = micros();
  read_controller();

  if (payload[0] == 0){
    motor_pwm(1, 0);
    motor_pwm(2, 0);
    motor_pwm(3, 0);
  }

  else if (payload[0] == 1){
    hover_val = map(payload[2], 0, 254, 0, 180);
    motor_pwm(1, hover_val);
    motor_pwm(2, 0);
    motor_pwm(3, 0);
  }

  else if (payload[0] == 2){
    thrust_forward = map(payload[1], 254, 0, 0, 40);
    thrust_differential = payload[2]-130;
    thrust_r_val = thrust_forward + int(thrust_differential/254.0 * 25);
    thrust_l_val = thrust_forward - int(thrust_differential/254.0 * 25);
    thrust_r_val = constrain(thrust_r_val, 0, 50);
    thrust_l_val = constrain(thrust_l_val, 0, 50);
    
    //Serial.println(thrust_differential);
    motor_pwm(2, hover_val);
    motor_pwm(2, thrust_r_val);
    motor_pwm(3, thrust_l_val);
  }

  else{
    motor_pwm(1, 0);
    motor_pwm(2, 0);
    motor_pwm(3, 0);
  }
  Serial.print(payload[0]);
  Serial.print(" ");
  Serial.print(payload[1]);
  Serial.print(" ");
  Serial.println(payload[2]);
  delay(10);
    
}

void motor_pwm(int motor, int pwm){
  if (motor ==1){esc_hover.write(pwm);}
  else if(motor == 2){esc_thrust_r.write(pwm);}
  else if(motor == 3){esc_thrust_l.write(pwm);}
}

void read_controller(){

    if (radio.available()) // Non-blocking
    {

      radio.read(payload, sizeof(payload));
      
  } 

}
