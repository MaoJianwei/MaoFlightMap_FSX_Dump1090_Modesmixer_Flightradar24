#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtGuiApplication1.h"

#include <windows.h>
#include "SimConnect.h"

class QtGuiApplication1 : public QMainWindow
{
	Q_OBJECT

public:
	QtGuiApplication1(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtGuiApplication1Class ui;

	HANDLE  hSimConnect;

private slots:
	void setupConnect();
	void killConnect();
	void getCommanderData();
	void createOneAI();
	void setAiData();
	void deleteOneAI();
};

static enum Mao_DATA_DEFINE_ID {
	CommanderFlightData,
	AiFlightData
};

static enum Mao_DATA_REQUEST_ID {
	CommanderFlightDataReq,
};