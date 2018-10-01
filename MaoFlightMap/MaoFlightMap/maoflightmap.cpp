#include "maoflightmap.h"

#include <QMessageBox>
#include <QNetworkProxy>

extern const char* Head[15];

QString Mao_Flight_DB_Path;
QString Mao_Flight_HTML_File_Path;
QString Mao_Flight_HTML_Lookup_Web_Path;
QString Mao_Flight_HTML_Trace_Web_Path;

#define Mao_Flight_Version "1.5"

MaoFlightMap::MaoFlightMap(QWidget *parent)
: QMainWindow(parent)
{

	qRegisterMetaType<MaoUIsignalB>("MaoUIsignalB");
	qRegisterMetaType<MaoUIsignalS>("MaoUIsignalS");

	//canPin = false;
	currentROW = -1;
	china = QTextCodec::codecForLocale();

	ui.setupUi(this);
	ui.MaoTabFrame->removeTab(3);

	connect(this, &MaoFlightMap::SHUTDOWN, &tFS, &MaoFSthread::SHUTDOWN);
	connect(this, &MaoFlightMap::SHUTDOWN, &tFly, &MaoFlyThread::SHUTDOWN);
	connect(this, &MaoFlightMap::SHUTDOWN, &tLookup, &MaoLookupThread::SHUTDOWN);
	
	connect(this, &MaoFlightMap::pathReady, &tFly, &MaoFlyThread::pathReady);
	connect(this, &MaoFlightMap::pathReady, &tLookup, &MaoLookupThread::pathReady);

	connect(ui.MaoSOCflightList, &QTableWidget::iconSizeChanged, this, &MaoFlightMap::SocListFocusIn);
	connect(ui.MaoFlightList, &QTableWidget::iconSizeChanged, this, &MaoFlightMap::FlightListFocusIn);
	connect(ui.MaoSOCglobalMap, &QPushButton::clicked, this, &MaoFlightMap::tableShrink);
	connect(ui.MaoGlobalMap, &QPushButton::clicked, this, &MaoFlightMap::tableShrink);
	connect(ui.MaoLocalMap, &QPushButton::clicked, this, &MaoFlightMap::tableShrink);
	connect(ui.MaoFlightMap, &QPushButton::clicked, this, &MaoFlightMap::tableShrink);
	connect(ui.MaoTabFrame, &QTabWidget::currentChanged, this, &MaoFlightMap::tableShrinkTab);

	connect(ui.MaoFlightNo, &QLineEdit::textChanged, this, &MaoFlightMap::lowToHigh);
	connect(ui.MaoArrICAO, &QLineEdit::textChanged, this, &MaoFlightMap::lowToHigh);
	connect(ui.MaoSOCusername, &QLineEdit::textChanged, this, &MaoFlightMap::lowToHigh);

	connect(ui.MaoSOCglobalMap, &QPushButton::clicked, &tLookup, &MaoLookupThread::getSOCglobalMap);
	connect(ui.MaoGlobalMap, &QPushButton::clicked, &tLookup, &MaoLookupThread::getGlobalMap);
	connect(ui.MaoLocalMap, &QPushButton::clicked, &tLookup, &MaoLookupThread::getLocalMap);
	connect(ui.MaoFlightMap, &QPushButton::clicked, this, &MaoFlightMap::lookupFlightMap);
	connect(this, &MaoFlightMap::sGetFlightMap, &tLookup, &MaoLookupThread::getFlightMap);

	connect(ui.MaoSOCupdate, &QPushButton::clicked, this, &MaoFlightMap::clickSOC);
	connect(this, &MaoFlightMap::sGetSOC, &tLookup, &MaoLookupThread::getSOC);


	connect(ui.MaoStartEndFlight, &QPushButton::clicked, this, &MaoFlightMap::startEndFlight);
	connect(ui.MaoCancelFlight, &QPushButton::clicked, this, &MaoFlightMap::cancelFlight);
	connect(&tLookup, &MaoLookupThread::webShow, this, &MaoFlightMap::lookupWebShow);
	connect(&tLookup, &MaoLookupThread::reportB, this, &MaoFlightMap::showUIb);
	connect(&tLookup, &MaoLookupThread::reportS, this, &MaoFlightMap::showUIs);

	connect(&tFS, &MaoFSthread::connectResult, this, &MaoFlightMap::showUIb);
	connect(&tFS, &MaoFSthread::pushFSdata, this, &MaoFlightMap::showUIs);
	connect(&tFly, &MaoFlyThread::recordReport, this, &MaoFlightMap::showUIb);
	connect(&tFly, &MaoFlyThread::pushFSdata, this, &MaoFlightMap::showUIs);
	connect(&tFly, &MaoFlyThread::locTrace, this, &MaoFlightMap::markPin);
	connect(this, &MaoFlightMap::startRecord, &tFly, &MaoFlyThread::recordFlight);

	//TODO - connect(ui.MaoLookupView, &QWebEngineView::iconChanged, this, &MaoFlightMap::tableShrink);

	connect(&update, &QNetworkAccessManager::finished, this, &MaoFlightMap::loadUpdate);



	initDriveCheck();

	initFlightListViewUI();

	initDatabase();

	initThread();

	initWeb();

	enableNetworkProxy();

	loadFlightList();
	loadSocFlightList();

	this->setWindowState(Qt::WindowMaximized);

	checkUpdate();

	QTimer * tUpdateDB = new QTimer();
	connect(tUpdateDB, &QTimer::timeout, this, &MaoFlightMap::checkDB);
	tUpdateDB->setSingleShot(true);
	tUpdateDB->start(1000);
}

