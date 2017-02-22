#pragma once

#include <QThread>
#include <QTimer>
#include <QMap>

#include <Windows.h>
#include "SimConnect.h"

#include "MaoSimConst.h"

class MaoSimEventloop : public QThread
{
	Q_OBJECT

public:
	MaoSimEventloop(HANDLE & hSimConnect, QMap<QString, DWORD> realFlights);
	~MaoSimEventloop();

	void Stop();

private:
	void run();

public slots:
	void dispatchEvent();

private:
	QTimer dispatcherTrigger;

	HANDLE & hSimConnect;
	QMap<QString, DWORD> & realFlights;
};