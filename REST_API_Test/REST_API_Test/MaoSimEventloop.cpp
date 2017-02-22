
#include "MaoSimEventloop.h"

MaoSimEventloop::MaoSimEventloop(HANDLE & hSimConnect, QMap<QString, DWORD> realFlights)
	: hSimConnect(hSimConnect), realFlights(realFlights)
{
	connect(&dispatcherTrigger, &QTimer::timeout, this, &MaoSimEventloop::dispatchEvent);

	dispatcherTrigger.setInterval(100);
	dispatcherTrigger.setSingleShot(true);
	dispatcherTrigger.moveToThread(this);
}
MaoSimEventloop::~MaoSimEventloop()
{
	dispatcherTrigger.stop();
}
void MaoSimEventloop::run() {
	dispatcherTrigger.start();
	exec();
}

void MaoSimEventloop::dispatchEvent() {


	// TODO - CHECK pasted CODE


	SIMCONNECT_RECV* pData;
	static DWORD cbData;

	while (SUCCEEDED(SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData))) {
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
				//ui.commanderData->setPlainText(
				//	QString("\nObjectID=%1  \nplaneAlt=%2  planeLat=%3  planeLon=%4  \ngpsAlt=%5  gpsLat=%6  gpsLon=%7  \ncalculateAlt=%8")
				//	.arg(pObjData->dwObjectID)
				//	.arg(pCFD->planeAlt)
				//	.arg(pCFD->planeLat)
				//	.arg(pCFD->planeLon)
				//	.arg(pCFD->gpsAlt)
				//	.arg(pCFD->gpsLat)
				//	.arg(pCFD->gpsLon)
				//	.arg(pCFD->gpsAlt * 3.28084));
				break;
			}
			}
			break;
		}
		case SIMCONNECT_RECV_ID_ASSIGNED_OBJECT_ID:
		{
			SIMCONNECT_RECV_ASSIGNED_OBJECT_ID  *pObjData = (SIMCONNECT_RECV_ASSIGNED_OBJECT_ID *)pData;
			//ui.createObjectId->setText(QString("%1").arg(pObjData->dwObjectID));
			break;
		}
		case SIMCONNECT_RECV_ID_EXCEPTION:
			// need to receive data for going here
			//ui.connectStatus->setEnabled(false);
			break;
		default:
			//printf("\nReceived:%d", pData->dwID);
			int a = 0;
			break;
		}
	}


	dispatcherTrigger.start();
}