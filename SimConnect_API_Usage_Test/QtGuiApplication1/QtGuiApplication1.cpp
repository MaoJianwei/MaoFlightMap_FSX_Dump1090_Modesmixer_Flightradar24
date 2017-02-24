#include "QtGuiApplication1.h"

QtGuiApplication1::QtGuiApplication1(QWidget *parent)
	: QMainWindow(parent), hSimConnect(NULL)
{
	ui.setupUi(this);

	connect(ui.setupConnect, &QPushButton::clicked, this, &QtGuiApplication1::setupConnect);
	connect(ui.killConnect, &QPushButton::clicked, this, &QtGuiApplication1::killConnect);
	connect(ui.getCommanderData, &QPushButton::clicked, this, &QtGuiApplication1::getCommanderData);
	connect(ui.createOneAI, &QPushButton::clicked, this, &QtGuiApplication1::createOneAI);
	connect(ui.setOneAI, &QPushButton::clicked, this, &QtGuiApplication1::setAiData);
	connect(ui.deleteOneAI, &QPushButton::clicked, this, &QtGuiApplication1::deleteOneAI);
}

void QtGuiApplication1::setupConnect()
{
	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "MaoFlightMap", NULL, 0, NULL, 0)))
	{
		handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_OPEN
		if (initSimConnect())
		{
			ui.commanderData->setPlainText("Connected to Flight Simulator!");
			ui.connectStatus->setChecked(true);
			ui.connectStatus->setEnabled(true);
			return;
		}
	}
	ui.commanderData->setPlainText("Fail to connect to Flight Simulator!");
	ui.connectStatus->setEnabled(false);
}
void QtGuiApplication1::killConnect()
{
	HRESULT hr = SimConnect_Close(hSimConnect);

	ui.commanderData->setPlainText("Kill connection to Flight Simulator!");
	ui.connectStatus->setChecked(false);
}
void QtGuiApplication1::getCommanderData()
{
	HRESULT hr = SimConnect_RequestDataOnSimObject(hSimConnect, CommanderFlightDataReq, CommanderFlightData, ui.createObjectId->text().toULong(), SIMCONNECT_PERIOD_ONCE, 0, 0, 0, 0);
	//HRESULT hr = SimConnect_RequestDataOnSimObjectType(
	//	hSimConnect, CommanderFlightDataReq, CommanderFlightData, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);
	
	handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
}
void QtGuiApplication1::createOneAI()
{
	SIMCONNECT_DATA_INITPOSITION initPos;
	initPos.Altitude = 6000.7014;
	initPos.Latitude = 19.9347;
	initPos.Longitude = 110.4438;
	initPos.Pitch = 0; // ¸©Ñö
	initPos.Bank = 0; // ²àÇã
	initPos.Heading = 90.0;
	initPos.OnGround = 0;
	initPos.Airspeed = 30;

	HRESULT hr = SimConnect_AICreateNonATCAircraft(hSimConnect, "Boeing 737-800 Paint1", "M-1080", initPos, CreateAiAircraftReq);

	handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID

	//ui.createObjectId->text().toULong()
	//hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_MASTER, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AUTOPILOT_ON, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_ATT_HOLD_ON, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_HDG_HOLD_ON, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AUTO_THROTTLE_ARM, 1, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_PANEL_SPEED_HOLD_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_PANEL_ALTITUDE_SET, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

	//hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_FREEZE_ALTITUDE_SET, 1, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	//hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_FREEZE_ATTITUDE_SET, 1, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	// NO RETURN when success
}
void QtGuiApplication1::setAiData()
{
	//test pitch

	int data = ui.setAlt->text().toInt();
	//ui.setAlt->setText(QString("%1").arg(data + 0.1));
	double data2 = ui.setLat->text().toLong();
	//ui.setLat->setText(QString("%1").arg(data2 - 0.1));
	HRESULT hr = SimConnect_SetDataOnSimObject(hSimConnect, AircraftAirSpeedData, ui.createObjectId->text().toInt(), 0, 0, sizeof(data), &data);
	//HRESULT hr = SimConnect_SetDataOnSimObject(hSimConnect, AircraftAltitudeData, SIMCONNECT_OBJECT_ID_USER, 0, 0, sizeof(data), &data);
	
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_ALT_VAR_SET_ENGLISH, data2, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_VS_VAR_SET_ENGLISH, 6000, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_PANEL_SPEED_HOLD_TOGGLE, 0, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_HEADING_BUG_SET, 220, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	//hr = SimConnect_TransmitClientEvent(hSimConnect, ui.createObjectId->text().toULong(), Mao_AP_ALT_HOLD_ON, 1, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);

	return;

	SIMCONNECT_DATA_INITPOSITION pos;
	pos.Altitude = 3130.7014;
	pos.Latitude = 19.9357;
	pos.Longitude = 110.4438;
	pos.Pitch = 0; // ¸©Ñö
	pos.Bank = 0; // ²àÇã
	pos.Heading = 90.0;
	pos.OnGround = 0;
	pos.Airspeed = 200;
	
	hr = SimConnect_SetDataOnSimObject(hSimConnect, AiFlightData, ui.createObjectId->text().toULong(), 0, 0, sizeof(pos), &pos);
	// NO RETURN when success
}
void QtGuiApplication1::deleteOneAI()
{
	HRESULT hr = SimConnect_AIRemoveObject(hSimConnect, ui.createObjectId->text().toULong() , RemoveAiAircraftReq);
	// NO RETURN when success
}
bool QtGuiApplication1::initSimConnect()
{
	HRESULT hr = S_OK;
	
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Altitude", "feet") : hr ;
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Latitude", "degrees") : hr;
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Longitude", "degrees") : hr;
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION ALT", "Meters") : hr;
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LAT", "degrees") : hr;
	//hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LON", "degrees") : hr;


	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT AVAILABLE", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT MASTER", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT WING LEVELER", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT HEADING LOCK", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT HEADING LOCK DIR", "Degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT ALTITUDE LOCK", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT ALTITUDE LOCK VAR", "Feet") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT ATTITUDE HOLD", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT PITCH HOLD REF", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT VERTICAL HOLD", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT VERTICAL HOLD VAR", "Feet/minute") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT AIRSPEED HOLD", "Bool") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT AIRSPEED HOLD VAR", "Knots") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "AUTOPILOT THROTTLE ARM", "Bool") : hr;


	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AiFlightData, "Initial Position", NULL, SIMCONNECT_DATATYPE_INITPOSITION) : hr;


	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftAltitudeData, "PLANE ALTITUDE", "feet") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftLatitudeData, "PLANE LATITUDE", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftLongitudeData, "PLANE LONGITUDE", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftPitchData, "PLANE PITCH DEGREES", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftBankData, "PLANE BANK DEGREES", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftHeadingData, "PLANE HEADING DEGREES MAGNETIC", "degrees", SIMCONNECT_DATATYPE_INT32) : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AircraftAirSpeedData, "AIRSPEED INDICATED", "Knots", SIMCONNECT_DATATYPE_INT32) : hr;
	//PLANE PITCH DEGREES
	//PLANE BANK DEGREES

	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_FREEZE_ALTITUDE_SET, "FREEZE_ALTITUDE_SET");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_FREEZE_ATTITUDE_SET, "FREEZE_ATTITUDE_SET");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_MASTER, "AP_MASTER");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AUTOPILOT_ON, "AUTOPILOT_ON");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_ATT_HOLD_ON, "AP_ATT_HOLD_ON");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_HDG_HOLD_ON, "AP_HDG_HOLD_ON");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_HEADING_BUG_SET, "HEADING_BUG_SET");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_PANEL_ALTITUDE_SET, "AP_PANEL_ALTITUDE_SET");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_PANEL_ALTITUDE_HOLD, "AP_PANEL_ALTITUDE_HOLD");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_ALT_VAR_SET_ENGLISH, "AP_ALT_VAR_SET_ENGLISH");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_PANEL_SPEED_HOLD_TOGGLE, "AP_PANEL_SPEED_HOLD_TOGGLE");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AUTO_THROTTLE_ARM, "AUTO_THROTTLE_ARM");
	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_AP_VS_VAR_SET_ENGLISH, "AP_VS_VAR_SET_ENGLISH");


	// NO RETURN when success
	return S_OK == hr ? true : false;
}