MaoFlightMap::~MaoFlightMap()
{
	if (true == tFS.isRunning()||
		true == tFly.isRunning()||
		true == tLookup.isRunning())//为了升级数据库被拒绝时，安全地关闭程序，可以寻找更恰当的方法
		shutdownThreads();

	QFile::remove(Mao_Flight_HTML_Lookup_Web_Path.section("", 9));//清理临时网页文件
	QFile::remove(Mao_Flight_HTML_Trace_Web_Path.section("", 9));

	db.close();
}

void MaoFlightMap::closeEvent(QCloseEvent* e)
{
	shutdownThreads();

	e->accept();
}
void MaoFlightMap::shutdownThreads()
{
	tFS.setShutdown();
	tFly.setShutdown();
	tLookup.setShutdown();

	emit SHUTDOWN();

	ui.MaoStatusBar->showMessage(zh("正在结束 FS 连接..."));
	while (true == tFS.isRunning())
		QApplication::processEvents();

	ui.MaoStatusBar->showMessage(zh("正在结束飞行记录..."));
	while (true == tFly.isRunning())
		QApplication::processEvents();

	ui.MaoStatusBar->showMessage(zh("正在结束航班查找..."));
	while (true == tLookup.isRunning())
		QApplication::processEvents();
}


// TODO
//void QWidget::focusInEvent(QFocusEvent *event)
//{
//	emit windowTitleChanged(nullptr);//利用这个信号来通知 UI，【TODO - 注意这个信号会不会被正常网页发出！】
//}
void QAbstractItemView::focusInEvent(QFocusEvent * event)
{
	emit iconSizeChanged(QSize(Mao_Flight_TableWidget_Width_Expend, 0));//利用这个信号来通知 UI
}
void QAbstractItemView::focusOutEvent(QFocusEvent * event) //为了刷新当前行号
{
	emit iconSizeChanged(QSize(0, 0));//利用这个信号来通知 UI
}
void MaoFlightMap::FlightListFocusIn(const QSize & newWidth)
{
	if (Mao_Flight_TableWidget_Width_Expend == newWidth.width())
	{
		ui.MaoTabFrame->setMinimumWidth(newWidth.width());
		ui.MaoTabFrame->setMaximumWidth(newWidth.width());
	}
	currentROW = ui.MaoFlightList->currentRow();
}
void MaoFlightMap::SocListFocusIn(const QSize & newWidth)
{
	if (Mao_Flight_TableWidget_Width_Expend == newWidth.width())//is Mao_Flight_TableWidget_Width_Expend
	{
		ui.MaoTabFrame->setMinimumWidth(Mao_Flight_TableWidget_Width_Expend_SOC);
		ui.MaoTabFrame->setMaximumWidth(Mao_Flight_TableWidget_Width_Expend_SOC);
	}
}
void MaoFlightMap::tableShrink()
{
	ui.MaoTabFrame->setMinimumWidth(Mao_Flight_TableWidget_Width_Shrink);
	ui.MaoTabFrame->setMaximumWidth(Mao_Flight_TableWidget_Width_Shrink);
}
void MaoFlightMap::tableShrinkTab(int index)
{
	switch (index)
	{
	case 0:
		ui.MaoTabFrame->setMinimumWidth(Mao_Flight_TableWidget_Width_Shrink);
		ui.MaoTabFrame->setMaximumWidth(Mao_Flight_TableWidget_Width_Shrink);
		ui.MaoLookupView->hide();
		ui.MaoTraceView->show();
		break;
	case 1:
		ui.MaoTabFrame->setMinimumWidth(Mao_Flight_TableWidget_Width_Expend);
		ui.MaoTabFrame->setMaximumWidth(Mao_Flight_TableWidget_Width_Expend);
		ui.MaoTraceView->hide();
		ui.MaoLookupView->show();
		break;
	case 2:
		ui.MaoTabFrame->setMinimumWidth(Mao_Flight_TableWidget_Width_Expend_SOC);
		ui.MaoTabFrame->setMaximumWidth(Mao_Flight_TableWidget_Width_Expend_SOC);
		ui.MaoTraceView->hide();
		ui.MaoLookupView->show();
		break;
	}
}



