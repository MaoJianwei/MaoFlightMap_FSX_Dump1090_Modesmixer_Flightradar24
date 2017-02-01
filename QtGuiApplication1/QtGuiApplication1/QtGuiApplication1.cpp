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
	HRESULT hr = SimConnect_RequestDataOnSimObjectType(
		hSimConnect, CommanderFlightDataReq, CommanderFlightData, 0, SIMCONNECT_SIMOBJECT_TYPE_USER);
	
	handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE
}
void QtGuiApplication1::createOneAI()
{
	SIMCONNECT_DATA_INITPOSITION initPos;
	initPos.Altitude = 100.7014;
	initPos.Latitude = 19.9347;
	initPos.Longitude = 110.4428;
	initPos.Pitch = 0; // ¸©Ñö
	initPos.Bank = 45; // ²àÇã
	initPos.Heading = 90.0;
	initPos.OnGround = 0;
	initPos.Airspeed = 20;

	HRESULT hr = SimConnect_AICreateNonATCAircraft(hSimConnect, "Boeing 737-800 Paint1", "M-1080", initPos, CreateAiAircraftReq);
	
	handleSimConnectData(hSimConnect); // SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID
}
void QtGuiApplication1::setAiData()
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
	HRESULT hr = SimConnect_SetDataOnSimObject(hSimConnect, AiFlightData, ui.createObjectId->text().toULong(), 0, 0, sizeof(pos), &pos);

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
	
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Altitude", "feet") : hr ;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Latitude", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "Plane Longitude", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION ALT", "Meters") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LAT", "degrees") : hr;
	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, CommanderFlightData, "GPS POSITION LON", "degrees") : hr;

	hr = S_OK == hr ? SimConnect_AddToDataDefinition(hSimConnect, AiFlightData, "Initial Position", NULL, SIMCONNECT_DATATYPE_INITPOSITION) : hr;
	
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
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
		{
			SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

			switch (pObjData->dwRequestID)
			{
				case CommanderFlightDataReq:
				{
					CommanderFlightDataStruct *pCFD = (CommanderFlightDataStruct*)&pObjData->dwData;
					ui.commanderData->setPlainText(
						QString("\nObjectID=%1  \nplaneAlt=%2  planeLat=%3  planeLon=%4  \ngpsAlt=%5  gpsLat=%6  gpsLon=%7  \ncalculateAlt=%8")
						.arg(pObjData->dwObjectID)
						.arg(pCFD->planeAlt)
						.arg(pCFD->planeLat)
						.arg(pCFD->planeLon)
						.arg(pCFD->gpsAlt)
						.arg(pCFD->gpsLat)
						.arg(pCFD->gpsLon)
						.arg(pCFD->gpsAlt * 3.28084));
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
		default:
			int a = 0;
			break;
	}
}