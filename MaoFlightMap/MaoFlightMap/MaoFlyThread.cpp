#include "MaoFlyThread.h"

extern QString Mao_Flight_DB_Path;

MaoFlyThread::MaoFlyThread()
{
	this->shutdown = false;
	canRecord = Cancel;
}
MaoFlyThread::~MaoFlyThread()
{
	db.close();
}
void MaoFlyThread::pathReady()
{
	db = QSqlDatabase::addDatabase("QSQLITE", "MaoFlyThread");
	db.setDatabaseName(Mao_Flight_DB_Path);
	if (false == db.open())
		throw 0;
}

void MaoFlyThread::setShutdown()
{
	shutdown = true;
}
void MaoFlyThread::SHUTDOWN()
{
	exit(0);
}




void MaoFlyThread::recordFlight(QString FlightNo, QString ArrICAO)
{

	//确认 FS 连接正常
	if (false == MaoFSthread::isConnected)
		return;//TODO - 连接不成功时，UI 应该不允许启动记录

	canRecord = Record;
	MaoUIsignalS signal;


	//分配执飞序号
	QSqlQuery dbQuery(db);
	dbQuery.exec("SELECT count(TaskNo) FROM FlightList");
	dbQuery.next();
	int TaskNo = dbQuery.value(0).toInt() + 1;
	
	signal = { Mao_Flight_UI_TaskNo, QString::number(TaskNo) };
	emit pushFSdata(signal);



	//识别机型、获取起飞机场、开始GPS经纬度
	DWORD dwResult;

	//机型
	char planeModel[24] = { 0 };
	if (!FSUIPC_Read(0x3500, 24, planeModel, &dwResult) ||
		!FSUIPC_Process(&dwResult))
	{
		//TODO - deal Failure
		//MaoFSthread::isConnected = false;
	}
	QString PlaneModel = planeModel;

	signal = { Mao_Flight_UI_PlaneModel, PlaneModel };
	emit pushFSdata(signal);

	
	//最近机场的ICAO（起飞机场）
	char depICAO[5] = { 0 };
	if (!FSUIPC_Read(0x0658, 4, depICAO, &dwResult) ||
		!FSUIPC_Process(&dwResult))
	{
		//TODO - deal Failure
	}
	QString DepICAO = depICAO;

	signal = { Mao_Flight_UI_DepICAO, DepICAO };
	emit pushFSdata(signal);


	////开始GPS纬度
	//long long latitudeTemp;
	//if (!FSUIPC_Read(0x0560, 8, &latitudeTemp, &dwResult) ||
	//	!FSUIPC_Process(&dwResult))
	//{
	//	//TODO - deal Failure
	//}
	//double nowLat = latitudeTemp * 90.0 / (10001750.0 * 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow

	////开始GPS经度
	//long long longitudeTemp;
	//if (!FSUIPC_Read(0x0568, 8, &longitudeTemp, &dwResult) ||
	//	!FSUIPC_Process(&dwResult))
	//{
	//	//TODO - deal Failure
	//}
	//double nowLon = longitudeTemp * 360.0 / (65536.0 * 65536.0	* 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow

	

	//获取起飞日期、时间
	int year, month, day, hour, minute, second;
	getNowDateTime(year, month, day, hour, minute, second);
	QString Date = "%1.%2.%3";
	Date = Date.arg(year).arg(month).arg(day);
	QString Time = "%1:%2:%3";
	Time = Time.arg(hour).arg(minute).arg(second);

	signal = { Mao_Flight_UI_DepDate, Date };
	emit pushFSdata(signal);
	signal = { Mao_Flight_UI_DepTime, Time };
	emit pushFSdata(signal);

	//生成航班数据库表名
	QString DetailTableName = "Mao%1%2%3%4%5%6";
	DetailTableName = DetailTableName.arg(year).arg(month).arg(day).arg(hour).arg(minute).arg(second);



	//写入航班记录
	QString query = "INSERT INTO FlightList "
		"("
		"TaskNo,"
		"DepDate,"
		"DepTime,"
		"FlightNo,"
		"PlaneModel,"
		"DepICAO,"
		"ArrICAO,"
		//"StartLat,"
		//"StartLon,"
		"DetailTable"
		")"
		" VALUES "
		"("
		"%1,"
		"\"%2\","
		"\"%3\","
		"\"%4\","
		"\"%5\","
		"\"%6\","
		"\"%7\","
		//"\"%8\","
		//"\"%9\","
		"\"%8\""
		")";
	query = query
		.arg(TaskNo)
		.arg(Date)
		.arg(Time)
		.arg(FlightNo)
		.arg(PlaneModel)
		.arg(DepICAO)
		.arg(ArrICAO)
		//.arg(nowLat, 0, 'g', 10)
		//.arg(nowLon, 0, 'g', 10)
		.arg(DetailTableName);


	if (false == dbQuery.exec(query))
		throw 0;//TODO


	//创建航班详细数据表
	query = "CREATE TABLE " + DetailTableName + " ("
		"WaypointNo INT primary key not null unique,"
		"Latitude text,"
		"Longitude text,"
		"Altitude text,"
		"RadioAlt text,"
		"GNDSpeed text,"
		"IndicatedSPD text"
		")";
	if (false == dbQuery.exec(query))
		throw 0;//TODO


	//开始执飞记录

	emit recordReport({ Mao_Flight_UI_Record_Start, true });

	int WaypointNo = 1;
	bool hasReportFSerror = false;
	QString locEmit = "%1,%2";
	while (Record == canRecord && false == this->shutdown)
	{
		double Lat, Lon, iSPD, gSPD;
		int alt, radioAlt;
		if (true == getFlightData(Lat, Lon, alt, radioAlt, iSPD, gSPD))//获取成功才记录
		{
			storeFlightData(dbQuery, DetailTableName, WaypointNo, Lat, Lon, alt, radioAlt, iSPD, gSPD);
			WaypointNo++;
			hasReportFSerror = false;

			emit locTrace(Lat, Lon);

			signal = { Mao_Flight_UI_AirSpeed, QString::number(iSPD, 'g', 4) };
			emit pushFSdata(signal);

			signal = { Mao_Flight_UI_GNDspeed, QString::number(gSPD, 'g', 4) };
			emit pushFSdata(signal);
		}
		else if (false == hasReportFSerror)//只汇报一次错误
		{
			MaoFSthread::isConnected = false;
			hasReportFSerror = true;//只汇报一次错误
		}

		for (int i = 0; i < 4 && Record == canRecord && false == this->shutdown; i++)//如果FS失败，则放弃这次记录，等待时间后，再次尝试记录；或者等待 CANCEL、FINISH 控制标记
			sleep(1);
	}

	if (Cancel == canRecord || true == this->shutdown)
	{
		//删除航班详细数据表
		query = "DROP TABLE " + DetailTableName;
		dbQuery.exec(query);

		//删除航班记录
		query = 
			"DELETE FROM FlightList "
			"WHERE TaskNo = %1";
		query = query.arg(TaskNo);
		dbQuery.exec(query);
	}
	else if (Complete == canRecord)
	{
		//获取降落日期、时间
		getNowDateTime(year, month, day, hour, minute, second);
		Date = "%1.%2.%3";
		Date = Date.arg(year).arg(month).arg(day);
		Time = "%1:%2:%3";
		Time = Time.arg(hour).arg(minute).arg(second);

		signal = { Mao_Flight_UI_ArrDate, Date };
		emit pushFSdata(signal);
		signal = { Mao_Flight_UI_ArrTime, Time };
		emit pushFSdata(signal);


		//最近机场的ICAO（实际降落机场）
		char actualArrICAO[5] = { 0 };
		if (!FSUIPC_Read(0x0658, 4, actualArrICAO, &dwResult) ||
			!FSUIPC_Process(&dwResult))
		{
			//TODO - deal Failure
		}
		QString ActualArrICAO = actualArrICAO;


		////结束GPS纬度
		//if (!FSUIPC_Read(0x0560, 8, &latitudeTemp, &dwResult) ||
		//	!FSUIPC_Process(&dwResult))
		//{
		//	//TODO - deal Failure
		//}
		//nowLat = latitudeTemp * 90.0 / (10001750.0 * 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow

		////结束GPS经度
		//if (!FSUIPC_Read(0x0568, 8, &longitudeTemp, &dwResult) ||
		//	!FSUIPC_Process(&dwResult))
		//{
		//	//TODO - deal Failure
		//}
		//nowLon = longitudeTemp * 360.0 / (65536.0 * 65536.0	* 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow


		//构造 SQL 语句，修改 FlightList 航班记录
		query =
			"UPDATE FlightList SET "
			"ArrDate = \"%1\","
			"ArrTime = \"%2\","
			//"EndLat = \"%3\","
			//"EndLon = \"%4\","
			;
		query = query
			.arg(Date)
			.arg(Time)
			//.arg(nowLat, 0, 'g', 10)
			//.arg(nowLon, 0, 'g', 10)
			;

		//-------判定是否备降
		if (ActualArrICAO == ArrICAO)
		{
			query += "DivArrICAO = \"\"";//没有备降
		}
		else
		{
			query += "DivArrICAO = \"%1\"";//备降
			query = query.arg(ActualArrICAO);

			signal = { Mao_Flight_UI_DivArrICAO, ActualArrICAO };
			emit pushFSdata(signal);
		}

		query += "WHERE TaskNo = %1";
		query = query.arg(TaskNo);

		//完善 FlightList 记录
		dbQuery.exec(query);
	}
	

	if (Complete == canRecord)
		emit recordReport({ Mao_Flight_UI_Record_Complete, true });
	else
		emit recordReport({ Mao_Flight_UI_Record_Complete, false });//CANCEL


	//其他善后工作
	
}



