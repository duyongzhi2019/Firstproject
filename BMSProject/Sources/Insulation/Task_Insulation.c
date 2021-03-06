 /*=======================================================================
 *Subsystem:   裸机
 *File:        Insulation.c
 *Author:      WenMing
 *Description: 接口
               电压检测开关：               PB0                 
               绝缘V+开关：                   PB1
               绝缘V-开关：                   PB2
               绝缘电压隔离检测开关正：       PB3
               绝缘电压隔离检测开关正：       PB4
               总压检测                       PAD00
               V+电压检测:                    PAD01
               V-电压检测:                    PAD08
               采样频率:                      2MHz
 =============================================================================
 * History:    修改历史记录列表
 * 1. Date:    
      Author:  
      Modification:
===============================================================================*/
 
 
  #include"Task_Insulation.h"

  IsoResist IsoDetect;        //定义绝缘检测结构体
/*=======================================================================
 *函数名:      VoltageDetect
 *功能:        电池包电压检测初始化（总压检测，绝缘检测）
 *参数:        无         
                      
 *返回：       无
 *说明：       ADC 时钟频率：2MHz
========================================================================*/
  void Insulation_Init(void) 
  {
     HighVoltS1_Dir = 1;
     HighVoltS1 = INS_SwitchOFF;      //绝缘检测V+开关关闭
     
     HighVoltS2_Dir = 1; 
     HighVoltS2 = INS_SwitchOFF;      //绝缘检测V-开关关闭
  }

