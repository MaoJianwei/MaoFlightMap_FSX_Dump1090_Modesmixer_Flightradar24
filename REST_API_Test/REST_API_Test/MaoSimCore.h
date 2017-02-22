#pragma once

#include <QString>
#include <QMap>

#include <windows.h>
#include "SimConnect.h"

#include "MaoSimConst.h"
#include "MaoSimEventloop.h"

class MaoSimCore {
	
	//Q_OBJECT

public:
	MaoSimCore();
	~MaoSimCore();

private:
	QMap<QString, DWORD> realFlights;
	MaoSimEventloop simEventloop;

	// --- FSX SDK ---
public:
	void setupConnect();
	void killConnect();
	void getCommanderData();
	void createOneAI();
	void setAiData();
	void deleteOneAI();

private:
	HANDLE hSimConnect;

	bool initSimConnect();
	//void handleSimConnectData(HANDLE hSimConnect);
private:
	//SIMCONNECT_RECV* _receiveSimConnectData(HANDLE hSimConnect);
	//void _handleSimConnectData(SIMCONNECT_RECV* pData);
};
