#pragma once
#include <QThread>
#include <QTimer>

#include "IPCuser.h"
#include "FSUIPC_User.h"


#include "MaoUIsignal.h"

class MaoFSthread :	public QThread
{
	Q_OBJECT

public:
	MaoFSthread();
	~MaoFSthread();
	void run()
	{
		conFS.start(1000);
		exec();
	}

	void setAutoRefresh(bool enabled);

	static bool isConnected ;

	void setShutdown();


	signals:
	void connectResult(MaoUIsignalB);
	void pushFSdata(MaoUIsignalS);


	public slots:
	void SHUTDOWN();


	private slots:
	void connectFS();//受isConnected控制，持续连接FSUIPC
	void refreshData();//受autoRefresh控制，受函数流程控制，持续更新 机型、起飞机场

private:
	bool autoRefresh;

	QTimer conFS;
	QTimer refresh;

	bool shutdown;
};

