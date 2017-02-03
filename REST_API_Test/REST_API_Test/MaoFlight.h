#pragma once

#include <QString>

class MaoFlight
{
public:
	MaoFlight(QString callsign = "",
		double altitude = 0,
		double latitude = 0,
		double longitude = 0,
		int track = 0,
		int speed = 0,
		double vertRate = 0)
	{
		this->callsign = callsign;
		this->altitude = altitude;
		this->latitude = latitude;
		this->longitude = longitude;
		this->track = track;
		this->speed = speed;
		this->vertRate = vertRate;
	}

private:
	QString callsign;
	double altitude;
	double latitude;
	double longitude;
	int track;
	int speed;
	double vertRate;
};