void MaoFlightMap::checkDB()
{
	QSqlQuery dbQuery(db);
	dbQuery.exec("SELECT StartLat FROM FlightList");
	
	if (true == dbQuery.next())//有StartLat等列，当前是老数据库，需要更新
	{
		dbQuery.finish();

		QMessageBox::StandardButton canUpdate = QMessageBox::information(this,
			zh("数据库升级"),
			zh("您的航班飞行数据库是1.0～1.2版本的，需要升级后才能使用。\n\n") +
			zh("是否现在帮您升级？（会帮您自动备份）\n\n") +
			zh("有需要可联系软件作者：MaoJianwei2012@126.com"),
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes
			);
		if (QMessageBox::Yes == canUpdate)
		{
			if (false == updateDB())
			{
				QMessageBox::information(this,
					zh("备份失败"),
					zh("请确认您是否拥有写文件的权限？\n\n为避免数据丢失，升级将不会进行。"),
					QMessageBox::Ok);
				QApplication::exit(Mao_Flight_DB_Backup_Fail);
			}
			else
				QMessageBox::information(this,
					zh("数据库升级"),
					zh("升级成功，Welcome aboard！"),
					QMessageBox::Ok);
		}
		else
			QApplication::exit(Mao_Flight_DB_Update_Reject);
	}
	else
		dbQuery.finish();

	delete ((QTimer*)sender());
}
bool MaoFlightMap::updateDB()
{
	if (false == backupDB())
		return false;

	//开始更新
	return updateDBfromV12();
}
bool MaoFlightMap::backupDB()
{
	QString backupFilename = Mao_Flight_DB_Path;
	backupFilename.resize(backupFilename.size() - 3);
	backupFilename += "_v12_backup.db";

	QFile::remove(backupFilename);
	if (false == QFile::copy(Mao_Flight_DB_Path, backupFilename))
		return false;
	else
		return true;
}
bool MaoFlightMap::updateDBfromV12()
{
	QSqlQuery dbOldQuery(db);
	dbOldQuery.exec("ALTER TABLE FlightList RENAME TO FlightListOld");
	dbOldQuery.exec("SELECT "
		"TaskNo,"
		"DepDate,"
		"DepTime,"
		"ArrDate,"
		"ArrTime,"
		"FlightNo,"
		"PlaneModel,"
		"DepICAO,"
		"ArrICAO,"
		"DivArrICAO,"
		"DetailTable"
		" FROM FlightListOld");



	db.transaction();

	QSqlQuery dbNewQuery(db);
	dbNewQuery.exec("CREATE TABLE FlightList ("
		"TaskNo INT primary key not null unique,"
		"DepDate text,"
		"DepTime text,"
		"ArrDate text,"
		"ArrTime text,"
		"FlightNo text,"
		"PlaneModel text,"
		"DepICAO text,"
		"ArrICAO text,"
		"DivArrICAO text,"
		"DetailTable text"
		")");

	QString query = "INSERT INTO FlightList VALUES ("
		"%1,"
		"\"%2\","
		"\"%3\","
		"\"%4\","
		"\"%5\","
		"\"%6\","
		"\"%7\","
		"\"%8\","
		"\"%9\","
		"\"%10\","
		"\"%11\""
		")";

	
	while (true == dbOldQuery.next())
	{
		dbNewQuery.exec(
			query.arg(dbOldQuery.value(0).toInt())
			.arg(dbOldQuery.value(1).toString())
			.arg(dbOldQuery.value(2).toString())
			.arg(dbOldQuery.value(3).toString())
			.arg(dbOldQuery.value(4).toString())
			.arg(dbOldQuery.value(5).toString())
			.arg(dbOldQuery.value(6).toString())
			.arg(dbOldQuery.value(7).toString())
			.arg(dbOldQuery.value(8).toString())
			.arg(dbOldQuery.value(9).toString())
			.arg(dbOldQuery.value(10).toString())
			);

	}

	dbOldQuery.exec("DROP TABLE FlightListOld");

	db.commit();


	return true;
}



