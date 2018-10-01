#ifndef MAOFLIGHTMAP_H
#define MAOFLIGHTMAP_H

#include <QtWidgets/QMainWindow>
#include "ui_maoflightmap.h"

#include "MaoWaypoint.h"
#include "MaoLookupThread.h"
#include "MaoFlyThread.h"
#include "MaoFSthread.h"
#include "MaoUIsignal.h"


#include <QTextCodec>
#include <QThread>
#include <QTimer>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTableWidget>
#include <qwebengineview.h>
#include <QCloseEvent>


#include <windows.h>

#include <vector>
using namespace std;





class MaoFlightMap : public QMainWindow
{
	Q_OBJECT

public:
	MaoFlightMap(QWidget *parent = 0);
	~MaoFlightMap();


signals:
	void SHUTDOWN();

	void sGetSOC(QString username, QString password);
	void sGetFlightMap(int No);
	void startRecord(QString FlightNo, QString ArrICAO);
	void pathReady();


	private slots:
	void markPin(double, double);

	void lookupFlightMap();

	void startEndFlight();
	void cancelFlight();

	void tableShrinkTab(int);// UI 体验
	void tableShrink();// UI 体验
	void FlightListFocusIn(const QSize &);// UI 体验
	void SocListFocusIn(const QSize &);// UI 体验
	void lookupWebShow();
	void showUIs(MaoUIsignalS);
	void showUIb(MaoUIsignalB);

	void clickSOC();
	
	void loadUpdate(QNetworkReply* reply);


	void checkDB();

private:
	void closeEvent(QCloseEvent* e);
	void shutdownThreads();

	void initDriveCheck();

	void initFlightTableView();
	void initSocTableView();
	void initFlightListViewUI();

	void initDatabase();
	void initThread();
	void initWeb();

	void checkUpdate();

	void enableNetworkProxy(); // shadowsocks 代理，给 QWebEngineView 加载 bing 地图用
	void disableNetworkProxy(); // 解除代理，给获取 SOC 数据用


	void loadFlightList();//启动时调用、需要重新载入航班表时调用，
	void loadSocFlightList();//启动时调用、需要重新载入航班表时调用，

	void genTraceHTML();
	void traceWebShow();

	void lowToHigh(const QString & text);
	QString zh(const char*);

	bool updateDB();
	bool backupDB();
	bool updateDBfromV12();

private:
	Ui::flightmapClass ui;
	QTextCodec* china;

	QSqlDatabase db;

	int currentROW;

	//bool canPin;

	MaoFSthread tFS;
	MaoFlyThread tFly;
	MaoLookupThread tLookup;

	QNetworkAccessManager update;

	//QTimer * tUpdateDB;//临时用一次，尝试用临时堆对象配合sender()完成
};

#endif // MAOFLIGHTMAP_H