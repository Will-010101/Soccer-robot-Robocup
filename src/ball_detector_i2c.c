

#define BNO055 0x52
#define NJL    0xA0
#define TSSP   0xA2
#define SRF_ADD    0x40
float Yaw=0;
unsigned int8 status; 

unsigned int8 get_ack_status(unsigned int8 address)
{
   unsigned int8 status;
   
   i2c_start();
   status = i2c_write(address); 
   i2c_stop();
    
   if(status==0)  return(TRUE);
   else           return(FALSE);
}

void Search_i2c()
{
   printf("\n\rStart:\n\r");
   delay_ms(1000);
   unsigned int8 count = 0;
   for(unsigned int8 i=0x10; i < 0xF0; i+=2) 
   { 
    status = get_ack_status(i); 
    if(status == TRUE) 
      {  
         printf("Addr:%X\n",i);
         count++; 
         delay_ms(500); 
      } 
   } 
   if(count == 0)  printf("Nothing Found\n"); 
   else            printf("Chips Found=%u\n",count); 
}

void BNO055_Config()
{
i2c_start();                                   
i2c_write(BNO055);                               
i2c_write(0x3E);       // Power Mode                                          
i2c_write(0x00);       // Normal:0X00 (or B00),Low Power: 0X01 (or B01),Suspend Mode: 0X02 (orB10)                                        
i2c_stop();                                                         
delay_ms(100);                                                         

i2c_start();                                                        
i2c_write(BNO055);                                                 
i2c_write(0x3D);       // Operation Mode                   
i2c_write(0x0C);       //NDOF:0X0C (or B1100),IMU:0x08 (or B1000),NDOF_FMC_OFF: 0x0B (or B1011)                                                                                   
i2c_stop();                                                         
delay_ms(100);          //Operation Mode switching time maximum=20ms
}

void BNO055_read()    //reg_addr_Yaw=0x1A
{
i2c_start();
i2c_write(0x52);     // 0x1A :GY955_SLAVE_WRT   
i2c_write(0x1A);       //reg_addr_Yaw=0x1A
i2c_start();
i2c_write(0x53);   // 0x1B ::GY955_SLAVE_RD
Yaw = (int16)(i2c_read()|i2c_read(0)<<8 )/16.00;
i2c_stop();
}
/*
unsigned int16 out_angle;
void NJL_read()  //reg_addr_Yaw=0x1A
{

i2c_start();
i2c_write(NJL+1);   // 0x1B ::GY955_SLAVE_RD
char msb_NJL = i2c_read();      // Read MSB byte first.
char lsb_NJL = i2c_read(0);     // Do a NACK on the last read
i2c_stop();      
out_angle = (int16) (make16(msb_NJL,lsb_NJL));   //in Degrees unit
//return out_angle;

}
*/
//!int8 d_njl[3]={0,0,0};
//!int1 sen[20];
//!unsigned int16 out_detected[20]={0,18,36,54,72,90,108,126,144,162,180,198,216,234,252,270,288,306,324,342};
//!unsigned int16 online_sum=0,online_count=0;
unsigned int8 out_angle;
//!unsigned int32 Symmetry=0;
void New_NJL_read()  //reg_addr_Yaw=0x1A
{

i2c_start();
i2c_write(NJL+1);   
out_angle= i2c_read(0);      
//!d_njl[1]= i2c_read();     
//!d_njl[2]= i2c_read(0);     // Do a NACK on the last read
i2c_stop(); 

//!for(int i=0;i<8;i++)   sen[i]=bit_test(njl[0],i);
//!for(int i=0;i<8;i++)   sen[i]=bit_test(njl[1],i);
//!for(int i=0;i<4;i++)   sen[i]=bit_test(njl[2],i);
//!     
//!for(int i=0;i<20;i++){
//!
//!if(sen[i])  {bit_set(Symmetry,i);if(i>0)online_count++;online_sum+=out_detected[i];} 
//!
//!}
//!        if(online_count>1) //only if we are on line
//!        {        
//!          out_angle=(online_sum/online_count); 
//!          out_angle/=18;
//!          if(bit_test(Symmetry,out_angle)) out_angle=out_detected[out_angle];
//!          else out_angle=out_detected[out_angle]+180;
//!           
//!          if(out_angle >= 360) out_angle-=360;
//!
//!
//!        }
//!        else
//!        {
//!        out_angle=500;Symmetry=0;
//!        }
}




signed int16 theta;
int8 dis;
void TSSP_read()  //reg_addr_Yaw=0x1A
{

i2c_start();
i2c_write(TSSP+1);   // 0x1B ::GY955_SLAVE_RD
char msb_TSSP = i2c_read();      // Read MSB byte first.
char lsb_TSSP = i2c_read();     // Do a NACK on the last read
dis=i2c_read(0);
theta = (int16) (make16(msb_TSSP, lsb_TSSP));   //in Degrees unit
//theta= (i2c_read()|i2c_read(0)<<8 );
i2c_stop();      
//Yaw = (int16) (make16(msb, lsb)/16.00);   //in Degrees unit
}
 
int8 SRF[3];
void SRF_read()  //reg_addr_Yaw=0x1A
{

i2c_start();
i2c_write(SRF_ADD+1);   
SRF[0] = i2c_read();      // Read MSB byte first.
SRF[1] = i2c_read();      // Read MSB byte first.
SRF[2] = i2c_read(0);      // Read MSB byte first.
i2c_stop();      
}

unsigned int16 x=0,y=0;
void get_position()
{
y=0;
 if(srf[0]>15 && srf[2]>15)            // is not robot Near
 {
    if(srf[0] <= srf[2]) x = srf[0]+9;
     else                 x = 180 - srf[2]-9;
 }
 else if(srf[0]>15) x = srf[0]+9;        // Left side is Robot
 else if(srf[2]>15) x = 180 - srf[2]-9;  // Right side is Robot
 else x = 200;


}

int16 p[4]={500,500,500,500};

void motor_test(int16 p1,int16 p2,int16 p3,int16 p4)
{
   output_high(PIN_e15);    set_pwm_duty(1,p[0]);        //up right   
   output_high(PIN_a9);     set_pwm_duty(2,p[1]);         //up left
   output_high(PIN_c2);     set_pwm_duty(3,p[2]);         //down left
   output_high(PIN_e13);    set_pwm_duty(4,p[3]);        //down right
   
}

