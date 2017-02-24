#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"

#include <windows.h>
#include "SimConnect.h"

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);


	Ui::QtGuiApplication1Class ui;
	HANDLE  hSimConnect;

private slots:
	void setupConnect();
	void killConnect();
	void getCommanderData();
	void createOneAI();
	void setAiData();
	void deleteOneAI();

private:
	bool initSimConnect();
	void handleSimConnectData(HANDLE hSimConnect);
private:
	SIMCONNECT_RECV* _receiveSimConnectData(HANDLE hSimConnect);
	void _handleSimConnectData(SIMCONNECT_RECV* pData);
	//void CALLBACK QtGuiApplication1::handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext);
};


enum Mao_EVENT_ID {
	Mao_FREEZE_ALTITUDE_SET,
	Mao_FREEZE_ATTITUDE_SET,

	Mao_AP_MASTER,
	Mao_AUTOPILOT_ON,
	Mao_AP_ATT_HOLD_ON, // pitch and so on...
	Mao_AP_HDG_HOLD_ON,
	Mao_AP_PANEL_HEADING_HOLD,
	Mao_HEADING_BUG_SET,
	Mao_AP_PANEL_ALTITUDE_SET,
	Mao_AP_PANEL_ALTITUDE_HOLD,
	Mao_AP_ALT_VAR_SET_ENGLISH,
	Mao_AP_PANEL_SPEED_HOLD_TOGGLE,
	Mao_AUTO_THROTTLE_ARM,
	Mao_AP_VS_VAR_SET_ENGLISH,
};

enum Mao_DATA_DEFINE_ID {
	CommanderFlightData,
	AiFlightData,

	AircraftAltitudeData,
	AircraftLatitudeData,
	AircraftLongitudeData,
	AircraftHeadingData,
	AircraftOnGroundData,
	AircraftAirSpeedData,
	AircraftPitchData,
	AircraftBankData
};

enum Mao_DATA_REQUEST_ID {
	CreateAiAircraftReq,
	RemoveAiAircraftReq,
	CommanderFlightDataReq,
};

struct CommanderFlightDataStruct
{
	//double  planeAlt;
	//double  planeLat;
	//double  planeLon;

	//double  gpsAlt;
	//double  gpsLat;
	//double  gpsLon;

	double  a1;
	double  a2;
	double  a3;
	double  a4;
	double  a5;
	double  a6;
	double  a7;
	double  a8;
	double  a9;
	double  a10;
	double  a11;
	double  a12;
	double  a13;
	double  a14;
};

//struct AircraftAltitudeStruct {
//	double Altitude;   // feet   
//};
//struct AircraftLocationStruct {
//	double Latitude;   // degrees
//	double Longitude;  // degrees
//};
//struct AircraftHeadingStruct {
//	double Heading;    // degrees
//};
//struct AircraftOnGroundStruct {
//	DWORD OnGround;   // 1=force to be on the ground
//};
//struct AircraftAirSpeedStruct {
//	DWORD Airspeed;   // knots
//};