void MaoFlightMap::lowToHigh(const QString & text)
{
	qobject_cast<QLineEdit *>(sender())->setText(text.toUpper());
}


void MaoFlightMap::lookupFlightMap()
{
	if (-1 != currentROW)
	{
		emit sGetFlightMap(currentROW + 1);
	}
}
void MaoFlightMap::startEndFlight()
{
	if (Record == tFly.getRecordControl())
	{
		tFly.setRecordControl(Complete);
		tFS.setAutoRefresh(true);
	}
	else
	{
		ui.MaoArrDate->clear();
		ui.MaoArrTime->clear();
		ui.MaoDivArrICAO->clear();

		tFS.setAutoRefresh(false);
		genTraceHTML();
		emit startRecord(ui.MaoFlightNo->text(), ui.MaoArrICAO->text());
	}
}
void MaoFlightMap::genTraceHTML()
{
	QFile::remove(Mao_Flight_HTML_Trace_Web_Path.section("", 9));
	QFile::copy(":/MaoFlightMap/MaoTraceWeb.html", Mao_Flight_HTML_Trace_Web_Path.section("", 9));
	QFile::setPermissions(Mao_Flight_HTML_Trace_Web_Path.section("", 9), QFileDevice::WriteOwner | QFileDevice::ReadOwner);
}
void MaoFlightMap::cancelFlight()
{
	tFly.setRecordControl(Cancel);
	tFS.setAutoRefresh(true);
}
void MaoFlightMap::clickSOC()
{
	disableNetworkProxy();

	ui.MaoSOCglobalMap->setEnabled(false);
	ui.MaoSOCupdate->setEnabled(false);
	ui.MaoSOCusername->setReadOnly(true);
	ui.MaoSOCpassword->setReadOnly(true);
	emit sGetSOC(ui.MaoSOCusername->text(), ui.MaoSOCpassword->text());
}

