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
 
//Prototipe**************************************************************************************
void Set_Motor(float,float);
void Set_Head(float);
void Move_Ball(float,float);
int Middle_Filter_angle();
int Middle_Filter_distance();
void Kalman_Filter(float,float);

//Global Variable********************************************************************************
int32  max_speed , max_speed_rotate=1800 , flg_enable=1;
float last_error_head=0 , error_head=0 , Gyro , ball_angle , distance , last_ball_angle=0 , last_distance=0 , delta_ball_angle , delta_distance , new_distance=0 , new_ball_angle=0;
int flg=0 , mid_ball_angle[5]={0,0,0,0,0} , mid_distance[5]={0,0,0,0,0} , counter = 0 ;
int32 ti,tis;
//Timer******************************************************************************************
#int_rda   
void input ()
{
   char value=getchar();
   if(value=='s'){flg_enable=0;}
   if(value=='p'){flg_enable=1;}
}  
#int_timer2  //
void inpu2 (){   flg+=1;tis++;}

//function***************************************************************************************
void Set_Motor(float angle , float speed)
{
   int flg_p1=0 , flg_p2=0 , flg_p3=0 , flg_p4=0 ;
   float angle2=angle+45 , p1, p2, p3, p4 ,cos1 , sin1 ,kd , kp,sinn,coss;
   if(angle2>360)  angle2-=360;
   max_speed=speed; 
   Gyro=Yaw;
   angle += 45; if( angle > 360 )angle -=360; angle = angle * 3.14/180;     //convert to radian
   if(Gyro<180) {error_head=Gyro;} else if(Gyro>=180) {error_head=360-Gyro;}//set error head
   coss=cos(angle);
   sinn=sin(angle);
   cos1=(coss*max_speed); sin1=(sinn*max_speed);
   p1=abs(cos1) ; p2=abs(sin1); p3=p1; p4=p2; 
   
   if(p1>=p2) { p2= ((float)(p2/p1))*max_speed; p1=max_speed; p3=p1; p4=p2; }
   else if(p2>p1) { p1= (((float)(p1/p2))*max_speed); p2=max_speed; p4=p2; p3=p1; }
   
   kd=(last_error_head-error_head)*(-500); if(kd > max_speed_rotate)kd=max_speed_rotate;
   kp=error_head*(max_speed_rotate/60); if(kp > max_speed_rotate)kp=max_speed_rotate;
     
   if( ((angle2>0 && angle2<=90) && (Gyro>1 && Gyro<=180))||(angle2>180 && angle2<=270 && Gyro>180 && Gyro<359) ) { p1+= (kp+kd); p2-= (kp+kd); p3-= (kp+kd); p4+= (kp+kd);}
   if( ((angle2>0 && angle2<=90) && (Gyro>180 && Gyro<359))||(angle2>180 && angle2<=270 && Gyro>1 && Gyro<=180) ) { p1-= (kp+kd); p2+=( kp+kd); p3+= (kp+kd); p4-= (kp+kd);}
   if( (angle2>90 && angle2<=180 && Gyro>1 && Gyro<=180)||(angle2>270 && angle2<=360 && Gyro>180 && Gyro<359) ) { p1-= (kp+kd); p2-= (kp+kd); p3+= (kp+kd); p4+= (kp+kd);}
   if( (angle2>90 && angle2<=180 && Gyro>180 && Gyro<359)||(angle2>270 && angle2<=360 && Gyro>1 && Gyro<=180) ) { p1+= (kp+kd); p2+= (kp+kd); p3-= (kp+kd); p4-= (kp+kd);}
       
   if(p1<0) {p1=abs(p1); flg_p1=1;}else{flg_p1=0;}
   if(p2<0) {p2=abs(p2); flg_p2=1;}else{flg_p2=0;}
   if(p3<0) {p3=abs(p3); flg_p3=1;}else{flg_p3=0;}
   if(p4<0) {p4=abs(p4); flg_p4=1;}else{flg_p4=0;}
   if(p1>max_speed_rotate) {p1=max_speed_rotate;}
   if(p2>max_speed_rotate) {p2=max_speed_rotate;}
   if(p3>max_speed_rotate) {p3=max_speed_rotate;}
   if(p4>max_speed_rotate) {p4=max_speed_rotate;}
         
   set_pwm_duty(1,(signed int)p1);
   set_pwm_duty(2,(signed int)p2);
   set_pwm_duty(3,(signed int)p3);
   set_pwm_duty(4,(signed int)p4);


  
   if(coss<0)
   { if(flg_p1==0)output_low(PIN_e15); else{output_high(PIN_e15);}   /*p1*/}
   else
   {if(flg_p1==0)output_high(PIN_e15); else{output_low(PIN_e15);} /*p1*/}
   if(coss<0)
   { if(flg_p3==0)output_high(PIN_c2); else{output_low(PIN_c2);}   /*p3*/}
   else
   {if(flg_p3==0)output_low(PIN_c2); else{output_high(PIN_c2);} /*p3*/}
   if(sinn>=0)
   { if(flg_p2==0)output_low(PIN_a9); else{output_high(PIN_a9);}   /*p2*/}
   else
   {if(flg_p2==0)output_high(PIN_a9); else{output_low(PIN_a9);} /*p2*/}
   if(sinn>=0)
   { if(flg_p4==0)output_low(PIN_e13); else{output_high(PIN_e13);}   /*p4*/}
   else
   {if(flg_p4==0)output_high(PIN_e13); else{output_low(PIN_e13);} /*p4*/}
   
   last_error_head=error_head;
   }

