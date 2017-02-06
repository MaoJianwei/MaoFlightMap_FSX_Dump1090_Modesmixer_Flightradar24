
#include <QDateTime>
#include  "MaoFlight.h"


MaoFlight::MaoFlight(QString ICAO)
{
	this->ICAO = ICAO;
	this->callsign = "";
	this->altitude = 0;
	this->latitude = 0;
	this->longitude = 0;
	this->track = 0;
	this->speed = 0;
	this->vertRate = 0;

	this->fsxObjectID = Mao_FSX_OBJECT_ID_UNABLE;
}

void MaoFlight::updateCallsign(QString & callsign)
{
	this->callsign = callsign;
}

void MaoFlight::updateAlt(double alt)
{
	this->altitude = alt;
}

void MaoFlight::updatePos(double & lat, double & lon)
{
	this->latitude = lat;
	this->longitude = lon;
}

void MaoFlight::updateTrack(int track)
{
	this->track = track;
}

void MaoFlight::updateSpeed(int speed)
{
	this->speed = speed;
}

void MaoFlight::updateVertRate(double vertRate)
{
	this->vertRate = vertRate;
}

void MaoFlight::seeAgain()
{
	this->lastSeen = QDateTime::currentSecsSinceEpoch();
}


QString MaoFlight::getICAO()
{
	return ICAO;
}

QString MaoFlight::getCallsign()
{
	return callsign;
}

double MaoFlight::getAlt()
{
	return altitude;
}

void MaoFlight::getPos(double & lat, double & lon)
{
	lat = latitude;
	lon = longitude;
}

int MaoFlight::getTrack()
{
	return track;
}

int MaoFlight::getSpeed()
{
	return speed;
}

double MaoFlight::getVertRate()
{
	return vertRate;
}

qint64 MaoFlight::getLastSeen()
{
	return lastSeen;
}

int MaoFlight::getFsxObjectID()
{
	return fsxObjectID;
}