void MaoFlightMap::initDriveCheck()
{
	DWORD count = GetLogicalDrives();
	count >>= 3;// 从 D 盘开始扫
	char drive = 'D';

	int i = 4;
	for (; i <= 26; i++)//扫 D~Z
	{
		if (0 == (count & 0x01))
		{
			count >>= 1;
			drive++;
		}
		else
		{
			QString root = QString(drive) + ":\\";
			unsigned int temp = GetDriveTypeA(root.toLocal8Bit().data());
			if (DRIVE_REMOVABLE == temp || DRIVE_FIXED == temp)
				break;
		}
	}

	if (i > 26)
	{
		//TODO - error
	}
	else
	{
		Mao_Flight_DB_Path = QString(drive) + ":/MaoFlightRecord.db";
		Mao_Flight_HTML_File_Path = QString(drive) + ":/MaoFlightRecord.html";
		Mao_Flight_HTML_Lookup_Web_Path = QString("file:///") + drive + ":/MaoFlightRecord.html";
		Mao_Flight_HTML_Trace_Web_Path = QString("file:///") + drive + ":/MaoTraceWeb.html";
		emit pathReady();
	}
}
void MaoFlightMap::initDatabase()
{
	db = QSqlDatabase::addDatabase("QSQLITE", "MaoFlightMap");
	db.setDatabaseName(Mao_Flight_DB_Path);

	if (false == db.open())
		throw 0;//TODO

	QSqlQuery dbQuery(db);

	dbQuery.exec("CREATE TABLE FlightList ("
		"TaskNo INT primary key not null unique,"
		"DepDate text,"
		"DepTime text,"
		"ArrDate text,"
		"ArrTime text,"
		"FlightNo text,"
		"PlaneModel text,"
		"DepICAO text,"
		"ArrICAO text,"
		"DivArrICAO text,"
		//"StartLat text,"
		//"StartLon text,"
		//"EndLat text,"
		//"EndLon text,"
		"DetailTable text"
		")");

	//**** 1.2 版
	//dbQuery.exec("CREATE TABLE FlightList ("
	//	"TaskNo INT primary key not null unique,"
	//	"DepDate text,"
	//	"DepTime text,"
	//	"ArrDate text,"
	//	"ArrTime text,"
	//	"FlightNo text,"
	//	"PlaneModel text,"
	//	"DepICAO text,"
	//	"ArrICAO text,"
	//	"DivArrICAO text,"
	//	"StartLat text,"
	//	"StartLon text,"
	//	"EndLat text,"
	//	"EndLon text,"
	//	"DetailTable text"
	//	")");
}

void MaoFlightMap::initThread()
{
	tFly.start();
	tFly.moveToThread(&tFly);

	tLookup.start();
	tLookup.moveToThread(&tLookup);

	tFS.start();
	tFS.moveToThread(&tFS);
}

void MaoFlightMap::initWeb()
{
	ui.MaoTraceView->setHtml("<img src=\"qrc:/MaoFlightMap/bupt.jpg\" style=\"max-width:100%;\"/>");//load(QUrl("qrc:/MaoFlightMap/bupt"));

	ui.MaoLookupView->setHtml("<img src=\"qrc:/MaoFlightMap/bupt.jpg\" style=\"max-width:100%;\"/>");
	ui.MaoLookupView->hide();
}

void MaoFlightMap::initFlightTableView()
{
	ui.MaoFlightList->clear();
	ui.MaoFlightList->setRowCount(0);

	ui.MaoFlightList->setColumnCount(Mao_Flight_DB_FlightList_Column);

	for (int i = 0; i < Mao_Flight_DB_FlightList_Column; i++)
		ui.MaoFlightList->setHorizontalHeaderItem(i, new QTableWidgetItem(zh(FlightHead[i])));
}
void MaoFlightMap::initSocTableView()
{
	ui.MaoSOCflightList->clear();
	ui.MaoSOCflightList->setRowCount(0);

	ui.MaoSOCflightList->setColumnCount(Mao_Flight_DB_SOC_Column);

	for (int i = 0; i < Mao_Flight_DB_SOC_Column; i++)
		ui.MaoSOCflightList->setHorizontalHeaderItem(i, new QTableWidgetItem(zh(SocHead[i])));
}