bool MaoFlyThread::getFlightData(
	double & Lat, 
	double & Lon, 
	int & alt, 
	int & radioAlt, 
	double & iSPD, 
	double & gSPD)
{
	DWORD dwResult;

	//纬度
	long long latitudeTemp;
	if (!FSUIPC_Read(0x0560, 8, &latitudeTemp, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	Lat = latitudeTemp * 90.0 / (10001750.0 * 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow


	//经度
	long long longitudeTemp;
	if (!FSUIPC_Read(0x0568, 8, &longitudeTemp, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	Lon = longitudeTemp * 360.0 / (65536.0 * 65536.0	* 65536.0 * 65536.0);//!!! use .0 (double), avoid overflow


	//海拔
	if (!FSUIPC_Read(0x0574, 4, &alt, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	alt *= 3.28084;


	//无线电高度（离地高度）
	if (!FSUIPC_Read(0x31E4, 4, &radioAlt, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	radioAlt = radioAlt * 3.28084 / 65535.0;

	//表速
	int IndicatedSpeed;
	if (!FSUIPC_Read(0x02BC, 4, &IndicatedSpeed, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	iSPD = IndicatedSpeed / 128.0;// knot

	//地速
	int GNDSpeed;
	if (!FSUIPC_Read(0x02B4, 4, &GNDSpeed, &dwResult) ||
		!FSUIPC_Process(&dwResult))
		return false;
	gSPD = GNDSpeed / 65536.0 * 3.6;// km/h

	return true;
}

bool MaoFlyThread::storeFlightData(
	QSqlQuery & dbQuery,
	QString & DetailTableName,
	int & WaypointNo,
	double & Lat,
	double & Lon,
	int & alt,
	int & radioAlt,
	double & iSPD,
	double & gSPD)
{
	QString query =
		"INSERT INTO " + DetailTableName + " VALUES ("
		"%1,"
		"\"%2\","
		"\"%3\","
		"\"%4\","
		"\"%5\","
		"\"%6\","
		"\"%7\""
		")";
	query = query
		.arg(WaypointNo)
		.arg(Lat, 0, 'g', 10)
		.arg(Lon, 0, 'g', 10)
		.arg(alt)
		.arg(radioAlt)
		.arg(gSPD)
		.arg(iSPD);

	return dbQuery.exec(query);
}

void MaoFlyThread::setRecordControl(RecordControlStatus control)
{
	canRecord = control;
}

RecordControlStatus MaoFlyThread::getRecordControl()
{
	return canRecord;
}

void MaoFlyThread::getNowDateTime(int& year, int & month, int& day, int& hour, int& minute, int&second)
{
	QDateTime now = QDateTime::currentDateTime();
	QDate nowD = now.date();
	QTime nowT = now.time();

	year = nowD.year();
	month = nowD.month();
	day = nowD.day();

	hour = nowT.hour();
	minute = nowT.minute();
	second = nowT.second();
}