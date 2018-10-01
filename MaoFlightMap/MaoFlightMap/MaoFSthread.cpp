#include "MaoFSthread.h"

bool MaoFSthread::isConnected = false;

MaoFSthread::MaoFSthread()
{
	this->shutdown = false;
	isConnected = false;
	autoRefresh = true;

	conFS.moveToThread(this);
	refresh.moveToThread(this);

	connect(&conFS, &QTimer::timeout, this, &MaoFSthread::connectFS);
	connect(&refresh, &QTimer::timeout, this, &MaoFSthread::refreshData);

	conFS.setSingleShot(true);
}
MaoFSthread::~MaoFSthread()
{
	conFS.stop();
	refresh.stop();
	FSUIPC_Close();
}

void MaoFSthread::setShutdown()
{
	this->shutdown = true;
}
void MaoFSthread::SHUTDOWN()
{
	exit(0);
}


void MaoFSthread::connectFS()
{
	if (false == isConnected)
	{
		DWORD dwResult;

		FSUIPC_Close();
		while (FALSE == FSUIPC_Open(SIM_ANY, &dwResult) && false == this->shutdown)
		{
			emit connectResult({ Mao_Flight_UI_ConnectStatus, false });
			FSUIPC_Close();
			sleep(1);
		}
		//if (FSUIPC_ERR_OK == dwResult)//not need
		//{
			emit connectResult({ Mao_Flight_UI_ConnectStatus, true });
			isConnected = true;
			refresh.start(1000);
		//}
	}
	if (false == this->shutdown)
		conFS.start(1000);//即循环检测，又不至于在连接过程中让事件队列一直增
}

void MaoFSthread::refreshData()
{
	if (true == autoRefresh)
	{
		DWORD dwResult;
		char ICAO[5] = { 0 };
		char planeModel[24] = { 0 };

		//机型
		if (!FSUIPC_Read(0x3500, 24, planeModel, &dwResult) ||
			!FSUIPC_Process(&dwResult))
		{
			refresh.stop();
			isConnected = false;
		}
		else
		{
			MaoUIsignalS signal = { Mao_Flight_UI_PlaneModel, planeModel };
			emit pushFSdata(signal);
		}

		//最近机场的ICAO
		if (!FSUIPC_Read(0x0658, 4, ICAO, &dwResult) ||
			!FSUIPC_Process(&dwResult))
		{
			refresh.stop();
			isConnected = false;
		}
		else
		{
			MaoUIsignalS signal = { Mao_Flight_UI_DepICAO, ICAO };
			emit pushFSdata(signal);
		}
	}
}

void MaoFSthread::setAutoRefresh(bool enabled)
{
	autoRefresh = enabled;
}