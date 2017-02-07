#pragma once

#include <Windows.h>
#include <QThread>
#include <QMap>
#include <QTimer>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "MaoFlight.h"
using namespace std;

class MaoRealFlightThread :	public QThread
{
	Q_OBJECT

signals :
	void FSXupdate(MaoFlightUpdate, MaoFlight*);
	void FSXtimeout(MaoFlight*);
	void debug(double);

public:
	MaoRealFlightThread();
	~MaoRealFlightThread();
	void setShutdown();
public slots:
	void SHUTDOWN();

protected:
	void run()
	{
		//timerDump1090.start();
		timerFlightRadar24.start();
		timerCheck.start();
		exec();
	}

private slots:
	void getDump1090Data();
	void loadDump1090Data(QNetworkReply* reply);
	void getFlightRadar24Data();
	void loadFlightRadar24Data(QNetworkReply* reply);

private:
	void checkFlightTimeout();

private:
	LARGE_INTEGER freq, starT, stop;





	bool isExit;

	CRITICAL_SECTION realFlightCS;
	QMap<QString, MaoFlight*> realFlights;

	QTimer timerCheck;
	QTimer timerDump1090;
	QNetworkAccessManager restDump1090;
	QTimer timerFlightRadar24;
	QNetworkAccessManager restFlightRadar24;
	QTimer timerModeSMixer2;
	QNetworkAccessManager restModeSMixer2;
};
