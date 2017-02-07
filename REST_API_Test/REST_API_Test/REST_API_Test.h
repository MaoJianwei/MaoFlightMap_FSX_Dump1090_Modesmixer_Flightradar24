#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_REST_API_Test.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include "MaoRealFlightThread.h"


class REST_API_Test : public QMainWindow
{
	Q_OBJECT

signals:
	void SHUTDOWN();

public:
	REST_API_Test(QWidget *parent = Q_NULLPTR);
	~REST_API_Test();

private slots:
	//void getModeSMixerData();
	//void loadModeSMixerData(QNetworkReply* reply);
	void getDump1090Data();
	void shutdownFSX();

	void debug(double);

private:
	Ui::REST_API_TestClass ui;

	MaoRealFlightThread realFlightThread;

	//int epochModeSMixer;
	//QNetworkAccessManager restModeSMixer;
};
