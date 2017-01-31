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
	if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Request Data", NULL, 0, 0, 0)))
	{
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

	hr = S_OK == hr ? SimConnect_CallDispatch(hSimConnect, handleSimConnectData, this) : hr;
	int a = 0;
}
void QtGuiApplication1::createOneAI()
{

}
void QtGuiApplication1::setAiData()
{

}
void QtGuiApplication1::deleteOneAI()
{

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

	//hr = S_OK == hr ? SimConnect_CallDispatch(hSimConnect, handleSimConnectData, this) : hr;

	return S_OK == hr ? true : false;
}
void CALLBACK handleSimConnectData(SIMCONNECT_RECV* pData, DWORD cbData, void *pContext)
{


	// todo: 拿到数据晚了一次




	switch (pData->dwID)
	{
		case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE:
		{
			SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE *pObjData = (SIMCONNECT_RECV_SIMOBJECT_DATA_BYTYPE*)pData;

			switch (pObjData->dwRequestID)
			{
				case CommanderFlightDataReq:
				{
					CommanderFlightDataStruct *pCFD = (CommanderFlightDataStruct*)&pObjData->dwData;
					QtGuiApplication1* app = (QtGuiApplication1*)pContext;
					app->ui.commanderData->setPlainText(
						QString("\nObjectID=%1  \nplaneAlt=%2  planeLat=%3  planeLon=%4  \ngpsAlt=%5  gpsLat=%6  gpsLon=%7")
						.arg(pObjData->dwObjectID)
						.arg(pCFD->planeAlt)
						.arg(pCFD->planeLat)
						.arg(pCFD->planeLon)
						.arg(pCFD->gpsAlt)
						.arg(pCFD->gpsLat)
						.arg(pCFD->gpsLon));
					break;
				}
			}
			break;
		}
	}
}