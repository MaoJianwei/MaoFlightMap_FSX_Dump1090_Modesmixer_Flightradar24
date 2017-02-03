#include "REST_API_Test.h"

#include <QJsonDocument>
#include <QHttpMultiPart>

#include "MaoFlight.h"

REST_API_Test::REST_API_Test(QWidget *parent)
	: QMainWindow(parent), epochModeSMixer(0)
{
	ui.setupUi(this);

	connect(ui.GetModeSMixerButton, &QPushButton::clicked, this, &REST_API_Test::getModeSMixerData);
	connect(&restModeSMixer, &QNetworkAccessManager::finished, this, &REST_API_Test::loadModeSMixerData);

	connect(ui.GetDump1090Button, &QPushButton::clicked, this, &REST_API_Test::getDump1090Data);
	connect(&restDump1090, &QNetworkAccessManager::finished, this, &REST_API_Test::loadDump1090Data);

}

void REST_API_Test::getModeSMixerData()
{
	QString urlModeSMixer = QString("http://%1:%2/json").arg("192.168.0.3").arg(8080);

	QNetworkRequest req = QNetworkRequest(urlModeSMixer);

	QString postDataModeSMixer = QString("{\"req\":\"getStats\",\"data\":{\"statsType\":\"flights\",\"id\":%1}}").arg(epochModeSMixer);

	restModeSMixer.post(req, postDataModeSMixer.toUtf8());
}

void REST_API_Test::loadModeSMixerData(QNetworkReply* reply)
{
	QByteArray data = reply->readAll();


	QJsonParseError error;
	QJsonDocument json = QJsonDocument::fromJson(data, &error);
	if (QJsonParseError::NoError != error.error)
		throw 40110;//TODO


	QVariantMap jsonMapOuter = json.toVariant().toMap();
	QVariantMap jsonMapStats = jsonMapOuter["stats"].toMap();

	epochModeSMixer = jsonMapStats["epoch"].toInt();
	QList<QVariant> jsonFlights = jsonMapStats["flights"].toList();
	for (QVariant flight : jsonFlights)
	{

	}

	ui.RestResult->setPlainText(data);
	ui.epochModesModeSMixer->setText(QString("%1   %2").arg(epochModeSMixer).arg(data.length()));
}

void REST_API_Test::getDump1090Data()
{
	QString urlDump1090 = QString("http://%1:%2/dump1090/data.json").arg("127.0.0.1").arg(8080);

	QNetworkRequest req = QNetworkRequest(urlDump1090);

	restDump1090.get(req);
}

void REST_API_Test::loadDump1090Data(QNetworkReply* reply)
{
	QByteArray data = reply->readAll();


	QJsonParseError error;
	QJsonDocument json = QJsonDocument::fromJson(data, &error);
	if (QJsonParseError::NoError != error.error)
		throw 40110;//TODO

	QVariantList jsonListOuter = json.toVariant().toList();
	for (QVariant flight : jsonListOuter)
	{
		QVariantMap one = flight.toMap();
		if (one["seen"].toInt() > 60)
			continue;

		if (0 == one["validposition"].toInt())
			continue;

		MaoFlight* f = new MaoFlight(one["flight"].toString().trimmed(),
									one["altitude"].toDouble(),
									one["lat"].toDouble(),
									one["lon"].toDouble(),
									one["track"].toInt(),
									one["speed"].toInt(),
									one["vert_rate"].toDouble());

		realFlights.push_back(f);		
	}
	int a = 0;
}
