#pragma once

#include <QString>
#include <QMap>
#include <QThread>

#include <windows.h>
#include "SimConnect.h"

#include "MaoFlight.h"
#include "MaoSimConst.h"
#include "MaoSimEventloop.h"

class MaoSimCore : public QThread
{
	Q_OBJECT

public:
	MaoSimCore();
	~MaoSimCore();

	void Stop();

public slots:
	void FSXupdate(MaoFlightUpdate, MaoFlight*);
	void FSXtimeout(MaoFlight*);

private:
	void run();

private:
	QMap<QString, DWORD> realFlights;
	MaoSimEventloop simEventloop;



	// --- Mao FSX SDK ---
public:
	bool setupConnect();
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
