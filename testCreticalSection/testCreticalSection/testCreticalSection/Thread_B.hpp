#pragma once

#include <Windows.h>
#include <QThread>
#include <QTextStream>

class Thread_B : public QThread
{
	Q_OBJECT

public:
	Thread_B(CRITICAL_SECTION & cs):cs(cs)
	{
	}

	~Thread_B()
	{
	}

	void run()
	{
		int count = 0;
		while (count<1) {
			msleep(10000);
			QTextStream(stdout) << QString("to in --- Current: %1").arg((qint64)(currentThreadId())) << endl;

			EnterCriticalSection(&cs);
			QTextStream(stdout) << QString("in ! --- Current: %1").arg((qint64)(currentThreadId())) << endl;
			int a = 0;
			//LeaveCriticalSection(&cs);
			count++;
			QTextStream(stdout) << QString("LockCount: %1, Recursion: %2, Owning: %3, Spin: %4 --- Current: %5")
				.arg(cs.LockCount).arg(cs.RecursionCount).arg((qint64)(cs.OwningThread)).arg(cs.SpinCount).arg((qint64)(currentThreadId())) << endl;
		}
	}

private:
	CRITICAL_SECTION & cs;
};