void QtGuiApplication1::handleSimConnectData(HANDLE hSimConnect)
{
	_handleSimConnectData(_receiveSimConnectData(hSimConnect));
}

SIMCONNECT_RECV* QtGuiApplication1::_receiveSimConnectData(HANDLE hSimConnect)
{
	SIMCONNECT_RECV* pData;
	static DWORD cbData;

	while (!SUCCEEDED(SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData)));

	return pData;
}
//void CALLBACK handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
void QtGuiApplication1::_handleSimConnectData(SIMCONNECT_RECV* pData)
{
	//QtGuiApplication1* app = (QtGuiApplication1*)pContext;
	
	switch (pData->dwID)
	{
		case SIMCONNECT_RECV_ID_OPEN:
			break;
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA:
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
		{
			//SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;
			SIMCONNECT_RECV_SIMOBJECT_DATA *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA*)pData;

			switch (pObjData->dwRequestID)
			{
				case CommanderFlightDataReq:
				{
					CommanderFlightDataStruct *pCFD = (CommanderFlightDataStruct*)&pObjData->dwData;
					ui.commanderData->setPlainText(
						QString("AVAILABLE=%1  \nMASTER=%2  \nWING LEVELER=%3  \nHEADING LOCK=%4  \nHEADING LOCK DIR=%5  \nALTITUDE LOCK=%6  \nALTITUDE LOCK VAR=%7  \nATTITUDE HOLD=%8  \nPITCH HOLD REF=%9  \nVERTICAL HOLD=%10  \nVERTICAL HOLD VAR=%11  \nAIRSPEED HOLD=%12  \nAIRSPEED HOLD VAR=%13  \nTHROTTLE ARM=%14")
						//.arg(pObjData->dwObjectID)
						//.arg(pCFD->planeAlt)
						//.arg(pCFD->planeLat)
						//.arg(pCFD->planeLon)
						//.arg(pCFD->gpsAlt)
						//.arg(pCFD->gpsLat)
						//.arg(pCFD->gpsLon)
						//.arg(pCFD->gpsAlt * 3.28084));
						.arg(pCFD->a1)
						.arg(pCFD->a2)
						.arg(pCFD->a3)
						.arg(pCFD->a4)
						.arg(pCFD->a5)
						.arg(pCFD->a6)
						.arg(pCFD->a7)
						.arg(pCFD->a8)
						.arg(pCFD->a9)
						.arg(pCFD->a10)
						.arg(pCFD->a11)
						.arg(pCFD->a12)
						.arg(pCFD->a13)
						.arg(pCFD->a14));
					break;
				}
			}
			break;
		}
		case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
		{
			SIMCONNECT_RECV_ASSIGNED_OBJECT_ID  *pObjData = (SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *)pData;
			ui.createObjectId->setText(QString("%1").arg(pObjData->dwObjectID));
			break;
		}
		case SIMCONNECT_RECV_ID_EXCEPTION:
		{	
			// need to receive data for going here
			SIMCONNECT_RECV_EXCEPTION *pObjData = (SIMCONNECT_RECV_EXCEPTION*)pData;

			ui.connectStatus->setEnabled(false);
			break;
		}
		default:
			int a = 0;
			break;
	}
}