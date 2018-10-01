
#pragma once

#define Mao_Flight_TableWidget_Width_Shrink 324
//#define Mao_Flight_TableWidget_Width_ExpendShort 566
#define Mao_Flight_TableWidget_Width_Expend 596
//#define Mao_Flight_TableWidget_Width_ExpendShort_SOC 634
#define Mao_Flight_TableWidget_Width_Expend_SOC 635

#define Mao_Flight_DB_SOC_Column 12-1
#define Mao_Flight_DB_FlightList_Column 11-2
//#define Mao_Flight_DB_Path "d:/MaoFlightRecord.db"
#define Mao_Flight_ICAO_Path ":/MaoFlightMap/MaoICAO.maoSerial"
//#define Mao_Flight_HTML_File_Path "d:/MaoFlightRecord.html"
//#define Mao_Flight_HTML_Lookup_Web_Path "file:///d:/MaoFlightRecord.html"
//#define Mao_Flight_HTML_Trace_Web_Path "file:///d:/MaoTraceWeb.html"

#define Mao_Flight_DB_Update_Reject 12580
#define Mao_Flight_DB_Backup_Fail 12555


static const char* FlightHead[Mao_Flight_DB_FlightList_Column] = {
	"起飞日期",
	"起飞时间",
	"降落日期",
	"降落时间",
	"航班号",
	"机型",
	"起飞机场",
	"降落机场",
	"备降机场"
};

static const char* SocHead[Mao_Flight_DB_SOC_Column] = {
	"日期",
	"航班号",
	"机型",
	"飞机",
	"起飞机场",
	"降落机场",
	"机长",
	"载客",
	"分数",
	"声誉",
	"收入"
};


