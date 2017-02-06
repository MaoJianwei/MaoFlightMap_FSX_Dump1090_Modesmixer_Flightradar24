#pragma once

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

public:
	MaoRealFlightThread();
	~MaoRealFlightThread();
	void setShutdown();
public slots:
	void SHUTDOWN();

protected:
	void run()
	{
		pullFlightTimer.start();
		exec();
	}

private slots:
	void getDump1090Data();
	void loadDump1090Data(QNetworkReply* reply);

private:
	void checkFlightTimeout();

private:
	bool isExit;

	QMap<QString, MaoFlight*> realFlights;

	QTimer pullFlightTimer;
	QNetworkAccessManager restDump1090;
};
