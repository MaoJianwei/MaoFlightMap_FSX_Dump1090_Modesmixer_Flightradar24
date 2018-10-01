

#pragma once

#include <QString>

#define Mao_Flight_UI_ConnectStatus 1
#define Mao_Flight_UI_Record_Start 2
#define Mao_Flight_UI_Record_Complete 3
#define Mao_Flight_UI_Soc_Complete 4
#define Mao_Flight_UI_Soc_Process 5
#define Mao_Flight_UI_Status_Message 6
#define Mao_Flight_UI_Soc_Button_Message 7
#define Mao_Flight_UI_Soc_Complete_Can_Retry 8



#define Mao_Flight_UI_TaskNo 111
#define Mao_Flight_UI_DepDate 222
#define Mao_Flight_UI_DepTime 333
#define Mao_Flight_UI_ArrDate 444
#define Mao_Flight_UI_ArrTime 555
#define Mao_Flight_UI_PlaneModel 666
#define Mao_Flight_UI_DepICAO 777	//也是最近机场
#define Mao_Flight_UI_DivArrICAO 888
#define Mao_Flight_UI_Web_Trace 999
#define Mao_Flight_UI_AirSpeed 1122
#define Mao_Flight_UI_GNDspeed 2233

struct MaoUIsignalS
{
	int type;
	QString value;
};

struct MaoUIsignalB
{
	int type;
	bool value;
};