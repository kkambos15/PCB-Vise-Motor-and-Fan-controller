// This code controls a DC motor through a L9110 H-Bridge Driver IC with an ATmega328p-pu MCU
// 4 input buttons control speed and direction of the motor (i.e. fast and slow speed, CCW and CW rotation)
// I am calling fast=coarse, slow=fine, forward=CCW, reverse=CW in this code since the motor is used to adjust the position of a vice
// See L9110 datasheet for motor drive logic  https://cdn-shop.adafruit.com/product-files/4489/4489_datasheet-l9110.pdf


//input button pin assignments
const int button1 = 1;                            // PD1 - coarseForward
const int button2 = 2;                            // PD2 - fineForward
const int button3 = 3;                            // PD3 - fineReverse
const int button4 = 4;                            // PD4 - coarse Forward

//output pin assignments
const int IB = 5;                                 // PD5 - inputB of H-Bridge IC
const int IA = 6;                                 // PD6 - inputA of H-Bridge IC

//input button state variables
int button1State;
int button2State;
int button3State;
int button4State;

int dutyCycle = 50;                               // Enter dutyCycle in % to set PWM speed (e.g. enter '50' for 50% duty cycle)
float PWM_value;

int lastButton2State = LOW;                       //need to know last state for buttons2&3 to determine rising edge of button presses. Not necessary for buttons1&4
int lastButton3State = LOW;

int noButtons;                                    //stores output of noButton and multipleButton checking functions
int multipleButtons;

void setup() {
  
    //GPIO Config
    pinMode(button1, INPUT);
    pinMode(button2, INPUT);
    pinMode(button3, INPUT);
    pinMode(button4, INPUT);
    pinMode(IA, OUTPUT);
    pinMode(IB, OUTPUT);
    PWM_value = (dutyCycle * 2.55);               // Takes dutyCycle in % and converts to PWM value from 0-255 as that's what analogWrite takes as an argument.  (e.g. 30% duty cycle = 76.5)
                                                  // Stored as float but analogWrite will round off/truncate any non-integer 
}

void loop() {
  
    //check the state of each button
    button1State = digitalRead(button1);
    button2State = digitalRead(button2);
    button3State = digitalRead(button3);
    button4State = digitalRead(button4);
    
    //using the 4 button states, check if no buttons or more than 1 button are pressed
    noButtonsPressed(button1State, button2State, button3State, button4State);
    multipleButtonsPressed(button1State, button2State, button3State, button4State);

    if(button1State == HIGH){                     //if button1 is pressed, full speed forward (CCW)
      coarseForward();
    }

    if(button2State == HIGH){                     //if button2 is pressed, slow speed forward (CCW)
      fineForward();
    }

    if(button3State == HIGH){                     //if button3 is pressed, slow speed reverse (CW)
      fineReverse();
    }

    if(button4State == HIGH){                     //if button4 is pressed, full speed reverse (CW)
      coarseReverse();
    }

                                                  //if no buttons are pressed, turn motor off
    if(noButtons == 1){
      motorOff();
    }

                                                  //if multiple buttons are pressed at a time, turn motor off for at least 1 second
    if(multipleButtons == 1){
      motorOff();
      delay(1000);
    }

    lastButton2State = button2State;              //update lastButtonState every loop
    lastButton3State = button3State;
}

//functions --------------------------------------------------------------------------------------------------
void coarseForward(){                             //full speed forward
    digitalWrite(IA, HIGH);                       
    digitalWrite(IB, LOW);                        
}

void fineForward(){                               //slower speed forward set by dutyCycle variable
    if(button2State != lastButton2State){         //checks for initial press of button2 (i.e. initial transition from LOW to HIGH)
        digitalWrite(IA, HIGH);                   //If so, gives motor full power for 10 ms in order to first get it moving (can sometimes stall at lower duty cycles).  
        digitalWrite(IB, LOW);
        delay(10);
    }
    analogWrite(IA, PWM_value);                   //if button2 is still being pressed, PWMs the motor for slower speed
    digitalWrite(IB, LOW);
}

void fineReverse(){                               //slower speed reverse set by dutyCycle variable
    if(button3State != lastButton3State){         //Same logic as "fineForward" except with button3 and mirrored outputs
        digitalWrite(IA, LOW);
        digitalWrite(IB, HIGH);
        delay(10);
    }
    digitalWrite(IA, LOW);
    analogWrite(IB, PWM_value);
}

void coarseReverse(){                             //full speed reverse
    digitalWrite(IA, LOW);                        
    digitalWrite(IB, HIGH);                       
}

                                                  //returns 1 if all 4 buttons are LOW, returns 0 otherwise
int noButtonsPressed(int w, int x, int y, int z){ 
    noButtons = !(w || x || y || z);
    return noButtons;
}

                                                  //returns 1 if 2 or more buttons are HIGH, returns 0 otherwise
int multipleButtonsPressed(int a, int b, int c, int d){
    multipleButtons = ((a && b) || (a && c) || (a && d) || (b && c) || (b && d) || (c && d));
    return multipleButtons;
}
    
void motorOff(){                                  //turns off motor
    digitalWrite(IA, LOW);
    digitalWrite(IB, LOW);
}
