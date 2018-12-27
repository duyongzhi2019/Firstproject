#include "SPI.h"

/*=======================================================================
 *������:      SPI1_Init(void) 
 *����:        SPI1��ʼ��
 *����:        ��       
 *���أ�       ��
 *˵����       ͨ�Ų�����:1Mhz
               SPI��ʼ��������ҪƬѡ
========================================================================*/
uint8 SPI1_Init(void) 
{

   SS2_Dir = 1;  //SPIƬѡ
   
   SPI1CR1=0b01011100;
   SPI1CR2=0x00;
   SPI1BR=0x71; //1MHz������
 
   SS2 = 1;
   return 0;
}
                           
/*=======================================================================
 *������:      SPI1_Write(uint8) 
 *����:        SPI1д���ݺ���
 *����:        data��Ҫд��8λ����       
 *���أ�       ��
 *˵����       SPI1д���ݺ���
========================================================================*/
void SPI1_Write(uint8 data) 
{
  while(!SPI1SR_SPTEF);                // ÿ��ֻ�ܷ���һ���ֽڵ����ݣ�
  SPI1DRL = data;
}

/*=======================================================================
 *������:      SPI1_read(void) 
 *����:        SPI1�����ݺ���
 *����:        ��       
 *���أ�       ��
 *˵����       SPI1�����ݺ���
========================================================================*/
uint8 SPI1_read(void) 
{
  uint8 temp2,returndata;
 
  while(!SPI1SR_SPIF) 
  temp2 = SPI1SR;   
  
  returndata = SPI1DRL;           //��ȡ����
  return(returndata); 
}


/*=======================================================================
 *������:      wakeup_idle(void) 
               wakeup_sleep(void)
 *����:        SPI1���Ƶ�����������SPI1���Ƶ���������
 *����:        ��       
 *���أ�       ��
 *˵����       SPI1���Ƶ���������SPI1���Ƶ���������
========================================================================*/
void wakeup_idle(void)
{
  SS2=0;
  delay_time(5);                             // Guarantees the isoSPI will be in ready mode
  SS2=1;
} 

void wakeup_sleep(void)
{
  SS2 = 0;
  delay_time(5);                             // Guarantees the LTC6804 will be in standby
  SS2 =1;
}

/*=======================================================================
 *������:      delay_time(uint16) 
 *����:        ��ʱ����
 *����:        t����ʱ��������       
 *���أ�       ��
 *˵����       �˺������ڻ��������С�д���ݺͶ����ݵĵȴ�ʱ����
========================================================================*/
void delay_time(uint16 t)
{
  uint16 i;
  for(i=0; i<t; i++);
}

    
 