#include "mymainwindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MyMainWindow w;
	w.show();
	return a.exec();
}
