#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_REST_API_Test.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <vector>
#include "MaoFlight.h"

using namespace std;

class REST_API_Test : public QMainWindow
{
	Q_OBJECT

public:
	REST_API_Test(QWidget *parent = Q_NULLPTR);

	private slots:
	void getModeSMixerData();
	void loadModeSMixerData(QNetworkReply* reply);
	void getDump1090Data();
	void loadDump1090Data(QNetworkReply* reply);
public:

private:
	Ui::REST_API_TestClass ui;

	vector<MaoFlight*> realFlights;

	int epochModeSMixer;
	QNetworkAccessManager restModeSMixer;
	QNetworkAccessManager restDump1090;
};