void MaoFlightMap::initFlightListViewUI()
{
	ui.MaoFlightList->setAcceptDrops(false);
	ui.MaoFlightList->setAlternatingRowColors(true);
	ui.MaoFlightList->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui.MaoFlightList->setShowGrid(true);
	//ui.MaoFlightList->setSortingEnabled(true);
	//ui.MaoFlightList->sortByColumn(0, Qt::AscendingOrder);
	ui.MaoFlightList->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.MaoFlightList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.MaoFlightList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//ui.MaoFlightList->setModel(&flightListModel);
	//ui.MaoFlightList->horizontalHeader()->setMinimumWidth(40);


	ui.MaoSOCflightList->setAcceptDrops(false);
	ui.MaoSOCflightList->setAlternatingRowColors(true);
	ui.MaoSOCflightList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.MaoSOCflightList->setShowGrid(true);
	//ui.MaoSOCflightList->setSortingEnabled(true);
	//ui.MaoSOCflightList->sortByColumn(0, Qt::AscendingOrder);
	ui.MaoSOCflightList->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.MaoSOCflightList->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.MaoSOCflightList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//ui.MaoSOCflightList->setModel(&flightListModel);
	//ui.MaoSOCflightList->horizontalHeader()->setMinimumWidth(40);


	/*
	ui.MaoOneFlightDetial->setAcceptDrops(false);
	ui.MaoOneFlightDetial->setAlternatingRowColors(true);
	ui.MaoOneFlightDetial->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.MaoOneFlightDetial->setShowGrid(true);
	//ui.MaoOneFlightDetial->setSortingEnabled(true);
	//ui.MaoOneFlightDetial->sortByColumn(0, Qt::AscendingOrder);
	ui.MaoOneFlightDetial->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.MaoOneFlightDetial->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.MaoOneFlightDetial->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//ui.MaoOneFlightDetial->setModel(&flightListModel);
	//ui.MaoOneFlightDetial->horizontalHeader()->setMinimumWidth(40);


	ui.MaoOneFlightData->setAcceptDrops(false);
	ui.MaoOneFlightData->setAlternatingRowColors(true);
	ui.MaoOneFlightData->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.MaoOneFlightData->setShowGrid(true);
	//ui.MaoOneFlightData->setSortingEnabled(true);
	//ui.MaoOneFlightData->sortByColumn(0, Qt::AscendingOrder);
	ui.MaoOneFlightData->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.MaoOneFlightData->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.MaoOneFlightData->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	//ui.MaoOneFlightData->setModel(&flightListModel);
	//ui.MaoOneFlightData->horizontalHeader()->setMinimumWidth(40);
	*/
}
void MaoFlightMap::enableNetworkProxy()
{
	// use Mao Bing Javascript API, we don't need proxy. http://www.maojianwei.com/softwareResource/MaoBingMapControl_english
	return;
	//QNetworkProxy proxy;
	//proxy.setType(QNetworkProxy::Socks5Proxy);
	//proxy.setHostName("www.maojianwei.com");
	//proxy.setPort(1080);
	//QNetworkProxy::setApplicationProxy(proxy);
}
void MaoFlightMap::disableNetworkProxy()
{
	// use Mao Bing Javascript API, we don't need proxy. http://www.maojianwei.com/softwareResource/MaoBingMapControl_english
	return;
	//QNetworkProxy proxy;
	//proxy.setType(QNetworkProxy::NoProxy);
	//QNetworkProxy::setApplicationProxy(proxy);
}

void MaoFlightMap::loadFlightList()
{

	initFlightTableView();
	int row = 0;
	QSqlQuery dbQuery(db);
	dbQuery.exec("SELECT "
		"DepDate,"
		"DepTime,"
		"ArrDate,"
		"ArrTime,"
		"FlightNo,"
		"PlaneModel,"
		"DepICAO,"
		"ArrICAO,"
		"DivArrICAO "
		"FROM FlightList");

	QTableWidgetItem * temp;
	while (true == dbQuery.next())
	{
		ui.MaoFlightList->setRowCount(ui.MaoFlightList->rowCount() + 1);
		for (int i = 0; i < Mao_Flight_DB_FlightList_Column; i++)
		{
			temp = new QTableWidgetItem(dbQuery.value(i).toString());
			if (i <= 4 || 7 == i)
				temp->setFlags((temp->flags()) & (~Qt::ItemIsEditable));//只允许修正“机型”、“起飞机场”、“备降机场”
			temp->setTextAlignment(Qt::AlignCenter);
			ui.MaoFlightList->setItem(row, i, temp);
		}
		row++;
	}
}
void MaoFlightMap::loadSocFlightList()
{
	initSocTableView();
	int row = 0;

	QSqlQuery dbQuery(db);
	dbQuery.exec("SELECT "
		"Date,"
		"FlightNo,"
		"PlaneModel,"
		"PlaneName,"
		"DepICAO,"
		"ArrICAO,"
		"Pilot,"
		"Passenger,"
		"Score,"
		"Reputation,"
		"Income"
		" FROM SocFlightList");

	QTableWidgetItem * temp;
	while (true == dbQuery.next())
	{
		ui.MaoSOCflightList->setRowCount(ui.MaoSOCflightList->rowCount() + 1);
		for (int i = 0; i < Mao_Flight_DB_SOC_Column; i++)
		{
			temp = new QTableWidgetItem(dbQuery.value(i).toString());
			temp->setTextAlignment(Qt::AlignCenter);
			ui.MaoSOCflightList->setItem(row, i, temp);
		}
		row++;
	}
}

