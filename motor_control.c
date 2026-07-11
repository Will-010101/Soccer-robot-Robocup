#include <33EP128GP506.h>
#device ICSP=1
#include <ConfigureClock.h>
#include <math.h>


#define DELAY 500
#define MAX_Velocity  1500


#pin_select OC1 = PIN_B4  // pwm1
#pin_select OC2 = PIN_A4  // pwm2
#pin_select OC3 = PIN_b11 // pwm3
#pin_select OC4 = PIN_c6  // pwm4


#pin_select U1TX = PIN_F1
#pin_select U1rX = PIN_F0


#use rs232(UART1, baud=9600,xmit=pin_f1,rcv=pin_f0)
#use i2c(MASTER,I2C1,Force_HW,slow)
#include <MyFunction.c>


#define Disable  OutPut_low(pin_c3)    //Disable All Motor
#define Enable   OutPut_high(pin_C3)   // Enable All Motor

#define r_high   OutPut_high(PIN_G6)
#define r_low    OutPut_low(PIN_G6)
#define l_high   OutPut_high(PIN_G7)
#define l_low    OutPut_low(PIN_G7)

#define Rkey     InPut(PIN_A10)        //Right Key
#define Lkey     InPut(PIN_B13)        //Left Key
#define Start    InPut(pin_A7)         //Start Key


#define TSSP_INT InPut(PIN_B1)
#define SRF_INT  InPut(PIN_A4)//D6 damage changed  A4 khak bar saresh
#define NJL_INT  InPut(PIN_D5)

void set_motor(float angle , float x)
{
   int max_speed=900;    //6*180
   
   angle += 45;
   if( angle > 360 )
   angle -=360;
   angle = angle * 3.14/180;     //convert to radian
   
   float error;
   int head_speed;
   if(x<180) {error=x;}
   else if(x>=180) {error=360-x;}
   head_speed= error;
   
   float p1, p2, p3, p4;
   float cos1=(cos(angle)*max_speed),sin1=(sin(angle)*max_speed);
   p1=abs(cos1) ; p2=abs(sin1); p3=p1; p4=p2; 
   
   //if(p1>=p2) { p2=((p2)*(max_speed)); p1=max_speed; p3=p1; p4=p2; printf("353*500=%u\r\n" ,(353*500)); printf("p2*max:%ld \r\n" ,p2); printf("max:%f \r\n" ,((max_speed))); }
   
   if(p1>=p2) {p2= ((float)(p2/p1))*max_speed; p1=max_speed; p3=p1; p4=p2;/*printf("no\r\n");*/}
   else if(p2>p1) { p1= (((float)(p1/p2))*max_speed); p2=max_speed; p4=p2; p3=p1; }

 
   angle-=45;
   if(x>10 && x<180)
   {  if(x>0 && x<180 ) {p1-= error*(max_speed/180); p3+= error*(max_speed/180);}
       else{p1+= error*(max_speed/180); p3-= error*(max_speed/180);}
      if(sin(angle)>0) {p2+= error*(max_speed/180); p4-= error*(max_speed/180);}
       else{p2-= error*(max_speed/180); p4+= error*(max_speed/180);}
      if(p1<0) {p1=0; p3=0;}
      if(p2<0) {p2=0; p4=0;}
      if(p1>max_speed) {p1=max_speed; p3=max_speed;}
      if(p2>max_speed) {p2=max_speed; p4=max_speed;}
   }
       
      
      set_pwm_duty(1,(signed int)p1);
      set_pwm_duty(2,(signed int)p2);
      set_pwm_duty(3,(signed int)p3);
      set_pwm_duty(4,(signed int)p4);
//!      printf("p1:%d \r\n" ,p1);
//!      printf("p2:%d \r\n" ,p2);
//!      printf("p3:%d \r\n" ,p3);
//!      printf("p4:%d \r\n" ,p4);

      angle+=45;
      if(cos(angle)<0)
      { output_low(PIN_e15);    /*up_right*/     output_high(PIN_c2);    /*down_left*/ }
      else
      { output_high(PIN_e15);    /*up_right*/     output_low(PIN_c2);    /*down_left*/ }
      
      if(sin(angle)>=0)
      { output_low(PIN_a9);     /*up_left*/     output_low(PIN_e13);   /*down_right*/ }
      else
      { output_high(PIN_a9);     /*up_left*/     output_high(PIN_e13);   /*down_right*/ }
 
}



