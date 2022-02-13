volatile unsigned char LookUp1[102] = {
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
238, 221, 204, 187, 170, 153, 136, 119, 102,  85,  68,  51,  34,  17,   0,  
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

volatile unsigned char LookUp2[102] = {
238, 221, 204, 187, 170, 153, 136, 119, 102,  85,  68,  51,  34,  17,   0,  
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
  0,  17,  34,  51,  68,  85, 102, 119, 136, 153, 170, 187, 204, 221, 238,  
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};



//const int dead_time = 100;// 6.25 us
int dead_time = 110;
int half_dead_time = dead_time/2;

int pwm_phU=0;
int pwm_phV=0;
int pwm_phW=0;

char PwmOrderU=0;
char PwmOrderV=34;
char PwmOrderW=68;
//=====================Pin Assignments==================
const int Contactor = 12;
const int FaultPin  = 2;
const int FaultLed  = 4;
const int pos1 = 7;
const int pos2 = 8;
const int pos3 = 13;
const int pos4 = A0;
const int pos5 = A1;
const int pos6 = A3;
const int pos7 = A4;
const int pos8 = A5;
//======================================================

int FrqVar      = 0;
int FrqVarTemp  = 400;
int RampUpCompleted=0;
int temp = 0;
void posControl(){
  // komütatör ile Hz cinsinden motor sürüş hızı 
     if     (digitalRead(pos1) == HIGH)   FrqVar = 205;  //50  hz         
     else if(digitalRead(pos2) == HIGH)   FrqVar =  90;  //100 hz         
     else if(digitalRead(pos3) == HIGH)   FrqVar =  50;  //150 hz           
     {  
     if     (digitalRead(pos4) == HIGH)   FrqVar = 31;   //200 hz         
     else if(digitalRead(pos5) == HIGH)   FrqVar = 19;    //250 hz         
     else if(digitalRead(pos6) == HIGH)   FrqVar = 12;    //300 hz         
     else if(digitalRead(pos7) == HIGH)   FrqVar =  6;    //350 hz         
     else if(digitalRead(pos8) == HIGH)   FrqVar =  2;    //400 hz         
}
  
  }

void(* resetFunc) (void) = 0;

void UpdatePwm(void) {
  if(pwm_phU > 255 - half_dead_time) pwm_phU = 255 - half_dead_time;
  
  if(pwm_phV > 255 - half_dead_time) pwm_phV = 255 - half_dead_time;
  
  if(pwm_phW > 255 - half_dead_time) pwm_phW = 255 - half_dead_time;
 
  if(pwm_phU < half_dead_time) pwm_phU = half_dead_time;
  
  if(pwm_phV < half_dead_time) pwm_phV = half_dead_time;
  
  if(pwm_phW < half_dead_time) pwm_phW = half_dead_time;

  while(TCNT0 > 0xFF) {}                                       // bunları ff yaptım. daha hızlı oldu
  OCR0A = (pwm_phU + half_dead_time);//~6 TL PH0 TMR0
  OCR0B = (pwm_phU - half_dead_time);//~5 TH PH0 TMR0
  while(TCNT1 > 0xFF) {}
  OCR1A = (pwm_phV + half_dead_time);//~9 TL PH1 TMR1
  OCR1B = (pwm_phV - half_dead_time);//~10 TH PH1 TMR1
  while(TCNT2 > 0xFF){}
  OCR2A = (pwm_phW + half_dead_time);//~3 TH PH2 TMR2
  OCR2B = (pwm_phW - half_dead_time);//~11 TL PH2 TMR2
  
}

void SyncPwmTimers(void) {
  GTCCR = (GTCCR & 0xFF) | 0x81;
  TCNT0 = 00;
  TCNT1 = 0x0000;
  TCNT2 = 0x00;
  GTCCR = (GTCCR & 0x7E);
  
  
}
  
//============================================

void RampUp()

{

char s=0;

   if(RampUpCompleted==0)
  {
    while(FrqVarTemp>FrqVar)
    {

      WaitContactor();
      OverCurrent();
      
    PwmOrderU++;if(PwmOrderU>101) 
                                 {
                                  PwmOrderU=0;                                //Increase dead time to avoid over current during ramp up
                                  if(FrqVarTemp >80){FrqVarTemp=FrqVarTemp-4;dead_time = 110;half_dead_time = dead_time/2;}
                                  if(FrqVarTemp <=80){FrqVarTemp=FrqVarTemp-2;dead_time = 100;half_dead_time = dead_time/2;} 
                                 } 
    PwmOrderV++;if(PwmOrderV>101){PwmOrderV=0;}
    PwmOrderW++;if(PwmOrderW>101){PwmOrderW=0;}

  
    pwm_phU=LookUp1[PwmOrderU];
    pwm_phV=LookUp2[PwmOrderV];
    pwm_phW=LookUp1[PwmOrderW];
    
    UpdatePwm();
        
      for(int i=0;i<FrqVarTemp;i++)
      {
      __asm__("nop\n\t");  
      }
    
    }

//delay(1000); 
dead_time = 100;
half_dead_time = dead_time/2;
    
   
    
    while(dead_time>15) {
      
   
      WaitContactor();
      OverCurrent();
   
    
    PwmOrderU++;
    if(PwmOrderU>101) 
     {
      
      
      
      PwmOrderU      = 0; 

      s++;
        if(s>=3)
        {
        s=0;
        dead_time      = dead_time-1;
        half_dead_time = dead_time/2;  
        }

      

     } 
    PwmOrderV++;if(PwmOrderV>101){PwmOrderV=0;}
    PwmOrderW++;if(PwmOrderW>101){PwmOrderW=0;}
 
  
    pwm_phU=LookUp1[PwmOrderU];
    pwm_phV=LookUp2[PwmOrderV];
    pwm_phW=LookUp1[PwmOrderW];
    
    UpdatePwm();
       
      for(int i=0;i<FrqVar;i++)
      {
      __asm__("nop\n\t");  
      }
    
    }
  

  RampUpCompleted=1;  
    
  }  

 }
//============================
void RampDown()
{
FrqVarTemp=FrqVar;    

dead_time =110;
half_dead_time = dead_time/2;    

  while(FrqVarTemp<300)
  {
   OverCurrent(); 
   //WaitContactor(); 
  // posControl();
    
  PwmOrderU++;if(PwmOrderU>101)
                               {PwmOrderU=0;
                                 if(FrqVarTemp>200                    ){FrqVarTemp=FrqVarTemp+6;}
                                 if(FrqVarTemp>50  && FrqVarTemp<= 200){FrqVarTemp=FrqVarTemp+3;}
                                 if(FrqVarTemp> 1  && FrqVarTemp<=  50){FrqVarTemp=FrqVarTemp+1;} 
                                 if(                 FrqVarTemp<=    1){FrqVarTemp=FrqVarTemp+1;}
                               }              
  PwmOrderV++;if(PwmOrderV>101){PwmOrderV=0;}
  PwmOrderW++;if(PwmOrderW>101){PwmOrderW=0;}

  
  pwm_phU=LookUp1[PwmOrderU];
  pwm_phV=LookUp2[PwmOrderV];
  pwm_phW=LookUp1[PwmOrderW];
    
  UpdatePwm();  
      
    for(int i=0;i<FrqVarTemp;i++)
    {
    __asm__("nop\n\t");  
    }
  }  
}


void WaitContactor()
{
    if(digitalRead(Contactor)==LOW)
    { 
    delay(10);
        if(digitalRead(Contactor)==LOW)
        { 
        delay(10);  
            if(digitalRead(Contactor)==LOW)
            { 
            delay(10); 
                if(digitalRead(Contactor)==LOW)
                { 
                delay(10); 
                    if(digitalRead(Contactor)==LOW)
                    { 
                    delay(10);
                        if(digitalRead(Contactor)==LOW)
                        { 
                         
                            if(RampUpCompleted==1)
                            {
                            RampDown(); 
                           
                            }
                          
                        pinMode(3,  INPUT);
                        pinMode(5,  INPUT);  
                        pinMode(6,  INPUT); 
                        pinMode(9,  INPUT); 
                        pinMode(10, INPUT); 
                        pinMode(11, INPUT);
                        delay(100000); // ~1sec
                            while(digitalRead(Contactor)==LOW)
                            {
                           delay(7500);//~100ms
                            }
                       delay(7500);//~100ms
                        resetFunc();
                        }   
                    }  
                }  
            } 
        }  
    }
}

void OverCurrent()
{
  if(digitalRead(FaultPin)==LOW)
  {
  delay(5);  
    if(digitalRead(FaultPin)==LOW)
    {
//  delay(10);
//      if(digitalRead(FaultPin)==HIGH)
//      {
//      delay(10);
//        if(digitalRead(FaultPin)==HIGH)
//        {
//        delay(10);
//          if(digitalRead(FaultPin)==HIGH)
          {
          pinMode(3,  INPUT);
          pinMode(5,  INPUT);  
          pinMode(6,  INPUT); 
          pinMode(9,  INPUT); 
          pinMode(10, INPUT); 
          pinMode(11, INPUT);

          digitalWrite(FaultLed, HIGH);

            while(digitalRead(Contactor)==HIGH)
            {
            __asm__("nop\n\t");
            }


          pinMode(3,  INPUT);
          pinMode(5,  INPUT);  
          pinMode(6,  INPUT); 
          pinMode(9,  INPUT); 
          pinMode(10, INPUT); 
          pinMode(11, INPUT);
          
         RampUpCompleted=0;
          digitalWrite(FaultLed, LOW);              
          }
        } 
      }      
    }
//  }
//}       
//============================================

void setup() {
  pinMode(Contactor,INPUT); 
  pinMode(FaultPin ,INPUT);
  pinMode(FaultLed ,OUTPUT);

  pinMode(pos1, INPUT);
  pinMode(pos2, INPUT);
  pinMode(pos3, INPUT);
  pinMode(pos4, INPUT);
  pinMode(pos5, INPUT);
  pinMode(pos6, INPUT);
  pinMode(pos7, INPUT);
  pinMode(pos8, INPUT);
  
  pinMode(3,  INPUT);
  pinMode(5,  INPUT);  
  pinMode(6,  INPUT); 
  pinMode(9,  INPUT); 
  pinMode(10, INPUT); 
  pinMode(11, INPUT);   

  TCCR1A = 0xE1;//PWM, Phase Correct 8-bit TOP at 0xFF 
  TCCR0A = 0xE1;//PWM, Phase Correct 8-bit TOP at 0xFF 
  TCCR2A = 0xE1;//PWM, Phase Correct 8-bit TOP at 0xFF 
  
  TCCR2B = 0x02;//clkI/O/1 (No prescaling) 31.25kHz PWM frequency   // bunları 01 iken 02 yaptım 3,9 khz hızında tetiklendi.
  TCCR1B = 0x02;//clkI/O/1 (No prescaling) 31.25kHz PWM frequency
  TCCR0B = 0x02;//clkI/O/1 (No prescaling) 31.25kHz PWM frequency
  
  SyncPwmTimers();
  
  pinMode(3, OUTPUT);   //PWM output
  pinMode(5, OUTPUT);   //PWM output
  pinMode(6, OUTPUT);   //PWM output
  pinMode(9, OUTPUT);   //PWM output
  pinMode(10, OUTPUT);   //PWM output
  pinMode(11, OUTPUT);   //PWM output

  delay(1000); //~100ms
 


  //FrqVar = 32;
  posControl();
  RampUp();
  //delay(100);
}



void loop() 
{  
  
  
  OverCurrent();
  WaitContactor();

 
  pwm_phU=LookUp1[PwmOrderU];//////////////////////
  pwm_phV=LookUp2[PwmOrderV];//////////////////////
  pwm_phW=LookUp1[PwmOrderW];//////////////////////

   
  
  PwmOrderU++;if(PwmOrderU>101){PwmOrderU=0;} 
  PwmOrderV++;if(PwmOrderV>101){PwmOrderV=0;}
  PwmOrderW++;if(PwmOrderW>101){PwmOrderW=0;}

  UpdatePwm();  
  
  for(int i=0;i<FrqVar;i++)
    {
    __asm__("nop\n\t");  
    
    }

       
 // delayMicroseconds (37);

  

}
