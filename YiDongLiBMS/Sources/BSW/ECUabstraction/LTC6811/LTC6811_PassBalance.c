/*=======================================================================
 *Subsystem:   ���
 *File:        LTC6811_PassBalance.C
 *Author:      WenM
 *Description: ͨ�ţ�
               �ӿڣ�
               �����ʣ�
 ========================================================================
 * History:    �޸���ʷ��¼�б�
 * 1. Date:   
      Author:  
      Modification:  
      
========================================================================*/
#include  "LTC6811_PassBalance.h"               

uint8 Balance_CFGR[NUM_IC][6];
/*=======================================================================
 *������:      Balance_Config();
 *����:        ��LTC6804������
 *����:        ��       
 *���أ�       ��
 *˵����       IC=3;GPIO=0XFF; REFON=1; SWTEN=0;  ADCOPT=0;  VUV=3.2; VOV=4.2; DCTO=0;
========================================================================*/
void LTC6811_BalanceCMDSend( uint8 *CFG,uint8 gpio, uint8 refon,uint8 swtrd, uint8 adcopt,
                 float  vuv,float   vov,uint16 ddc,uint8 dcto) 
{           
  uint8 char1,char2,char3;
  uint8 current_ic;                             /* ��LTC6804_1�ĳ�ʼ���������ò��� */

  *CFG++=(gpio<<3)+(refon<<2)+(swtrd<<1)+adcopt;
  
  *CFG++=(uint8)(((uint16)vuv*10000/16-1)&0x00ff);
  
  char1=(uint8)((((uint16)vov*10000/16)&0x000f)<<4);

  char2=(uint8)((((uint16)vuv*10000/16-1)&0x0f00)>>8);
   
  *CFG++=char1+char2;
  
  *CFG++= (uint8)(((uint16)(vov*10000/16)&0x0ff0)>>4);
  
  *CFG++= ddc;
  
  char3 = (uint8)(ddc>>8);
  
  char3 = char3 &0x0f; 
 
  *CFG++=(dcto<<4)+ char3; 
}
/*=======================================================================
 *������:      Passive_Balance��void��
 *����:        �Ե������б�������
 *����:        state1����1��оƬ�ľ���״̬��Χ��1~12
               state2����2��оƬ�ľ���״̬��Χ��1~12
               state3����3��оƬ�ľ���״̬��Χ��1~12
               time  : ����ʱ�䳤��
 *���أ�       ��
 *˵����       �������⺯��
========================================================================*/
uint8 LTC6811_BalanceControl(uint16 state1, uint16 state2, uint16 state3, uint8 time) 
{
  uint16 state[3]={0,0,0};
  if((state1<1||state1>NUM_Battery)||(state2<1||state2>NUM_Battery)||((state3<1||state3>NUM_Battery)))
  {
    return 1;
  }
  state[1] = 1<<(state1-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[1] = 1<<(state1-1+6-NUM1_Batper_front);
  }
  state[2] = 1<<(state2-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[2] = 1<<(state2-1+6-NUM1_Batper_front); 
  }
  state[3] = 1<<(state3-1);
  if(state[1]>NUM1_Batper_front)
  {
    state[3] = 1<<(state3-1+6-NUM1_Batper_front); 
  }
  
  LTC6811_BalanceCMDSend(&Balance_CFGR[0][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[1], time) ;   /* ����ֵ�����ṹ��,���������������ֵ��*/                                           

  LTC6811_BalanceCMDSend(&Balance_CFGR[1][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[2], time) ;   /* ����ֵ�����ṹ��,���������������ֵ��*/  

  LTC6811_BalanceCMDSend(&Balance_CFGR[2][0], DGPIO, DREFON, DSWTRD, DADCOPT, UNDER_V, OVER_V, state[3], time) ;   /* ����ֵ�����ṹ��,���������������ֵ��*/  

  LTC6804_wrcfg(NUM_IC, Balance_CFGR);
  
  return 0; 
}