void set_head(float x)
{
   float error;
   int head_speed;
   
   if(x<180) {error=x;}
   else if(x>=180) {error=360-x;}
   
   head_speed= error*1300/180;
   if(x>5 && x<180)
   {
      set_pwm_duty(1,head_speed);  
      set_pwm_duty(2,head_speed); 
      set_pwm_duty(3,head_speed);
      set_pwm_duty(4,head_speed); 
      output_low(PIN_e15);    /*up_right*/     output_high(PIN_c2);    /*down_left*/ 
      output_high(PIN_a9);     /*up_left*/     output_low(PIN_e13);   /*down_right*/ 
   }
   else if(x>=180 && x<355)
   {
      set_pwm_duty(1,head_speed);  
      set_pwm_duty(2,head_speed); 
      set_pwm_duty(3,head_speed);
      set_pwm_duty(4,head_speed); 
      output_high(PIN_e15);    /*up_right*/     output_low(PIN_c2);    /*down_left*/ 
      output_low(PIN_a9);     /*up_left*/     output_high(PIN_e13);   /*down_right*/ 
   }
   else
   {
      set_pwm_duty(1,0);  
      set_pwm_duty(2,0); 
      set_pwm_duty(3,0);
      set_pwm_duty(4,0); 
   }
}
void main()
{
    setup_pll();
   
   //setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 40000);   
   //enable_interrupts(INT_TIMER2);
   enable_interrupts(INTR_GLOBAL);
   //N=(1000*MIPS)/(Fpwm*T_DIV) -->>1000=59.95 Khz
   setup_timer1(TMR_INTERNAL | TMR_DIV_BY_1,2000 ); //2000
   //setup_compare(1, COMPARE_PWM_EDGE | COMPARE_TIMER1);
   setup_compare(1, COMPARE_PWM_EDGE | COMPARE_TIMER1 | COMPARE_SYNCHRONIZE |
                       COMPARE_TRIG_SYNC_TIMER1);
   setup_compare(2, COMPARE_PWM_EDGE | COMPARE_TIMER1 | COMPARE_SYNCHRONIZE |
                       COMPARE_TRIG_SYNC_TIMER1);
   setup_compare(3, COMPARE_PWM_EDGE | COMPARE_TIMER1 | COMPARE_SYNCHRONIZE |
                       COMPARE_TRIG_SYNC_TIMER1);
   setup_compare(4, COMPARE_PWM_EDGE | COMPARE_TIMER1 | COMPARE_SYNCHRONIZE |
                       COMPARE_TRIG_SYNC_TIMER1);
    
    disable; 
    set_pwm_duty(1,0) ;    output_low(pin_e15); //Motor 1 is off:low=ccw
    set_pwm_duty(2,0) ;    output_low(pin_a9);  //Motor 2 is off
    set_pwm_duty(3,0) ;    output_low(pin_c2);  //Motor 3 is off
    set_pwm_duty(4,0) ;    output_low(pin_e13); //Motor 4 is off
    //Search_i2c();
    
    delay_ms(500);
    BNO055_Config();
    delay_ms(500);
    enable;
    //delay_ms(3000);
    while(Start==1);
    delay_ms(100);
    while(Start==0);
    while(True)
    {
    //printf("oskoletun kardam\r\n");
     //set_motor(120);
     BNO055_read();
     set_motor(120,Yaw);
    //if(TSSP_INT) 
    //{

//  TSSP_read();
      //  float ball_angle=theta;
      //  float distance=dis;
    //}
    }
    
    disable;
}
