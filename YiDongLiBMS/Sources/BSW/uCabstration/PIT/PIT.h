/*=======================================================================
 *Subsystem:   ���
 *File:        PIT.h
 *Author:      WenYuhao
 *Description: 
 ========================================================================
 * History:    
 * 1. Date:
      Author:
      Modification:
========================================================================*/

#ifndef _PIT_H_
#define _PIT_H_  

  #include  "TypeDefinition.h"
  #include  "MC9S12XEP100.h"

//���Ż��ֹPIT0����ж�
    #define EnablePIT(channel)           PITINTE |= (1<<channel)    //���Ŷ�ʱ������ж�
    #define DisablePIT(channel)          PITINTE &= ~(1<<channel)  //��ֹ��ʱ������ж�

  //PIT��ʼ��
  enum 
  {
    //PIT��ʼ������
    Init_Normal_PIT = 0,
    Init_Fault_PIT_Channel,
    Init_Fault_PIT_Period
  };

  typedef struct 
  {
    //uint16 T10ms;       // 10ms����ʱ���־;
    uint16 T500ms;      // 500ms����ʱ���־;
    //uint16 T1s;         // 1s����ʱ���־; 
  }PIT_TimePeriod_T;
  extern PIT_TimePeriod_T PIT_TimePeriod;  

  void PITInit(uint8 channel,uint8 MUXSEL,uint8 MTLD,uint16 LD);

#endif                                                                           