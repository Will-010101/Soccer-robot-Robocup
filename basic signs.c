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

float i=0 , ball_angle=0 , distance=0 , flg=0;

#int_timer2  
void inpu2 (){   flg+=1;}

void main()
{
    setup_pll();
     enable_interrupts(INTR_GLOBAL);
   setup_timer1(TMR_INTERNAL | TMR_DIV_BY_1,2000 ); //2000
   setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 59999);
    enable_interrupts(int_rda);
    enable_interrupts(int_timer2);
    enable_interrupts(GLOBAL);
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
    delay_ms(500);
    BNO055_Config();
    delay_ms(500);
    enable;
    
    while(True)
    {
//!       while(Start==1)
//!       {
//!         if(i==1)
//!        {
//!          set_pwm_duty(1,1000);
//!         set_pwm_duty(2,1000);
//!         set_pwm_duty(3,1000);
//!         set_pwm_duty(4,1000);
//!         output_low(PIN_e15);
//!          output_high(PIN_c2);
//!          output_high(PIN_a9); 
//!          output_high(PIN_e13); 
//!         }
//!         if(i==2)
//!         {
//!            set_pwm_duty(1,0) ;    output_low(pin_e15); 
//!            set_pwm_duty(2,0) ;    output_low(pin_a9);  
//!            set_pwm_duty(3,0) ;    output_low(pin_c2);  
//!            set_pwm_duty(4,0) ;    output_low(pin_e13);
//!            BNO055_read();
//!            printf("Gyro:%f \r\n" ,Yaw);
//!         }
//!         if(i==3)
//!         {
//!           r_high;
//!           l_high;
//!           delay_ms(500);
//!           r_low;
//!           l_low;
//!           delay_ms(500);
//!         }
//!         if(i==4)
//!         {
//!           if(TSSP_INT) 
//!           {
//!              TSSP_read();
//!              ball_angle=theta;
//!              distance=dis;
//!              ball_angle-=90; if(ball_angle<0)ball_angle+=360;ball_angle=360-ball_angle;
//!              if(flg>98)
//!              { printf("ball_angle:%f \r\n" ,ball_angle); printf("distance:%f \r\n" ,distance); flg=0;}
//!           }
//!         }
//!       
//!       }
//!       delay_ms(100);
//!       while(Start==0);
//!       delay_ms(200);
//!       i=i+1;
//!       if(i>=5)
//!       {i=1;}
r_high;
           l_high;
           delay_ms(500);
           r_low;
           l_low;
           delay_ms(500);
    }  
    disable;
}

