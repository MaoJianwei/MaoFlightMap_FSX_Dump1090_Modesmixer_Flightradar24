
#include <QJsonParseError>
#include <QJsonDocument>
#include "MaoRealFlightThread.h"

MaoRealFlightThread::MaoRealFlightThread():isExit(false)
{
	updateFSXcenter(19.9963, 110.3254);



	qRegisterMetaType<MaoFlight>("MaoFlight");
	qRegisterMetaType<MaoFlightUpdate>("MaoFlightUpdate");

	//connect(&timerDump1090, &QTimer::timeout, this, &MaoRealFlightThread::getDump1090Data);
	//connect(&restDump1090, &QNetworkAccessManager::finished, this, &MaoRealFlightThread::loadDump1090Data);
	connect(&timerFlightRadar24, &QTimer::timeout, this, &MaoRealFlightThread::getFlightRadar24Data);
	connect(&restFlightRadar24, &QNetworkAccessManager::finished, this, &MaoRealFlightThread::loadFlightRadar24Data);
	connect(&timerCheck, &QTimer::timeout, this, &MaoRealFlightThread::checkFlightTimeout);


	timerCheck.setInterval(1000);
	timerCheck.setSingleShot(false);
	timerCheck.moveToThread(this);

	timerDump1090.setInterval(1000);
	timerDump1090.setSingleShot(true);
	timerDump1090.moveToThread(this);

	timerFlightRadar24.setInterval(3000);
	timerFlightRadar24.setSingleShot(true);
	timerFlightRadar24.moveToThread(this);
	//restDump1090.moveToThread(this);

	InitializeCriticalSection(&realFlightCS);
}

MaoRealFlightThread::~MaoRealFlightThread()
{
	//clear map *
	for (MaoFlight* flight : realFlights.values())
	{
		realFlights.remove(flight->getICAO());
		delete flight;
	}

	DeleteCriticalSection(&realFlightCS);
}

void MaoRealFlightThread::setShutdown()
{
	isExit = true;
	timerCheck.stop();
	timerDump1090.stop();
	timerFlightRadar24.stop();
}

void MaoRealFlightThread::updateFSXcenter(double fsxLat, double fsxLon)
{
	latUp = fsxLat + 2 > 90 ? 90 : fsxLat + 2;
	latDown = fsxLat - 2 < -90 ? -90 : fsxLat - 2;
	lonLeft = fsxLon - 2 < -180 ? 360 + fsxLon - 2 : fsxLon - 2;
	lonRight = fsxLon + 2 > 180 ? -360 + fsxLon + 2 : fsxLon + 2;
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


	qint64 start = QDateTime::currentMSecsSinceEpoch();

	EnterCriticalSection(&realFlightCS);

	QVariantList jsonListOuter = json.toVariant().toList();
	for (QVariant flight : jsonListOuter)
	{
		QVariantMap one = flight.toMap();
		if (one["seen"].toInt() > 60)
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

	LeaveCriticalSection(&realFlightCS);

	if(false == isExit)
		timerDump1090.start();
	

	qint64 over = QDateTime::currentMSecsSinceEpoch();
	//emit debug(over - start);
}

void MaoRealFlightThread::getFlightRadar24Data()
{
	// 20.69,19.45,109.72,110.18
	//double latUp = 42.69, latDown = 19.45, lonLeft = 105.72, lonRight = 120.18;
	QString urlFlightRadar24 = QString("http://data-live.flightradar24.com/zones/fcgi/feed.js?bounds=%1,%2,%3,%4&faa=1&mlat=1&flarm=1&adsb=1&gnd=1&air=1&vehicles=0&estimated=1&maxage=7200&gliders=1&stats=0")
		.arg(latUp).arg(latDown).arg(lonLeft).arg(lonRight);

	QNetworkRequest req = QNetworkRequest(urlFlightRadar24);

	restFlightRadar24.get(req);
}

void MaoRealFlightThread::loadFlightRadar24Data(QNetworkReply* reply)
{
	//QueryPerformanceFrequency(&freq);
	//QueryPerformanceCounter(&starT);
	//QueryPerformanceCounter(&stop);
	//double result = double(stop.QuadPart - starT.QuadPart) / double(freq.QuadPart) * 1000000;//得出的单位为 微秒

	QByteArray data = reply->readAll();


	QJsonParseError error;
	QJsonDocument json = QJsonDocument::fromJson(data, &error);
	if (QJsonParseError::NoError != error.error)
		throw 40110;//TODO


	qint64 start = QDateTime::currentMSecsSinceEpoch();

	EnterCriticalSection(&realFlightCS);

	QMap<QString, QVariant> jsonMapOuter = json.toVariant().toMap();
	for (QVariant v : jsonMapOuter.values())
	{
		if (v.type() != QVariant::Type::List)
			continue;


		QVariantList flight = v.toList();

		QString ICAO = flight[0].toString();
		MaoFlight* newOne = realFlights.value(ICAO, 0);
		if (0 == newOne)
		{
			newOne = new MaoFlight(ICAO);
			realFlights.insert(ICAO, newOne);
		}

		newOne->seeAgain();


		QString callsign = flight[16].toString();
		if (newOne->getCallsign() != callsign)
		{
			newOne->updateCallsign(callsign);
			emit FSXupdate(CALLSIGN_UPDATE, newOne);
		}

		double alt = flight[4].toDouble();
		if (newOne->getAlt() != alt)
		{
			newOne->updateAlt(alt);
			emit FSXupdate(ALT_UPDATE, newOne);
		}

		double lat = flight[1].toDouble();
		double lon = flight[2].toDouble();
		double oldLat, oldLon;
		newOne->getPos(oldLat, oldLon);
		if (oldLat != lat || oldLon != lon)
		{
			newOne->updatePos(lat, lon);
			emit FSXupdate(POS_UPDATE, newOne);
		}

		int track = flight[3].toInt();
		if (newOne->getTrack() != track)
		{
			newOne->updateTrack(track);
			emit FSXupdate(TRACK_UPDATE, newOne);
		}


		int speed = flight[5].toInt();
		if (newOne->getSpeed() != speed)
		{
			newOne->updateSpeed(speed);
			emit FSXupdate(SPEED_UPDATE, newOne);
		}

		double vertRate = flight[15].toDouble();
		if (newOne->getVertRate() != vertRate)
		{
			newOne->updateVertRate(vertRate);
			emit FSXupdate(VERT_RATE_UPDATE, newOne);
		}
	}

	LeaveCriticalSection(&realFlightCS);

	if (false == isExit);
		timerFlightRadar24.start();


	qint64 over = QDateTime::currentMSecsSinceEpoch();
	emit debug(over - start);
	//emit debug(result);
}

void MaoRealFlightThread::checkFlightTimeout()
{
	EnterCriticalSection(&realFlightCS);
	for (MaoFlight* f : realFlights.values())
	{
		if (QDateTime::currentSecsSinceEpoch() - f->getLastSeen() > 60)
		{
			realFlights.remove(f->getICAO());
			emit FSXtimeout(f);
		}
	}
	LeaveCriticalSection(&realFlightCS);
}
