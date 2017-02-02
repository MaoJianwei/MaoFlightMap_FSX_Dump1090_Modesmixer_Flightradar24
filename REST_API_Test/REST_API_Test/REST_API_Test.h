#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_REST_API_Test.h"

class REST_API_Test : public QMainWindow
{
	Q_OBJECT

public:
	REST_API_Test(QWidget *parent = Q_NULLPTR);

private:
	Ui::REST_API_TestClass ui;
};
