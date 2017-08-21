#ifndef _DATASCOPE_
#define _DATASCOPE_
extern unsigned char DataScope_OutPut_Buffer[400];
void DataScope_Get_Channel_Data(float Data,unsigned char Channel);
unsigned char DataScope_Data_Generate(unsigned char Channel_Number);
void SendFloat(unsigned char Addr,float FloatData);//have tested
void Send3ECG(unsigned char Addr,short int ecg1,short int ecg2,short int ecg3);//have tested

#endif