void MaoFlightMap::checkUpdate()
{
	QUrl url = "http://maojianwei.github.io/softwareUpdate/MaoFlightMap.html";
	QNetworkRequest req = QNetworkRequest(url);
	update.get(req);
}
void MaoFlightMap::loadUpdate(QNetworkReply* reply)
{
	QString data = reply->readAll();
	if (data.indexOf(';') >= 0)
	{
		QStringList edition = data.split(';');
		if (edition.length() >= 4)
		{
			QString version = edition[0];
			QByteArray url = edition[1].toLocal8Bit();
			QByteArray detail = edition[2].toLocal8Bit();
			QString backupStation = edition[3];
			if (Mao_Flight_Version != version)
			{
				QMessageBox::StandardButton choose = QMessageBox::information(this,
					zh("发现新版本！"),
					zh("当前版本：") + Mao_Flight_Version +
					zh("\n最新版本：") + version +
					zh("\n\n更新内容：\n") +
					zh(detail.data()) +
					zh("\n\n官方备用下载点：") + backupStation +
					zh("（点击 Help）"),
					QMessageBox::Ok | QMessageBox::Help | QMessageBox::Cancel, QMessageBox::Ok);
				if (QMessageBox::Ok == choose)
					ShellExecuteA(NULL, "open", "explorer.exe", url.data(), NULL, SW_SHOW);
				else if (QMessageBox::Help == choose)
					ShellExecuteA(NULL, "open", "explorer.exe", backupStation.toLocal8Bit(), NULL, SW_SHOW);
			}
		}
	}
}
void MaoFlightMap::markPin(double lat, double lon)
{
	QString temp = "updatePos(%1,%2)";
	temp = temp.arg(lat).arg(lon);
	ui.MaoTraceView->page()->runJavaScript(temp);
}
void MaoFlightMap::traceWebShow()
{
	enableNetworkProxy();

	ui.MaoTraceView->stop();
	ui.MaoTraceView->load(QUrl(Mao_Flight_HTML_Trace_Web_Path));
}


void MaoFlightMap::lookupWebShow()
{
	enableNetworkProxy();

	ui.MaoLookupView->stop();
	ui.MaoLookupView->load(QUrl(Mao_Flight_HTML_Lookup_Web_Path));
}


QString MaoFlightMap::zh(const char* str)
{
	return china->toUnicode(str);
}
//void MaoFlightMap::selectCell(int row, int column)
//{
//	currentROW = row;
//}

