#pragma once

#include <QString>

#define Mao_FSX_OBJECT_ID_UNABLE -1
#define Mao_ICAO_UNABLE "940110"


class MaoFlight
{
public:
	MaoFlight(QString ICAO = Mao_ICAO_UNABLE);

public:
	void updateCallsign(QString & callsign);
	void updateAlt(double alt);
	void updatePos(double & lat, double & lon);
	void updateTrack(int track);
	void updateSpeed(int speed);
	void updateVertRate(double vertRate);
	void seeAgain();

	QString getICAO();
	QString getCallsign();
	double getAlt();
	void getPos(double & lat, double & lon);
	int getTrack();
	int getSpeed();
	double getVertRate();
	qint64 getLastSeen();
	int getFsxObjectID();

private:
	QString ICAO;
	QString callsign;
	double altitude;
	double latitude;
	double longitude;
	int track;
	int speed;
	double vertRate;

	qint64 lastSeen;
	int fsxObjectID;
};

enum MaoFlightUpdate
{
	CALLSIGN_UPDATE,
	ALT_UPDATE,
	POS_UPDATE,
	TRACK_UPDATE,
	SPEED_UPDATE,
	VERT_RATE_UPDATE
};
