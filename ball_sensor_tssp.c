#Include<33ep128gp506.h>
#include <math.h>
#include<ConfigureClock.h>
#define led1_high  output_high(pin_c2)
#define led1_low   output_low(pin_c2)
#define led1       output_toggle(pin_c2)
//!#define led2_high  output_high(pin_c1)
//!#define led2_low   output_low(pin_c1)
 //!#define led3_high  output_high(pin_c0)
//!#define led3_low   output_low(pin_c0)
//!#define led4_high  output_high(pin_b3)
//!#define led4_low   output_low(pin_b3)
//!#define i2c_done   output_high(pin_c13)
//!#define i2c_busy   output_low(pin_c13)
#define TSSP_ON    output_high(pin_b2);
#define TSSP_OFF   output_low(pin_b2);
//#pin_select
#pin_select U1TX = PIN_C6
#pin_select U1RX = PIN_c7   //no rx//just a useless pin 
#use rs232(UART1, baud = 9600)
#use i2c(SLAVE, I2C1, address = 0xA2, stream = I2C_PORT1)
#use timer(timer = 1, tick = 1us, bits = 16, isr)


signed int16 Pulsewidth[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, start_time[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, TSSP[16], max = 0, min = 1000, theta = 0;
signed int16 *P_Pulsewidth = Pulsewidth, *P_start_time = start_time, *P_TSSP = TSSP, *P_max = &max, *P_min = &min, *P_theta = &theta;
int1 Pre_Edge[16] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 }, TimeOut = 0;
float Ax[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, Ay[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }, totalx = 0, totaly = 0, dis1 = 0;
float *P_Ax = Ax, *P_Ay = Ay, *P_totalx = &totalx, *P_totaly = &totaly, *P_dis1 = &dis1;
unsigned int8 noball = 0, dis = 0, data[3] = { 255,255,255 }, done[16], flgg = 0;
unsigned int8 *P_noball = &noball, *P_dis = &dis, *P_data = data, *P_done = done, *P_flgg = &flgg;
int8 i_max = 0, i_min = 0, ledc;
int8 *P_i_max = &i_max, *P_i_min = &i_min, *p_ledc = &ledc;

#INT_TIMER2
void  timer2_isr(void)
{
    *P_flgg += 1;
    TimeOut = 1;
}

#INT_SI2C
void SI2C_interrupt()
{
    int state = i2c_isr_state();

    if (state < 0x80)     // Master is sending data
    {
        i2c_read();
    }

    if (state >= 0x80)    // Master is requesting data from slave 
    {
        i2c_write(data[state - 0x80]);
        if (state == 0x82) { output_drive(pin_C13);; output_low(PIN_C13); delay_us(10); }
    }

}


void read_tssp()
{
    *P_TSSP = input(pin_b1);
    *(P_TSSP + 1) = input(pin_e15);
    *(P_TSSP + 2) = input(pin_a8);
    *(P_TSSP + 3) = input(pin_b7);
    *(P_TSSP + 4) = input(pin_b10);
    *(P_TSSP + 5) = input(pin_b11);
    *(P_TSSP + 6) = input(pin_b12);
    *(P_TSSP + 7) = input(pin_b13);
    *(P_TSSP + 8) = input(pin_a10);
    *(P_TSSP + 9) = input(pin_a7);
    *(P_TSSP + 10) = input(pin_b14);
    *(P_TSSP + 11) = input(pin_b15);
    *(P_TSSP + 12) = input(pin_g6);
    *(P_TSSP + 13) = input(pin_a0);
    *(P_TSSP + 14) = input(pin_a1);
    *(P_TSSP + 15) = input(pin_b0);
}



void angle_of_ball()
{
    disable_interrupts(INTR_GLOBAL);
    *P_Ax = pulsewidth[0];
    *P_Ay = 0;
    *(P_Ax + 1) = pulsewidth[1] * 0.92;   //cos22.5=0.92
    *(P_Ay + 1) = pulsewidth[1] * 0.38;   //sin22.5=0.38
    *(P_Ax + 2) = pulsewidth[2] * 0.70;   //cos45=0.7
    *(P_Ay + 2) = pulsewidth[2] * 0.70;
    *(P_Ax + 3) = pulsewidth[3] * 0.38;   //cos67.5=0.38
    *(P_Ay + 3) = pulsewidth[3] * 0.92;   //sin67.5=0.92
    *(P_Ax + 4) = 0;
    *(P_Ay + 4) = pulsewidth[4] * 1;
    *(P_Ax + 5) = pulsewidth[5] * (-0.38);
    *(P_Ay + 5) = pulsewidth[5] * 0.92;
    *(P_Ax + 6) = pulsewidth[6] * (-0.70);
    *(P_Ay + 6) = pulsewidth[6] * 0.70;
    *(P_Ax + 7) = pulsewidth[7] * (-0.92);
    *(P_Ay + 7) = pulsewidth[7] * 0.38;
    *(P_Ax + 8) = pulsewidth[8] * (-1);
    *(P_Ay + 8) = 0;
    *(P_Ax + 9) = pulsewidth[9] * (-0.92);
    *(P_Ay + 9) = pulsewidth[9] * (-0.38);
    *(P_Ax + 10) = pulsewidth[10] * (-0.70);
    *(P_Ay + 10) = pulsewidth[10] * (-0.70);
    *(P_Ax + 11) = pulsewidth[11] * (-0.38);
    *(P_Ay + 11) = pulsewidth[11] * (-0.92);
    *(P_Ax + 12) = 0;
    *(P_Ay + 12) = pulsewidth[12] * (-1);
    *(P_Ax + 13) = pulsewidth[13] * (0.38);
    *(P_Ay + 13) = pulsewidth[13] * (-0.92);
    *(P_Ax + 14) = pulsewidth[14] * (0.70);
    *(P_Ay + 14) = pulsewidth[14] * (-0.70);
    *(P_Ax + 15) = pulsewidth[15] * (0.92);
    *(P_Ay + 15) = pulsewidth[15] * (-0.38);
    int i = 0;
    for (int i = 0; i < 16; i++)
    {

        if (*P_i_max == i) { *P_totalx = (*(P_Ax + i) * 8) + *P_totalx; *P_totaly = (*(P_Ay + i) * 8) + *P_totaly; }
        else if ((*P_i_max + 1) == i)
        {
            if (*(P_pulsewidth + *P_i_max + 1) > *(pulsewidth + *P_i_max - 1))
            {
                *P_totalx = (*(P_Ax + i) * 5) + *p_totalx; *P_totaly = (*(P_Ay + i) * 5) + *P_totaly;
            }
            else
            {
                *P_totalx = (*(P_Ax + i) * 4) + *P_totalx; *P_totaly = (*(P_Ay + i) * 4) + *P_totaly;
            }
        }
        else if ((*P_i_max - 1) == i)
        {
            if (*(pulsewidth + *P_i_max - 1) - 1 > *(P_pulsewidth + *P_i_max + 1))
            {
                *P_totalx = (*(P_Ax + i) * 5) + *P_totalx; *P_totaly = (*(P_Ay + i) * 5) + *P_totaly;
            }
            else
            {
                *P_totalx = (*(P_Ax + i) * 4) + *P_totalx; *P_totaly = (*(P_Ay + i) * 4) + *P_totaly;
            }
        }

        else if ((*P_i_max - 2) == i || (*P_i_max + 2) == i)
        {
            *P_totalx = (*(P_Ax + i) * 2) + *P_totalx; *P_totaly = (*(P_Ay + i) * 2) + *P_totaly;
        }
        else { *P_totalx = (*(P_Ax + i) * 1) + *P_totalx; *P_totaly = (*(P_Ay + i) * 1) + *P_totaly; }
    }

    *P_theta = (atan2(*P_totaly, *P_totalx) * 57.5);
    if (*P_theta < 0) *P_theta += 360;

    for (int i = 0; i < 16; i++)
    {
        if (*P_i_max == i) { *P_totalx = (*(P_Ax + i) * 1) + *P_totalx; *P_totaly = (*(P_Ay + i) * 1) + *P_totaly; }
        else { *P_totalx = *(P_Ax + i) + *P_totalx; *P_totaly = *(P_Ay + i) + *P_totaly; }

    }

    *P_dis1 = sqrt((*P_totalx * *P_totalx) + (*P_totaly * *P_totaly));  //distance
    if (*P_dis1 > 10000)*P_dis1 = 10000;
    *P_dis = (1 + (((10 - 1) / (9500 - 1000)) * (*P_dis1 - 1000)));
    *P_totalx = 0;
    *P_totaly = 0;
    enable_interrupts(INTR_GLOBAL);
}



void main()
{

    setup_pll();
    // setup_timer1(TMR_INTERNAL | TMR_DIV_BY_64, 10000); //overfolow 10 ms
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_64, 2500);    //overfolow 1 ms
    enable_interrupts(INT_TIMER2);
    enable_interrupts(INT_SI2C);
    enable_interrupts(INTR_GLOBAL);

    output_low(PIN_C13);
    output_float(PIN_C13);

    led1_high; delay_ms(500);
    led1_low; delay_ms(100);
    TSSP_ON;
    set_timer2(0);

    while (true)
    {

        //  printf("%d , %u \r\n",theta,dis); //Monitoring Theta And Distance
        if ((ledc++) > 50) Led1;

        *P_theta = 0; set_timer2(0); timeout = 0; for (int8 i = 0; i < 16; i++) *(p_done + i) = 0;

        while ((*P_done < 3 || *(p_done + 1) < 3 || *(p_done + 2) < 3 || *(p_done + 3) < 3 || *(p_done + 4) < 3 || *(p_done + 5) < 3 || *(p_done + 6) < 3 || *(p_done + 7) < 3 || *(p_done + 8) < 3 || *(p_done + 9) < 3 ||
            *(p_done + 10) < 3 || *(p_done + 11) < 3 || *(p_done + 12) < 3 || *(p_done + 13) < 3 || *(p_done + 14) < 3 || *(p_done + 15) < 3) && timeout == 0)
        {
            read_tssp();
            for (int8 i = 0; i < 16; i++)
            {
                if (*(P_TSSP + i) == 1 && *(p_done + i) == 0) { *(p_done + i) = 1; }

                else if (*(P_TSSP + i) == 0 && *(p_done + i) == 1)
                {
                    *(P_start_time + i) = get_ticks();
                    Pre_edge[i] = 0; *(p_done + i) = 2;
                }
                else if (*(P_TSSP + i) == 1 && *(p_done + i) == 2)
                {
                    *(P_pulsewidth + i) = get_ticks(); // End time 
                    Pre_edge[i] = 1;
                    if (*(P_pulsewidth + i) < *(P_start_time + i)) { *(P_pulsewidth + i) = 65535 - (*(P_start_time + i) - *(P_pulsewidth + i)); }
                    else *(P_pulsewidth + i) -= *(P_start_time + i);
                    *(p_done + i) = 3;
                }

            }
        }
        *P_noball = 0;
        for (int i = 0; i <= 15; i++) { if (*(p_done + i) < 3) { *(P_pulsewidth + i) = 0; *P_noball++; } }

        if (*P_noball > 15) { *P_theta = 500; *P_dis1 = 0; }
        else
        {
            *P_max = 0, *P_min = 1000;

            for (int i = 0; i < 16; i++)
            {
                if (*(P_pulsewidth + i) > *P_max) { *P_max = *(P_pulsewidth + i); *P_i_max = i; }
                if (*(P_pulsewidth + i) < *P_min) { *P_min = *(P_pulsewidth + i); *P_i_min = i; }
            }

            angle_of_ball();
        }


        if (*P_dis > 10) *P_dis = 10;
        *(P_data) = make8(*P_theta, 1);
        *(P_data + 1) = make8(*P_theta, 0);
        *(P_data + 2) = *P_dis;
        *P_max = 0, *P_min = 1000;
        output_float(PIN_C13);
                    if(*P_flgg>98)
                    {
                         printf("dis:   %f \r\n" ,dis1); 
                         printf("ball_angle:   %d \r\n" ,theta); 
                          *P_flgg=0;
                    }
    }
}
