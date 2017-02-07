#include "REST_API_Test.h"

#include <QJsonDocument>
#include <QHttpMultiPart>

#include "MaoFlight.h"

REST_API_Test::REST_API_Test(QWidget *parent)
	: QMainWindow(parent)//, epochModeSMixer(0)
{
	ui.setupUi(this);


	connect(this, &REST_API_Test::SHUTDOWN, &realFlightThread, &MaoRealFlightThread::SHUTDOWN);
	//connect(ui.GetModeSMixerButton, &QPushButton::clicked, this, &REST_API_Test::getModeSMixerData);
	//connect(&restModeSMixer, &QNetworkAccessManager::finished, this, &REST_API_Test::loadModeSMixerData);
	connect(ui.GetDump1090Button, &QPushButton::clicked, this, &REST_API_Test::getDump1090Data);
	//connect(&restDump1090, &QNetworkAccessManager::finished, this, &REST_API_Test::loadDump1090Data);
	connect(&realFlightThread, &MaoRealFlightThread::debug, this, &REST_API_Test::debug);
}

REST_API_Test::~REST_API_Test()
{
	shutdownFSX();
	ui.epochModesModeSMixer->setText("正在关闭...");

	while (true == realFlightThread.isRunning())
		QApplication::processEvents();

	ui.epochModesModeSMixer->setText("关闭完成！！！");
}

void REST_API_Test::getDump1090Data()
{
	realFlightThread.start();
	realFlightThread.moveToThread(&realFlightThread);
}

void REST_API_Test::shutdownFSX()
{
	realFlightThread.setShutdown();
	emit SHUTDOWN();
}

void REST_API_Test::debug(double text)
{
	ui.RestResult->setPlainText(ui.RestResult->toPlainText() + QString("%1,\n").arg(text));

	//ui.RestResult->setPlainText(text);
}

//void REST_API_Test::getModeSMixerData()
//{
//	QString urlModeSMixer = QString("http://%1:%2/json").arg("192.168.0.3").arg(8080);
//
//	QNetworkRequest req = QNetworkRequest(urlModeSMixer);
//
//	QString postDataModeSMixer = QString("{\"req\":\"getStats\",\"data\":{\"statsType\":\"flights\",\"id\":%1}}").arg(epochModeSMixer);
//
//	restModeSMixer.post(req, postDataModeSMixer.toUtf8());
//}
//
//void REST_API_Test::loadModeSMixerData(QNetworkReply* reply)
//{
//	QByteArray data = reply->readAll();
//
//
//	QJsonParseError error;
//	QJsonDocument json = QJsonDocument::fromJson(data, &error);
//	if (QJsonParseError::NoError != error.error)
//		throw 40110;//TODO
//
//
//	QVariantMap jsonMapOuter = json.toVariant().toMap();
//	QVariantMap jsonMapStats = jsonMapOuter["stats"].toMap();
//
//	epochModeSMixer = jsonMapStats["epoch"].toInt();
//	QList<QVariant> jsonFlights = jsonMapStats["flights"].toList();
//	for (QVariant flight : jsonFlights)
//	{
//
//	}
//
//	ui.RestResult->setPlainText(data);
//	ui.epochModesModeSMixer->setText(QString("%1   %2").arg(epochModeSMixer).arg(data.length()));
//}
