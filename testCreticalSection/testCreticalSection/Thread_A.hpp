#pragma once

#include <Windows.h>
#include <QThread>
#include <QTextStream>

class Thread_A : public QThread
{
	Q_OBJECT

public:
	Thread_A(CRITICAL_SECTION & cs) :cs(cs)
	{
	}

	~Thread_A()
	{
	}

	void run()
	{
		int count = 0;
		QTextStream(stdout) << QString("LockCount: %1, Recursion: %2, Owning: %3, Spin: %4 --- Current: %5")
			.arg(cs.LockCount).arg(cs.RecursionCount).arg((qint64)(cs.OwningThread)).arg(cs.SpinCount).arg((qint64)(currentThreadId())) << endl;
		while (count<10) {
			EnterCriticalSection(&cs);
			int a = 0;
			//LeaveCriticalSection(&cs);
			msleep(500);
			count++;
			QTextStream(stdout) << QString("LockCount: %1, Recursion: %2, Owning: %3, Spin: %4 --- Current: %5")
				.arg(cs.LockCount).arg(cs.RecursionCount).arg((qint64)(cs.OwningThread)).arg(cs.SpinCount).arg((qint64)(currentThreadId())) << endl;
		}


		msleep(10000);
		count = 0;
		while (count<10) {
			LeaveCriticalSection(&cs);
			int a = 0;
			msleep(500);
			count++;
			QTextStream(stdout) << QString("LockCount: %1, Recursion: %2, Owning: %3, Spin: %4 release --- Current: %5")
				.arg(cs.LockCount).arg(cs.RecursionCount).arg((qint64)(cs.OwningThread)).arg(cs.SpinCount).arg((qint64)(currentThreadId())) << endl;
		}
	}

private:
	CRITICAL_SECTION & cs;
};