void Set_Head(float Gyro)
{
   int head_speed;
   if(Gyro<180) {error_head=Gyro;} else if(Gyro>=180) {error_head=360-Gyro;}  //set error head
   if(error_head<35){head_speed= error_head*max_speed_rotate/35;}else{ head_speed=max_speed;}
   if(Gyro>1 && Gyro<180)
   {  set_pwm_duty(1,head_speed);  set_pwm_duty(2,head_speed); set_pwm_duty(3,head_speed); set_pwm_duty(4,head_speed); 
      output_low(PIN_e15);    /*p1*/     output_high(PIN_c2);    /*p3*/ output_high(PIN_a9);     /*p2*/     output_low(PIN_e13);   /*p4*/ }
   else if(Gyro>=180 && Gyro<359)
   {  set_pwm_duty(1,head_speed);  set_pwm_duty(2,head_speed); set_pwm_duty(3,head_speed); set_pwm_duty(4,head_speed); 
      output_high(PIN_e15);    /*p1*/     output_low(PIN_c2);    /*p3*/ output_low(PIN_a9);     /*p2*/     output_high(PIN_e13);   /*p4*/ }
   else
   {set_pwm_duty(1,0);  set_pwm_duty(2,0); set_pwm_duty(3,0); set_pwm_duty(4,0); }
}
//min speed = 500      max speed = 2000
void Move_Ball(float distance,float ball_angle)
{
   //printf("ball_angle\r\n    %f",ball_angle);
   //dis+=2;
   float r=4.5, angle2 , zr=0 , zch=0 , speedd=2000 , r_2=(speedd-1000)*4/1000;           //speed=1200      25,335        speed=2000     320,90       dis = 2,3 ||   
   //if(ball_angle<180 && ball_angle>15)r=2;
   //zr=((max_speed_rotate-1200)/8000)+((distance*0.1));                //1200=0.5   2000=1.5     
   //zch=(((max_speed_rotate-1200)*1.3/8000)+(0.11+(distance*0.5)));                //1200=0.5   2000=1.8     
   //zch=(((max_speed_rotate-1200)*1.3/8000)); 
   //if(ball_angle>=25 && ball_angle<180)distance+=zr;
   //if(ball_angle>=335 && ball_angle<180)distance+=zch;
//!      zch=360-(((speedd-500)*37.5/1500)+7.5);
//!      if(speedd<=2000 && speedd>=1200)
//!         zr=((speedd-1200)*65/800)+25;
//!      else
//!         zr=((speedd-500)*18/700)+7;
//!   if(ball_angle<=160 && ball_angle>=80)
//!      {zr-=(65-((ball_angle-100)*65/100)-62); 
//!      if(flg>98){
//!      printf("zr    :%f " ,zr); printf("angle    :%f \r\n" ,ball_angle); flg=0;}}
//!   if(ball_angle<=200 && ball_angle<=320)
//!      zch+=(65-((80-ball_angle)*65/80));
   if(zch>352)zch=352;
   if(zr<7)zr=7;
   distance =20-distance;
   //if(ball_angle>45 && ball_angle<90)ball_angle-=25;
   //if(ball_angle>30 && ball_angle<160)dis-=1.5;
   if(distance==0) distance=0.1;
   if(ball_angle>=338  || ball_angle<=18){set_motor(0,800);}
   else if(ball_angle>=18 && ball_angle<180)
   {
      //dis-=1.5;
      
      angle2=((asin(r/distance))*180)/3.14;
      angle2=ball_angle+angle2;
//!      if(distance<r_2)
//!         set_motor(angle2,1500);
//!      else
         set_motor(angle2,600);
   }
   else
   {
      angle2=((asin(r/distance))*180)/3.14;
      angle2=ball_angle-angle2;
//!      if(distance<r_2)
//!         set_motor(angle2,1500);
//!      else
         set_motor(angle2,600);
   }
}

