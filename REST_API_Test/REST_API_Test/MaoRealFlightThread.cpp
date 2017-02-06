
#include <QJsonParseError>
#include <QJsonDocument>
#include "MaoRealFlightThread.h"

MaoRealFlightThread::MaoRealFlightThread():isExit(false)
{
	qRegisterMetaType<MaoFlight>("MaoFlight");
	qRegisterMetaType<MaoFlightUpdate>("MaoFlightUpdate");

	connect(&pullFlightTimer, &QTimer::timeout, this, &MaoRealFlightThread::getDump1090Data);
	connect(&restDump1090, &QNetworkAccessManager::finished, this, &MaoRealFlightThread::loadDump1090Data);
	
	pullFlightTimer.setInterval(1000);
	pullFlightTimer.setSingleShot(true);
	pullFlightTimer.moveToThread(this);
	//restDump1090.moveToThread(this);
}

MaoRealFlightThread::~MaoRealFlightThread()
{
	//clear map *
	for (MaoFlight* flight : realFlights.values())
	{
		realFlights.remove(flight->getICAO());
		delete flight;
	}
}

void MaoRealFlightThread::setShutdown()
{
	isExit = true;
	pullFlightTimer.stop();
}

void MaoRealFlightThread::SHUTDOWN()
{
	exit(0);
}

void MaoRealFlightThread::getDump1090Data()
{
	QString urlDump1090 = QString("http://%1:%2/dump1090/data.json").arg("127.0.0.1").arg(8080);

	QNetworkRequest req = QNetworkRequest(urlDump1090);

	restDump1090.get(req);
}

void MaoRealFlightThread::loadDump1090Data(QNetworkReply* reply)
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
		if (one["seen"].toInt() > 60) //|| 0 == one["validposition"].toInt())
			continue;


		QString ICAO = one["hex"].toString();
		MaoFlight* newOne = realFlights.value(ICAO, 0);
		if (0 == newOne)
		{
			newOne = new MaoFlight(ICAO);
			realFlights.insert(ICAO, newOne);
		}

		newOne->seeAgain();

		
		QString callsign = one["flight"].toString().trimmed();
		if (newOne->getCallsign() != callsign)
		{
			newOne->updateCallsign(callsign);
			emit FSXupdate(CALLSIGN_UPDATE, newOne);
		}

		double alt = one["altitude"].toDouble();
		if (newOne->getAlt() != alt)
		{
			newOne->updateAlt(alt);
			emit FSXupdate(ALT_UPDATE, newOne);
		}

		double lat = one["lat"].toDouble();
		double lon = one["lon"].toDouble();
		double oldLat, oldLon;
		newOne->getPos(oldLat, oldLon);
		if (oldLat != lat || oldLon != lon)
		{
			newOne->updatePos(lat, lon);
			emit FSXupdate(POS_UPDATE, newOne);
		}

		int track = one["track"].toInt();
		if (newOne->getTrack() != track)
		{
			newOne->updateTrack(track);
			emit FSXupdate(TRACK_UPDATE, newOne);
		}


		int speed = one["speed"].toInt();
		if (newOne->getSpeed() != speed)
		{
			newOne->updateSpeed(speed);
			emit FSXupdate(SPEED_UPDATE, newOne);
		}

		double vertRate = one["vert_rate"].toDouble();
		if (newOne->getVertRate() != vertRate)
		{
			newOne->updateVertRate(vertRate);
			emit FSXupdate(VERT_RATE_UPDATE, newOne);
		}
	}


	checkFlightTimeout();


	if(false == isExit)
		pullFlightTimer.start();
}

void MaoRealFlightThread::checkFlightTimeout()
{
	for (MaoFlight* f : realFlights.values())
	{
		if (QDateTime::currentSecsSinceEpoch() - f->getLastSeen() > 60)
		{
			realFlights.remove(f->getICAO());
			emit FSXtimeout(f);
		}
	}
}
