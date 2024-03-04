#include "mainwindow.h"
#include <QApplication>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("WiNGSPAN Delivery Receipt Generator");

    w.show();
    return a.exec();
}
