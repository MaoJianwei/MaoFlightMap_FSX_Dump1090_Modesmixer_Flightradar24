#include <QtCore/QCoreApplication>

#include "Thread_A.hpp"
#include "Thread_B.hpp"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QTextStream(stdout) << QString("%1, %2, %3, %4, %5").arg(4).arg(0).arg(1).arg(1).arg(0) << endl;

	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);

	Thread_A * AAA = new Thread_A(cs);
	Thread_B * BBB = new Thread_B(cs);
	
	AAA->start();
	BBB->start();

	a.exec();

	DeleteCriticalSection(&cs);

	return 0;
}
