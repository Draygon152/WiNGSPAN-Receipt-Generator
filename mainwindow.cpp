#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configure kmGroupBox
    ui->kmGroupBox->setStyleSheet("QGroupBox { border: 1px solid gray; }");
    ui->kmStatusLabel->clear();

    // Configure locationGroupBox
    ui->locationGroupBox->setStyleSheet("QGroupBox { border: 1px solid gray; }");

    // Configure generateReceiptButton
    ui->generateReceiptButton->setEnabled(false);
    ui->receiptStatusLabel->clear();

    loadReceiptNumber();
    loadReceiptTemplate();
}

MainWindow::~MainWindow()
{
    delete km;
    delete ui;
}

void MainWindow::on_getKMInfoButton_clicked()
{
    ui->getKMInfoButton->setEnabled(false);
    ui->kmStatusLabel->clear();
    QString kmLink = ui->kmLineEdit->text();
    km = new KMParse(kmLink);
    
    if (km->hasValidKMLink())
    {
        ui->kmStatusLabel->setText("Killmail Info Fetched!");
        ui->kmStatusLabel->setStyleSheet("QLabel { color: green; }");
        // TODO: Add success sound
        validKMFetched = true;
        ui->getKMInfoButton->setEnabled(true);
        ui->generateReceiptButton->setEnabled(true);
    }

    else
    {
        ui->kmStatusLabel->setText("Invalid Killmail Link");
        ui->kmStatusLabel->setStyleSheet("QLabel { color: red; }");
        QApplication::beep(); // TODO: Replace with failure sound
        validKMFetched = false;
        ui->getKMInfoButton->setEnabled(true);
        ui->generateReceiptButton->setEnabled(false);
    }

    QTimer::singleShot(10000,
                       [&]()
                       {
                           ui->kmStatusLabel->clear();
                       });
}

void MainWindow::on_kmLineEdit_returnPressed()
{
    on_getKMInfoButton_clicked();
}

void MainWindow::on_generateReceiptButton_clicked()
{
    if (validKMFetched)
    {
        ui->receiptStatusLabel->clear();

        QClipboard* clipboard = QGuiApplication::clipboard();;
        QString output;

        ui->locationLineEdit->setEnabled(false);
        km->setLocation(ui->locationLineEdit->text());
        QList<QString> outputArgs = km->getOutputArgs();
        QString paddedReceiptNumber = QStringLiteral("%1").arg(curReceiptNumber, 5, 10, QLatin1Char('0'));

        output = RECEIPT_TEMPLATE.arg(paddedReceiptNumber,
                                      outputArgs[0],
                                      outputArgs[1],
                                      outputArgs[2],
                                      outputArgs[3],
                                      outputArgs[4],
                                      outputArgs[5],
                                      outputArgs[6],
                                      outputArgs[7],
                                      outputArgs[8],
                                      outputArgs[9],
                                      outputArgs[10]);

        ui->receiptStatusLabel->setText("Receipt Generated!");
        ui->receiptStatusLabel->setStyleSheet("QLabel { color: green; }");
        ui->locationLineEdit->setEnabled(true);
        updateReceiptNumber();

        QTimer::singleShot(10000,
                           [&]()
                           {
                               ui->receiptStatusLabel->clear();
                           });

        clipboard->setText(output);
        QApplication::beep(); // TODO: Replace with better sound
    }
}

void MainWindow::loadReceiptNumber()
{
    QFile file(RECEIPT_NUMBER_FILENAME);
    // Attempt to open receipt number storage file
    if (file.open(QIODevice::ReadOnly))
    {
        curReceiptNumber = file.readAll().toInt();
        file.close();
    }

    else
    {
        QSaveFile newFile(RECEIPT_NUMBER_FILENAME);
        QByteArray newReceiptCount;
        newReceiptCount.setNum(1);

        newFile.open(QIODevice::WriteOnly);
        newFile.write(newReceiptCount);
        newFile.commit();

        curReceiptNumber = 1;
    }
}

void MainWindow::updateReceiptNumber()
{
    QSaveFile newFile(RECEIPT_NUMBER_FILENAME);
    QByteArray newReceiptCount;
    curReceiptNumber += 1;
    newReceiptCount.setNum(curReceiptNumber);

    newFile.open(QIODevice::WriteOnly);
    newFile.write(newReceiptCount);
    newFile.commit();
}

void MainWindow::loadReceiptTemplate()
{
    QFile file(RECEIPT_TEMPLATE_FILEPATH);

    if (file.open(QIODevice::ReadOnly))
    {
        RECEIPT_TEMPLATE = file.readAll();
        file.close();
    }
}
