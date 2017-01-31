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
		ui.commanderData->setPlainText("Connected to Flight Simulator!");
		ui.connectStatus->setChecked(true);
		ui.connectStatus->setEnabled(true);
	}
	else
	{
		ui.commanderData->setPlainText("Fail to connect to Flight Simulator!");
		ui.connectStatus->setEnabled(false);
	}
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