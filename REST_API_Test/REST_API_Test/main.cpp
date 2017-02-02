#include "REST_API_Test.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	REST_API_Test w;
	w.show();
	return a.exec();
}
