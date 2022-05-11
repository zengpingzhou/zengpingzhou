#include "UI/ui.h"

#include <QApplication>
#include <QTranslator>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	QTranslator tr;
	tr.load(":/res/zh_CN.qm", a.applicationDirPath());
	a.installTranslator(&tr);
	MyUI w;
	w.resize(800,600);
	w.show();
	return a.exec();
}