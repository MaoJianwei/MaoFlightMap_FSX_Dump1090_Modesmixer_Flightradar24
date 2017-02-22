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
	Mao_FREEZE_ATTITUDE_SET
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
	AircraftVerticalSpeedData,
};

enum Mao_DATA_REQUEST_ID {
	CreateAiAircraftReq,
	RemoveAiAircraftReq,
	CommanderFlightDataReq,
};

struct CommanderFlightDataStruct
{
	double  planeAlt;
	double  planeLat;
	double  planeLon;

	double  gpsAlt;
	double  gpsLat;
	double  gpsLon;
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