void MaoFlightMap::showUIs(MaoUIsignalS signal)
{
	switch (signal.type)
	{
	case Mao_Flight_UI_TaskNo:
		ui.MaoTaskNo->setText(signal.value);
		break;
	case Mao_Flight_UI_DepDate:
		ui.MaoDepDate->setText(signal.value);
		break;
	case Mao_Flight_UI_DepTime:
		ui.MaoDepTime->setText(signal.value);
		break;
	case Mao_Flight_UI_ArrDate:
		ui.MaoArrDate->setText(signal.value);
		break;
	case Mao_Flight_UI_ArrTime:
		ui.MaoArrTime->setText(signal.value);
		break;
	case Mao_Flight_UI_PlaneModel:
		ui.MaoPlaneModel->setText(signal.value);
		break;
	case Mao_Flight_UI_DepICAO:
		ui.MaoDepICAO->setText(signal.value);
		break;
	case Mao_Flight_UI_DivArrICAO:
		ui.MaoDivArrICAO->setText(signal.value);
		break;
	case Mao_Flight_UI_Web_Trace:
		ui.MaoTraceView->page()->runJavaScript("updatePos(" + signal.value + ")");// TODO - 新的信号机制，待调 2015.09.30 10：45 教二201
		break;
	case Mao_Flight_UI_Status_Message:
		ui.MaoStatusBar->showMessage(signal.value);
		break;
	case Mao_Flight_UI_Soc_Button_Message:
		ui.MaoSOCupdate->setText(signal.value);
		break;
	case Mao_Flight_UI_AirSpeed:
		ui.MaoAirSpeed->setText(signal.value + zh(" 节"));
		break;
	case Mao_Flight_UI_GNDspeed:
		ui.MaoGNDspeed->setText(signal.value + " Km/h");
		break;




	case Mao_Flight_UI_Soc_Complete: // QString 是剩余尝试次数
		ui.MaoSOCusername->setText(zh("请输入正确的 SOC 账号、密码，需要重启软件再试"));
		ui.MaoSOCpassword->setText(zh("登录失败，还可以尝试 %1 次，失败过多需等 15 分钟").arg(signal.value));
		ui.MaoSOCpassword->setEchoMode(QLineEdit::EchoMode::Normal);

		ui.MaoSOCusername->setReadOnly(true);
		ui.MaoSOCpassword->setReadOnly(true);
		ui.MaoSOCglobalMap->setEnabled(true);
		ui.MaoSOCupdate->setEnabled(false);
		ui.MaoSOCupdate->setText(zh("从 SOC 更新"));
		break;
	default:
		throw 0;//TODO - 不应该出现这种情况
	}
}
void MaoFlightMap::showUIb(MaoUIsignalB signal)
{
	switch (signal.type)
	{
	case Mao_Flight_UI_ConnectStatus:
		if (true == signal.value)
		{
			ui.MaoStartEndFlight->setEnabled(true);
			ui.MaoStatusBar->showMessage(zh("FS 连接成功！"));
		}
		else
		{
			ui.MaoStartEndFlight->setEnabled(false);
			ui.MaoStatusBar->showMessage(zh("FS 连接失败，等待 FSX 和 FSUIPC 开启..."));//不能结束航班，但可以取消航班
		}
		break;
	case Mao_Flight_UI_Record_Complete:
		if (true == signal.value)
		{
			ui.MaoCancelFlight->setEnabled(false);
			ui.MaoStatusBar->showMessage(zh("航班成功完成！"));
			loadFlightList();
		}
		else
		{
			ui.MaoCancelFlight->setEnabled(false);
			ui.MaoStatusBar->showMessage(zh("本次航班取消"));
		}
		ui.MaoFlightNo->setReadOnly(false);
		ui.MaoArrICAO->setReadOnly(false);
		break;
	case Mao_Flight_UI_Record_Start:
		traceWebShow();
		ui.MaoFlightNo->setReadOnly(true);
		ui.MaoArrICAO->setReadOnly(true);
		ui.MaoCancelFlight->setEnabled(true);
		ui.MaoStatusBar->showMessage(zh("开始执飞航班..."));
		break;
	case Mao_Flight_UI_Soc_Complete:
		if (true == signal.value)
		{
			loadSocFlightList();
			ui.MaoSOCusername->setReadOnly(true); // 不能登陆其他账号，需要重启才行
			ui.MaoSOCpassword->setReadOnly(true); // 不能登陆其他账号，需要重启才行
			ui.MaoSOCupdate->setEnabled(true);    // 可继续获取本账号的航班记录
			ui.MaoSOCupdate->setText(zh("从 SOC 更新"));
		}
		else
		{
			ui.MaoSOCusername->setText(zh("请输入正确的 SOC 账号、密码，需要重启软件再试"));
			ui.MaoSOCpassword->setText(zh("可能是其他问题，联系软件作者，www.MaoJianwei.com"));
			ui.MaoSOCpassword->setEchoMode(QLineEdit::EchoMode::Normal);
			ui.MaoSOCusername->setReadOnly(true); // 似乎失败后不能重试
			ui.MaoSOCpassword->setReadOnly(true); // 似乎失败后不能重试
			ui.MaoSOCupdate->setEnabled(false);
		}
		ui.MaoSOCglobalMap->setEnabled(true);
		break;
	case Mao_Flight_UI_Soc_Complete_Can_Retry:
		ui.MaoSOCusername->setReadOnly(false);
		ui.MaoSOCpassword->setReadOnly(false);
		ui.MaoSOCglobalMap->setEnabled(true);
		ui.MaoSOCupdate->setEnabled(true);
		ui.MaoSOCupdate->setText(zh("从 SOC 更新\n账号、密码不能为空"));
		break;
	default:
		throw 0;//TODO - 不应该出现这种情况
	}
}