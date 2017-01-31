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
private:
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
	//void CALLBACK QtGuiApplication1::handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext);
};

void CALLBACK handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext);


static enum Mao_DATA_DEFINE_ID {
	CommanderFlightData,
	AiFlightData
};

static enum Mao_DATA_REQUEST_ID {
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