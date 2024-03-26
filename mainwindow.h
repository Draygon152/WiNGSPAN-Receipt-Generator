#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qfile.h>
#include <qsavefile.h>
#include <qclipboard.h>
#include <qlist.h>
#include <qtimer.h>
#include "KMParse.h"



QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        void on_getKMInfoButton_clicked();
        void on_kmLineEdit_returnPressed();
        void on_generateReceiptButton_clicked();

    private:
        Ui::MainWindow *ui;
        KMParse *km;

        const QString RECEIPT_NUMBER_FILENAME = "ReceiptNumberStorage";
        const QString RECEIPT_TEMPLATE_FILEPATH = ":/data/data/Receipt Template";
        QString RECEIPT_TEMPLATE;
        // RECEIPT NUMBER: %1
        // ITEMS DELIVERED: %2
        // KM ID: %3
        // KM HASH: %4
        // VICTIM ID: %5
        // VICTIM NAME: %6
        // SHIP ID: %7
        // SHIP NAME: %8
        // SHIP POSSESSIVE: %9
        // SYS ID: %10
        // SYS NAME: %11
        // LOCATION: %12

        int curReceiptNumber = -1;
        bool validKMFetched = false;

        void loadReceiptNumber();
        void updateReceiptNumber();
        void loadReceiptTemplate();
};
#endif // MAINWINDOW_H
