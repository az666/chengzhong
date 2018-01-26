//添加了温度模块，计划加入温度线性漂移。
//晚上已成功搞定滤波算法
#include <HX711.h>
#include "DS18B20_S.h"
#include <EEPROM.h>        //寄存器库
//******************************************************************
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)//中断延时
#define ulong unsigned long
//******************************************************************
#define A 10//滤波偏移量 
//******************************************************************
int i=0;
 unsigned char *dpointer;//寄存器文件-结构体存储浮点型数据
union data {
  double v;
  unsigned char dchar[8];
} dvalue;
//******************************************************************

HX711 hx(A0, A1, 128);
DS18B20_S  ds(9);//pin9
//**********************************************************
void delay_(int ms)//自制软件延时
{
 for(int i=0; i<ms; i++)
 {
 for(ulong j=0;j<1985;j++) NOP;
 } 
}
double jiance (void)//检测函数
{
 Serial.println("begin...jiance"); 
 for(i = 0; i < 8; i++) dvalue.dchar[i] = EEPROM.read(i);
 Serial.print(dvalue.v,10); 
 delay_(500);
 if(dvalue.v>1)
 { 
 jiaozhun();
 return dvalue.v;
 }
 else
 {
  return dvalue.v;
  }
 Serial.println("gogogo"); //进入称量状态
  }
//************************************************************** 
  double pingjunzhi (void)//求采集10次数据的均值函数
{
double zhi;
double sum = 0;    // 为了减小误差，一次取出10个值后求平均值。
for (int i = 0; i < 10; i++) // 循环的越多精度越高，当然耗费的时间也越多
sum += hx.read();  // 累加
zhi=sum/10;
return zhi;   
  }
//************************************************************
double pianyiliang(void)//偏移量的计算函数
  {
  double pyl;
double sum = 0;    // 为了减小误差，一次取出10个值后求平均值。
for (int i = 0; i <20; i++) // 循环的越多精度越高，当然耗费的时间也越多
sum += hx.read();  // 累加
pyl=sum/20;
  Serial.print(pyl);
return pyl;    
}
//***********************************************
void zhongduan()//外部中断函数
  {   
jiaozhun();
double x = jiance();
hx.set_co(x); //修改修正系数
    }
//***********************************************
double jiaozhun()//校准函数
{
  double jieguo1;
 double jieguo2;
 double jieguo3;
 pingjunzhi();//空走一遍初始化校准程序
jieguo1=pingjunzhi();
Serial.println(jieguo1);
delay_(1000);//软件延时1000m 
Serial.println("Now calibrate! Please add 500g weight.");
 delay_(3000);
 pingjunzhi();//空走一遍初始化校准程序
 delay_(1000);
jieguo2=pingjunzhi();
Serial.println(jieguo2); 
jieguo3=500/(jieguo2-jieguo1);
Serial.println("success"); 
Serial.print(jieguo3,10); 
//////////////////////////////存储数据
dvalue.v =jieguo3;
dpointer = dvalue.dchar ;
   for(i = 0; i < 8; i++) {
      EEPROM.write(i,*dpointer);
      dpointer++;
    }
 delay_(1000);
 Serial.println("wuchaxishu_success!!!"); 
  }
double filter()  //限幅滤波算法函数
{  
   double xfvalue; 
   xfvalue = hx.bias_read();
   //Serial.print("aaa");
   Serial.print(xfvalue);
   double  new_value;  
   new_value = hx.bias_read();
   Serial.print(",");
   Serial.print(new_value);
   Serial.print(",");
  if((new_value-xfvalue>A)||(xfvalue-new_value>A))
   return xfvalue; 
   else 
   return new_value;  
}






















//****************************************//
void setup() {
  Serial.begin(9600);
  hx.set_offset(pianyiliang());//设置偏移量
  hx.set_co(jiance()); //修改修正系数
  attachInterrupt(0,zhongduan, CHANGE);//调用外部中断
  
}
//*********************************************//
void loop() {
 /*ds.start();//开始测量温度
 float temprature=ds.get();
 double sum1 ;
 sum1 = hx.bias_read();
  Serial.print(hx.read());
  Serial.print(",");
  Serial.print(filter());
  Serial.print(",");  
  Serial.println( temprature);//(sum1/ 10 * (1 + (20 - temprature) / 10 * 0.02)）;
  Serial.println(sum1/10*(1+(20-temprature)/10*0.02));*/
double x=filter();
  Serial.println(x);
}