int Middle_Filter_angle()
{
   int count_s=0 , input ,value[5];
   for(int i=0;i<5;i++)
      value[i]=mid_ball_angle[i];
   for(int j=0;j<3;j++)
   {
      input=value[j];
      count_s=j;
      for(int i=j;i<5;i++)
      {
         if(value[i]<input)
         {
            count_s=i;
            input=value[i];
         }
      }
      value[count_s]=value[j];
      value[j]=input;
   }
   return value[2];
}

int Middle_Filter_distance()
{
   int count_s=0 , input ,value[5];
   for(int i=0;i<5;i++)
      value[i]=mid_distance[i];
   for(int j=0;j<3;j++)
   {
      input=value[j];
      count_s=j;
      for(int i=j;i<5;i++)
      {
         if(value[i]<input)
         {
            count_s=i;
            input=value[i];
         }
      }
      value[count_s]=value[j];
      value[j]=input;
   }
   return value[2];
}

void Kalman_Filter()
{
   int distance_test= Middle_Filter_distance() , direction , error;
   delta_distance=distance_test-last_distance;
   delta_ball_angle=abs(ball_angle-last_ball_angle);
   error=new_distance-(delta_distance+distance_test);
   if(error>1 || error<-1)
      new_distance=delta_distance+distance_test;
   if(last_ball_angle>=ball_angle)
   {
      if((last_ball_angle-ball_angle)<=180)
         direction = -1;    //unc
      else
         direction =  1;   //c
   }
   if(ball_angle<last_ball_angle)
   {
      if((ball_angle-last_ball_angle)<=180)
         direction =  1;   //c
      else
         direction = -1;   //unc
   }
   if(direction == 1)
   {
      error=new_ball_angle-(delta_ball_angle+ball_angle);
      if(error>8 || error<-8)
         new_ball_angle=ball_angle+delta_ball_angle;
   }
   else
   {
      error=new_ball_angle-(ball_angle-delta_ball_angle);
      if(error>8 || error<-8)
         new_ball_angle=ball_angle-delta_ball_angle;
   }
   if(new_ball_angle>=360)new_ball_angle=new_ball_angle-360;
   if(new_ball_angle<0)new_ball_angle=360-new_ball_angle;
   last_distance=distance_test;
   last_ball_angle=ball_angle;
   move_ball(new_distance,new_ball_angle);
//!   if(flg>98)
//!   { printf("1  :%f" ,ball_angle); printf("   2  :%f \r\n" ,new_ball_angle); flg=0;}
}

void main()
{
    //setup_pll();
   
   //setup_timer1(TMR_INTERNAL | TMR_DIV_BY_1, 40000);   
   //setup_timer4(TMR_INTERNAL | TMR_DIV_BY_256, 65535);   //279ms
   //enable_interrupts(INT_TIMER2);
   enable_interrupts(INTR_GLOBAL);
   //N=(1000*MIPS)/(Fpwm*T_DIV) -->>1000=59.95 Khz
   setup_timer1(TMR_INTERNAL | TMR_DIV_BY_1,2000 ); //2000
   setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 5999);
    enable_interrupts(int_rda);
    enable_interrupts(int_timer2);
    enable_interrupts(GLOBAL);
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
    while(Start==1);
    delay_ms(100);
    while(Start==0); 
    while(True)
    {
    //set_timer4(0);
    tis=0;
     BNO055_read();
     New_NJL_read();
     //if(TSSP_INT) 
     //{
        TSSP_read();
        ball_angle=theta;
        //printf("ball_angle:  %d \r\n" ,theta); 
        distance=dis;
        ball_angle-=90; if(ball_angle<0)ball_angle+=360;ball_angle=360-ball_angle;
        mid_ball_angle[counter]=ball_angle;
        //mid_distance[counter]=distance;
        counter++;
        if(counter>=5)counter=0;
//!        if(flg>98)
//!        { printf("ball_angle:%f \r\n" ,ball_angle); printf("distance:%f \r\n" ,distance); flg=0;}
        ball_angle=Middle_Filter_angle();
     //}
     if(ball_angle<-1)flg_enable=1;else{flg_enable=0;}
     if(flg_enable==0)
     move_ball(distance,ball_angle);
     //Kalman_Filter();
     //set_motor(0,0);
     else
     set_motor(0,0);
     //print(out_angle)
//!     if(flg>98)
//!   { printf("ball_angle:%f \r\n" ,ball_angle); printf("distance:%f \r\n" ,distance); flg=0;}
//! if(flg>500)
//!  { printf("%lu\r\n",ti);flg=0;}
//!  ti=tis;
    }
    
    disable;
    //ti=get_timer4();
    
    
}

//!      printf("p1:%f \r\n" ,p1);
//!      printf("p2:%f \r\n" ,p2);
//!      printf("p3:%f \r\n" ,p3);
//!      printf("p4:%f \r\n" ,p4);
//!if(flg>98)
//!{ printf("ball_angle:%f \r\n" ,ball_angle); printf("distance:%f \r\n" ,distance); flg=0;}
