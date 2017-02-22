
#include "MaoSimCore.h"


MaoSimCore::MaoSimCore()
	:hSimConnect(NULL), simEventloop(hSimConnect, realFlights)
{

}

MaoSimCore::~MaoSimCore()
{

}


void MaoSimCore::setupConnect()
{
	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "MaoFlightMap", NULL, 0, NULL, 0)))
	{
		// handleSimConnectData - SIMCONNECT_RECV_ID_OPEN
		if (initSimConnect())
		{
			//ui.commanderData->setPlainText("Connected to Flight Simulator!");
			return;
		}
	}
	//ui.commanderData->setPlainText("Fail to connect to Flight Simulator!");
}

bool MaoSimCore::initSimConnect()
{
	HRESULT hr = S_OK;

	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Altitude", "feet") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Latitude", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Longitude", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION ALT", "Meters") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LAT", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LON", "degrees") : hr;

	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AiFlightData, "Initial Position", NULL, SIMCONNECT_DATATYPE_INITPOSITION) : hr;

	hr = SimConnect_MapClientEventToSimEvent(hSimConnect, Mao_FREEZE_ALTITUDE_TOGGLE, "FREEZE_ALTITUDE_TOGGLE");

	// NO RETURN when success
	return S_OK == hr ? true : false;
}

void MaoSimCore::killConnect()
{
	HRESULT hr = SimConnect_Close(hSimConnect);

	//ui.commanderData->setPlainText("Kill connection to Flight Simulator!");
}
void MaoSimCore::getCommanderData()
{
	HRESULT hr = SimConnect_RequestDataOnSimObjectType(
		hSimConnect, CommanderFlightDataReq, CommanderFlightData, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);

	// handleSimConnectData - SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
}
void MaoSimCore::createOneAI()
{
	SIMCONNECT_DATA_INITPOSITION initPos;
	initPos.Altitude = 100.7014;
	initPos.Latitude = 19.9347;
	initPos.Longitude = 110.4438;
	initPos.Pitch = 0; // ¸©Ñö
	initPos.Bank = 0; // ²àÇã
	initPos.Heading = 90.0;
	initPos.OnGround = 0;
	initPos.Airspeed = 20;

	HRESULT hr = SimConnect_AICreateNonATCAircraft(hSimConnect, "Boeing 737-800 Paint1", "M-1080", initPos, CreateAiAircraftReq);

	handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID

	hr = SimConnect_TransmitClientEvent(hSimConnect, 11, //ui.createObjectId->text().toULong(),
		Mao_FREEZE_ALTITUDE_TOGGLE, 1, SIMCONNECT_GROUP_PRIORITY_DEFAULT, SIMCONNECT_EVENT_FLAG_GROUPID_IS_PRIORITY);
	// NO RETURN when success 
}
void MaoSimCore::setAiData()
{
	SIMCONNECT_DATA_INITPOSITION pos;
	pos.Altitude = 3130.7014;
	pos.Latitude = 19.9357;
	pos.Longitude = 110.4438;
	pos.Pitch = 0; // ¸©Ñö
	pos.Bank = 0; // ²àÇã
	pos.Heading = 90.0;
	pos.OnGround = 0;
	pos.Airspeed = 200;

	HRESULT hr = SimConnect_SetDataOnSimObject(hSimConnect, AiFlightData, 11, //ui.createObjectId->text().toULong(),
		0, 0, sizeof(pos), &pos);
	// NO RETURN when success
}
void MaoSimCore::deleteOneAI()
{
	HRESULT hr = SimConnect_AIRemoveObject(hSimConnect, 11, //ui.createObjectId->text().toULong(),
		RemoveAiAircraftReq);
	// NO RETURN when success
}





//void MaoSimCore::handleSimConnectData(HANDLE hSimConnect)
//{
//	_handleSimConnectData(_receiveSimConnectData(hSimConnect));
//}
//
//SIMCONNECT_RECV* MaoSimCore::_receiveSimConnectData(HANDLE hSimConnect)
//{
//	SIMCONNECT_RECV* pData;
//	static DWORD cbData;
//
//	while (!SUCCEEDED(SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData)));
//
//	return pData;
//}
////void CALLBACK handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
//void MaoSimCore::_handleSimConnectData(SIMCONNECT_RECV* pData)
//{
//	//QtGuiApplication1* app = (QtGuiApplication1*)pContext;
//
//	switch (pData->dwID)
//	{
//		case SIMCONNECT_RECV_ID_OPEN:
//			break;
//		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
//		{
//			SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;
//
//			switch (pObjData->dwRequestID)
//			{
//				case CommanderFlightDataReq:
//				{
//					CommanderFlightDataStruct *pCFD = (CommanderFlightDataStruct*)&pObjData->dwData;
//					//ui.commanderData->setPlainText(
//					//	QString("\nObjectID=%1  \nplaneAlt=%2  planeLat=%3  planeLon=%4  \ngpsAlt=%5  gpsLat=%6  gpsLon=%7  \ncalculateAlt=%8")
//					//	.arg(pObjData->dwObjectID)
//					//	.arg(pCFD->planeAlt)
//					//	.arg(pCFD->planeLat)
//					//	.arg(pCFD->planeLon)
//					//	.arg(pCFD->gpsAlt)
//					//	.arg(pCFD->gpsLat)
//					//	.arg(pCFD->gpsLon)
//					//	.arg(pCFD->gpsAlt * 3.28084));
//					break;
//				}
//			}
//			break;
//		}
//		case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
//		{
//			SIMCONNECT_RECV_ASSIGNED_OBJECT_ID  *pObjData = (SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *)pData;
//			//ui.createObjectId->setText(QString("%1").arg(pObjData->dwObjectID));
//			break;
//		}
//		case SIMCONNECT_RECV_ID_EXCEPTION:
//			// need to receive data for going here
//			//ui.connectStatus->setEnabled(false);
//			break;
//		default:
//			//printf("\nReceived:%d", pData->dwID);
//			int a = 0;
//			break;
//	}
//}