/*=======================================================================
 *函数名:      InsulationDetect
 *功能:        电池包绝缘检测
 *参数:        无         
                      
 *返回：       无
 *说明：       ADC 时钟频率：2MHz
========================================================================*/

      uint32 SumVpositive,SumVnegtive,total_VOL,total_VOL1;
      float Vpositive_1,Vnegtive_1;
      uint16 VposBuff[12],VnegBuff[12],VposBuff1[12],VnegBuff1[12],VposBuff2[12],VnegBuff2[12];
      uint16 Max_Volt,Max_Volt1,Min_Volt,Min_Volt1;
      //uint16 QWE,QWE1;
  void Task_InsulationDetect(void) 
  {
      uint8 count,i,j;
      static uint8 Time_Flag,Time_Cnt;
      //uint32 SumVpositive,SumVnegtive,total_VOL,total_VOL1;
      static uint16 VoltFlag1,VoltFlag2;
      //float Vpositive_1,Vnegtive_1;
      //uint16 VposBuff[12],VnegBuff[12],VposBuff1[12],VnegBuff1[12],VposBuff2[12],VnegBuff2[12];
      //uint16 Max_Volt,Max_Volt1,Min_Volt,Min_Volt1;
      
      Insulation_Init();
      //QWE = ADCValue(HVPositiveChannel);
      //QWE1 = ADCValue(HVNegtiveChannel);
      if(Time_Flag == 0) 
      { 
        Time_Cnt = 0;           
        for(count = 0; count < 12; count++)//测试12次再去掉最高值和最低值 
        {
          VposBuff[count] = ADC_Value(HVPositiveChannel);   //正对底盘：PAD1
          
          VnegBuff[count] = ADC_Value(HVNegtiveChannel);    //负对底盘：PAD8
  
        }    
        
        Max_Volt = 0;                 
        Max_Volt1 = 0; 
        Min_Volt = 0xFFFF;
        Min_Volt1 = 0xFFFF;
        SumVpositive =0;
        SumVnegtive =0;
        
        for(i =0; i<12; i++)            // 查找每一个数组中的最大最小值(12组)
        {
           if(VposBuff[i] >=Max_Volt)
           {
              Max_Volt = VposBuff[i];
           } 
           if(VposBuff[i] <= Min_Volt)
           {
              Min_Volt = VposBuff[i];
           }
           
           if(VnegBuff[i] >Max_Volt1)
           {
              Max_Volt1 = VnegBuff[i];
           } 
           if(VnegBuff[i] < Min_Volt1)
           {
              Min_Volt1 = VnegBuff[i];
           }
        }
        
        for(count = 0; count < 12; count++)      //把所有值全部加起来 //
        {
           SumVpositive += VposBuff[count];
           SumVnegtive += VnegBuff[count];
        }
                
        SumVpositive = SumVpositive - Max_Volt - Min_Volt;
        SumVnegtive = SumVnegtive - Max_Volt1 - Min_Volt1; 
                
        IsoDetect.insulation_Vposit = SumVpositive/10.0;
        IsoDetect.insulation_Vnegt = SumVnegtive/10.0;               // 去除最大最小值之后求平均值
                
        SumVpositive =0;
        SumVnegtive =0;
      
        IsoDetect.insulation_TotalVolt = ((IsoDetect.insulation_Vposit + IsoDetect.insulation_Vnegt) * Stand_Volt)/4096.0/100.0*5100.0;
      }
       
      if(IsoDetect.insulation_TotalVolt <= LowVoltageDec)            // 判断总压值是否需要进行绝缘检测
      {
          IsoDetect.insulation_grade = 0x00;
          IsoDetect.insulation_curr = 0xaa;
          IsoDetect.insulation_resist = 0x1388;    //按通信协议 5M
      } 
      else
      {                                            // 当电压高于LowVoltageDec大于阈值         
         if(IsoDetect.insulation_Vposit >= IsoDetect.insulation_Vnegt) //正极>=负极
         {           
           Time_Flag = 1;                
           if(Time_Cnt == 0)
           {
             HighVoltS1 = INS_SwitchON;                // 绝缘检测V+开关闭合,闭合后停一段时间在检测电压值，延迟时间待定  //
             HighVoltS2 = INS_SwitchOFF;
                            // 绝缘检测V-开关断开 //
           }
          
           if(Time_Cnt >= 1) 
           {             
             Time_Flag = 0;
             Time_Cnt = 0;
                                      
             for(count = 0; count < DetectCount; count++)       // 因为只要正极的电压，负极是否可以去掉 //
             {
                VposBuff1[count] = ADC_Value(HVPositiveChannel);   //正对底盘：PAD1
                VnegBuff1[count] = ADC_Value(HVNegtiveChannel);    //负对底盘：PAD8
               
             }           
              Max_Volt = 0;                 
              Max_Volt1 = 0; 
              Min_Volt = 0xFFFF;
              Min_Volt1 = 0xFFFF;
              for(i =0; i< DetectCount;i++)            // 查找每一个数组中的最大最小值 //
              {
                 if(VposBuff1[i] >Max_Volt)
                 {
                    Max_Volt = VposBuff1[i];
                 } 
                 if(VposBuff1[i] < Min_Volt)
                 {
                    Min_Volt = VposBuff1[i];
                 }
                 
                 if(VnegBuff1[i] >Max_Volt1)
                 {
                    Max_Volt1 = VnegBuff1[i];
                 } 
                 if(VnegBuff1[i] < Min_Volt1)
                 {
                    Min_Volt1 = VnegBuff1[i];
                 }
              }
              
              for(count = 0; count < 12; count++)           // 把所有值全部加起来 //
              {
                 SumVpositive += VposBuff1[count];
                 SumVnegtive += VnegBuff1[count];
                 
              }                                             // 减去最大最小值，求十个数的平均值//
              SumVpositive = SumVpositive - Max_Volt - Min_Volt;
              SumVnegtive = SumVnegtive - Max_Volt1 - Min_Volt1; 
              
              Vpositive_1 = SumVpositive/10.0;
              Vnegtive_1 = SumVnegtive/10.0;                // 去除最大最小值之后求平均值// 
              
              
              SumVpositive=0;
              SumVnegtive=0;    
              
              HighVoltS1 = INS_SwitchOFF;
              HighVoltS2 = INS_SwitchOFF; 
              if(IsoDetect.insulation_Vposit<0.1)
              {
                IsoDetect.insulation_resist = (uint16)0x1388;   //负极电阻
                IsoDetect.insulation_resist_N = (uint16)0x1388; //负极电阻
                IsoDetect.insulation_resist_P = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)*1.0/Vpositive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);//正极电阻 
              }
              else if(IsoDetect.insulation_Vnegt<0.1)
              {
                IsoDetect.insulation_resist = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)/Vpositive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
                IsoDetect.insulation_resist_N = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)*1.0/Vpositive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
                IsoDetect.insulation_resist_P = (uint16)0x1388; //正极电阻 
              }
              else
              {
                IsoDetect.insulation_resist = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)/Vpositive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
                IsoDetect.insulation_resist_N = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)*1.0/Vpositive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
                IsoDetect.insulation_resist_P = (uint16)(abs(IsoDetect.insulation_Vposit-Vpositive_1)*1.0/Vpositive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);//正极电阻 
              }
           }
         }  
          
         else                                   //正极<负极
         {  
           Time_Flag = 1; 
           if(Time_Cnt == 0)
           {                    
             HighVoltS1 = INS_SwitchOFF;              //V+开关断开
             HighVoltS2 = INS_SwitchON;               //V-开关闭合
           }
           if(Time_Cnt>0) 
           {         
             Time_Flag = 0;
             Time_Cnt = 0;
           
             for(count = 0; count < DetectCount; count++)    // 只要了负极的电压，正极是否可以去掉 //
             {
               VposBuff2[count] = ADC_Value(HVPositiveChannel);  //正对底盘：PAD1
               VnegBuff2[count] = ADC_Value(HVNegtiveChannel);   //负对底盘：PAD8            
             }  
                        
              Max_Volt = 0;                 
              Max_Volt1 = 0; 
              Min_Volt = 0xFFFF;
              Min_Volt1 = 0xFFFF;
              for(i =0; i< DetectCount;i++)            // 查找每一个数组中的最大最小值 //
              {
                 if(VposBuff2[i] >Max_Volt)
                 {
                    Max_Volt = VposBuff2[i];
                 } 
                 if(VposBuff2[i] < Min_Volt)
                 {
                    Min_Volt = VposBuff2[i];
                 }
                 
                 if(VnegBuff2[i] >Max_Volt1)
                 {
                    Max_Volt1 = VnegBuff2[i];
                 } 
                 if(VnegBuff2[i] < Min_Volt1)
                 {
                    Min_Volt1 = VnegBuff2[i];
                 }
              }
              
              for(count = 0; count < 12; count++)      // 把所有值全部加起来 //
              {
                 SumVpositive += VposBuff2[count];
                 SumVnegtive += VnegBuff2[count];
             
              }
                                            // 减去最大最小值，求十个数的平均值 //
              SumVpositive = SumVpositive - Max_Volt - Min_Volt;
              SumVnegtive = SumVnegtive - Max_Volt1 - Min_Volt1; 
              Vpositive_1 = SumVpositive/10.0;
              Vnegtive_1 = SumVnegtive/10.0;              // 去除最大最小值之后求平均值// 
              
              
              HighVoltS1 = INS_SwitchOFF;
              HighVoltS2 = INS_SwitchOFF; 
              
              
              SumVpositive=0;
              SumVnegtive=0;
              if(IsoDetect.insulation_Vposit<0.1)
              {
                IsoDetect.insulation_resist = (abs(IsoDetect.insulation_Vposit-Vnegtive_1)*1.0/Vnegtive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);      
                IsoDetect.insulation_resist_P = (uint16)(abs(IsoDetect.insulation_Vnegt-Vnegtive_1)*1.0/Vnegtive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);//正极电阻
                IsoDetect.insulation_resist_N = (uint16)0x1388;
              }
              else if(IsoDetect.insulation_Vnegt<0.1)
              {
                IsoDetect.insulation_resist = (uint16)0x1388;      
                IsoDetect.insulation_resist_P = (uint16)0x1388;//正极电阻
                IsoDetect.insulation_resist_N = (uint16)(abs(IsoDetect.insulation_Vnegt-Vnegtive_1)/Vnegtive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
              }
              else
              {                
                IsoDetect.insulation_resist = (abs(IsoDetect.insulation_Vnegt-Vnegtive_1)*1.0/Vnegtive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);    //Vposit改为Vneg  
                IsoDetect.insulation_resist_P = (uint16)(abs(IsoDetect.insulation_Vnegt-Vnegtive_1)*1.0/Vnegtive_1*(1+IsoDetect.insulation_Vposit/IsoDetect.insulation_Vnegt) * Bias_Resitance);//正极电阻
                IsoDetect.insulation_resist_N = (uint16)(abs(IsoDetect.insulation_Vnegt-Vnegtive_1)/Vnegtive_1*(1+IsoDetect.insulation_Vnegt/IsoDetect.insulation_Vposit) * Bias_Resitance); //负极电阻
              }
           }         
        }          
      //判断绝缘故障
      if(IsoDetect.insulation_resist > 0x1388)                   //5M
      {
        IsoDetect.insulation_grade = 0x00;
        IsoDetect.insulation_curr = 0xaa;
        IsoDetect.insulation_resist = 0x1388;                    //按通信协议 5M
      }    
      else 
      {
          if(IsoDetect.insulation_resist > Resistance_Alarm1)       //无故障
          {
            IsoDetect.insulation_grade = 0x00;
            IsoDetect.insulation_curr = 0xaa;      
          } 
          if(IsoDetect.insulation_resist < Resistance_Alarm2)       //最高绝缘故障
          {
            IsoDetect.insulation_grade = 0x02;
            IsoDetect.insulation_curr = 0x55;
          }
          else                                                      //次级故障
          {
            IsoDetect.insulation_grade = 0x01;
            IsoDetect.insulation_curr = 0x55;
          }   
      }  
    }   
    
    Time_Cnt++;      
  }