#pragma once

enum Mao_EVENT_ID {
	Mao_FREEZE_ALTITUDE_TOGGLE
};

enum Mao_DATA_DEFINE_ID {
	CommanderFlightData,
	AiFlightData
};

enum Mao_DATA_REQUEST_ID {
	CommanderFlightDataReq,
	CreateAiAircraftReq = 0x3000000,
	RemoveAiAircraftReq = 0x6000000,
};

struct CommanderFlightDataStruct
{
	double  planeAlt;
	double  planeLat;
	double  planeLon;

	double  gpsAlt;
	double  gpsLat;
	double  gpsLon;
};
