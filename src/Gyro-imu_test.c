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


void main()
{
    setup_pll();
    delay_ms(500);
    BNO055_Config();
    delay_ms(500);
    while(True){BNO055_read(); printf("Gyro:%f \r\n" ,Yaw);}
}


