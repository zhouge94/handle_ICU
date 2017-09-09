#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <stdio.h>
#include <fcntl.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int pd;
    char buf[32];

   /* pd = open("/proc/boardname", O_RDONLY);

    if (read(pd, buf, 32) < 0){
        perror("read error");
        exit(1);
    }
    if (strncmp(buf,"OK335xS2",8) == 0) {
        ctl_485_flags = CTL_485_FLAGS_NO;
    }
    else if(strncmp(buf,"OK335xS",7) == 0 || strncmp(buf,"OK335xD",7) == 0)
    {
        ctl_485_flags = CTL_485_FLAGS_YES;
    }
    ::close(pd);
*/
    ui->setupUi(this);
    startInit();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
             ui->retranslateUi(this);
             break;
        default:
             break;
    }
}

//initialition
void MainWindow::startInit()
{
    setActionsEnabled(false);

    ui->delayspinBox->setEnabled(false);
    ui->sendmsgBtn->setEnabled(false);
    ui->sendMsgLineEdit->setEnabled(false);
    ui->obocheckBox->setEnabled(false);
    ui->actionAdd->setEnabled(true);
    ui->recvbyteslcdNumber->setNumDigits(8);
    ui->sendbyteslcdNumber->setNumDigits(8);
    //read ms
    timerdly = TIMER_INTERVAL;

    //initilation ms
    obotimerdly = OBO_TIMER_INTERVAL;

    //read time set
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(readMyCom()));
    //continue set
    obotimer = new QTimer(this);
    connect(obotimer, SIGNAL(timeout()), this, SLOT(sendMsg()));

    myCom = NULL;

}

void MainWindow::setActionsEnabled(bool status)
{
    ui->actionSave->setEnabled(status);
    ui->actionClose->setEnabled(status);
    ui->actionLoadfile->setEnabled(status);
    ui->actionCleanPort->setEnabled(status);
    ui->actionWriteToFile->setEnabled(status);
}

void MainWindow::setComboBoxEnabled(bool status)
{
    ui->portNameComboBox->setEnabled(status);
    ui->baudRateComboBox->setEnabled(status);
    ui->dataBitsComboBox->setEnabled(status);
    ui->parityComboBox->setEnabled(status);
    ui->stopBitsComboBox->setEnabled(status);
    ui->hardflowComboBox->setEnabled(status);
}

//open port
void MainWindow::on_actionOpen_triggered()
{
    QString portName = "/dev/" + ui->portNameComboBox->currentText();   //get port
    myCom = new Posix_QextSerialPort(portName, QextSerialBase::Polling);


    if(myCom->open(QIODevice::ReadWrite))
    {
        QMessageBox::information(this, tr("open success"), tr("open succeed ") + portName, QMessageBox::Ok);
    }
    else
    {
        QMessageBox::critical(this, tr("open failed"), tr("open failed ") + portName + tr("\nThe serial port is already in use or does not exists!"), QMessageBox::Ok);
        return;
    }

    //set bound
    myCom->setBaudRate((BaudRateType)ui->baudRateComboBox->currentIndex());

    //set databits
    myCom->setDataBits((DataBitsType)ui->dataBitsComboBox->currentIndex());

    //set parity
    myCom->setParity((ParityType)ui->parityComboBox->currentIndex());

    //set stopbits
    myCom->setStopBits((StopBitsType)ui->stopBitsComboBox->currentIndex());

    //constrol ui
    ui->sendmsgBtn->setEnabled(true);

    setComboBoxEnabled(false);

    ui->sendMsgLineEdit->setEnabled(true);

    ui->actionOpen->setEnabled(false);
    ui->sendMsgLineEdit->setFocus();
    ui->obocheckBox->setEnabled(true);
    ui->actionAdd->setEnabled(false);

    setActionsEnabled(true);

    //start timer
    timer->start(timerdly);
    //set flow
    myCom->setFlowControl(ui->hardflowComboBox->currentIndex() == 1 ? FLOW_HARDWARE : FLOW_OFF);
    //set timeout
    myCom->setTimeout(TIME_OUT);

}


//close port
void MainWindow::on_actionClose_triggered()
{

    myCom->close();
    delete myCom;
    myCom = NULL;
    timer->stop();

    ui->sendmsgBtn->setEnabled(false);

    setComboBoxEnabled(true);

    ui->actionOpen->setEnabled(true);
    ui->sendMsgLineEdit->setEnabled(false);
    ui->obocheckBox->setEnabled(false);
    ui->actionAdd->setEnabled(true);

    setActionsEnabled(false);

    ui->actionWriteToFile->setChecked(false);

}



//read data
void MainWindow::readMyCom()
{
    QByteArray temp = myCom->readAll();

    if(!temp.isEmpty())
    {

        if(write2fileName.isEmpty())
        {
            ui->textBrowser->setTextColor(Qt::lightGray);
            ui->textBrowser->append(tr("receive: "));
            ui->textBrowser->setTextColor(Qt::black);
            if(ui->ccradioButton->isChecked())
            {
                ui->textBrowser->append(temp);
            }
            else if(ui->chradioButton->isChecked())
            {
                ui->textBrowser->append(tr("")+temp.toHex());
            }
            else
            {
                ui->textBrowser->append(tr("")+temp.toInt());
            }

        }
        else
        {
            QFile file(write2fileName);
            //faild
            if(!file.open(QFile::Append | QIODevice::Text))
            {
                QMessageBox::warning(this, tr("write file"), tr("open file %1 faild, can not write\n%2").arg(write2fileName).arg(file.errorString()), QMessageBox::Ok);
                return;
            }
            QTextStream out(&file);
            out<<temp<<endl;
            file.close();
            ui->textBrowser->append(tr("receive:write file over %1").arg(write2fileName));
        }

        ui->recvbyteslcdNumber->display(ui->recvbyteslcdNumber->value() + temp.size());

    }
}

//continue send data
void MainWindow::sendMsg()
{
    qint64 sendbytesnum = 0;
    sendbytesnum = myCom->write(ui->sendMsgLineEdit->text().toAscii());
    ui->sendbyteslcdNumber->display(ui->sendbyteslcdNumber->value()+sendbytesnum);
    ui->textBrowser->append(tr("send: ") + ui->sendMsgLineEdit->text());

}

//send data
void MainWindow::on_sendmsgBtn_clicked()
{
    qint64 sendbytesnum = 0;
    //halt send
    if(ui->sendmsgBtn->text() == tr("halt"))
    {
        obotimer->stop();
        ui->sendmsgBtn->setText(tr("send"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/send.png"));
        return;
    }
    //send null
    if(ui->sendMsgLineEdit->text().isEmpty())
    {
        QMessageBox::information(this, tr("hint"), tr("can not send data"), QMessageBox::Ok);
        return;
    }
    //send data
    sendbytesnum = myCom->write(ui->sendMsgLineEdit->text().toAscii());

    ui->sendbyteslcdNumber->display(ui->sendbyteslcdNumber->value()+(int)sendbytesnum);

    //ui control
    ui->textBrowser->setTextColor(Qt::lightGray);
    ui->textBrowser->append(tr("send: "));
    ui->textBrowser->setTextColor(Qt::black);
    ui->textBrowser->append(ui->sendMsgLineEdit->text());
    //not continue send
    if(!ui->obocheckBox->isChecked())
    {
        ui->sendMsgLineEdit->setFocus();
    }else
    { //continue send
        obotimer->start(obotimerdly);
        ui->sendmsgBtn->setText(tr("halt"));
        ui->sendmsgBtn->setIcon(QIcon(":new/prefix1/src/pause.png"));
    }
}
//clear text
void MainWindow::on_clearUpBtn_clicked()
{
    ui->textBrowser->clear();
}

//timer clear
void MainWindow::on_actionClearBytes_triggered()
{
    if(ui->recvbyteslcdNumber->value() == 0)
    {
        QMessageBox::information(this, tr("hint"), tr("cleared"), QMessageBox::Ok);
    }
    else
    {
        ui->recvbyteslcdNumber->display(0);

    }

    if(ui->sendbyteslcdNumber->value() == 0)
    {
        QMessageBox::information(this, tr("hint"), tr("cleared"), QMessageBox::Ok);
    }
    else
    {
        ui->sendbyteslcdNumber->display(0);

    }
}

//checkBox
void MainWindow::on_obocheckBox_clicked()
{
    if(ui->obocheckBox->isChecked())
    {
        ui->delayspinBox->setEnabled(true);
        //check data start timer

    }
    else
    {
        ui->delayspinBox->setEnabled(false);
        //close timer

    }
}

//save textBrowser
void MainWindow::on_actionSave_triggered()
{

    if(ui->textBrowser->toPlainText().isEmpty())
    {
        QMessageBox::information(this, "hint", tr("can not data send ,please input data"), QMessageBox::Ok);
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("save"), tr("noname.txt"));
    QFile file(filename);
    //exit
    if(file.fileName().isEmpty())
    {
        return;
    }
    //open faild rturn
    if(!file.open(QFile::WriteOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("save file "), tr("open file %1faild, can not save\n%2").arg(filename).arg(file.errorString()), QMessageBox::Ok);
        return;
    }
    //write data to file
    QTextStream out(&file);
    out<<ui->textBrowser->toPlainText();
    file.close();
    //modify title
    setWindowTitle("saved: " + QFileInfo(filename).canonicalFilePath());

}

//exit
void MainWindow::on_actionExit_triggered()
{
    if(myCom->isOpen())
    {
        myCom->close();
        delete myCom;
    }
    this->close();
}

//add port
void MainWindow::on_actionAdd_triggered()
{
    bool ok = false;
    QString portname;

    portname = QInputDialog::getText(this, tr("add port"), tr("dev name"), QLineEdit::Normal, 0, &ok);
    if(ok && !portname.isEmpty())
    {
        ui->portNameComboBox->addItem(portname);

    }
}

//modify ms
void MainWindow::on_delayspinBox_valueChanged(int )
{
    obotimerdly = ui->delayspinBox->value();
}

//set read time
void MainWindow::on_actionReadPortInterval_triggered()
{
    bool ok = false;
    unsigned int interval;

    interval = QInputDialog::getInteger(this, tr("read ms"),
                                        tr("read ms between(10-5000，hm)"), timerdly, 10, 5000, 1, &ok);
    if(ok)
    {
        timerdly = interval;
        if(timer->isActive())
        {
            timer->start(timerdly);
        }

    }
}

//load file
void MainWindow::on_actionLoadfile_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("read file"), 0, tr("save to (*.txt *.log)"));  //
    QFile file(filename);

    //exit
    if(file.fileName().isEmpty())
    {
        return;
    }

    //faild return
    if(!file.open(QFile::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("open faild"),
                             tr("sorry! Lincom can not open this file . please modify permission try to use chmod ."),
                             QMessageBox::Ok);
        return;
    }

    //file size limit exceeded
    if(file.size() > MAX_FILE_SIZE)
    {
        QMessageBox::critical(this, tr("loard faild"), tr("file size is %1 byte, limit exceeded 10000 byte").arg(file.size()), QMessageBox::Ok);
        return;
    }
    //file too large
    if(file.size() > TIP_FILE_SIZE)
    {
        if(QMessageBox::question(this, tr("hint"), tr("You want to open the file is too large, it will consumu more time, to continue to open it ?"),
                                 QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {

            return;
        }
    }

    //read data
    QByteArray line;
    while(!file.atEnd())
    {
        line = file.readLine();
        ui->sendMsgLineEdit->setText(ui->sendMsgLineEdit->text() + tr(line));

    }
    file.close();

}

//清空串口中的I/O数据
void MainWindow::on_actionCleanPort_triggered()
{
    myCom->flush();
}

//写入文件菜单
void MainWindow::on_actionWriteToFile_triggered()
{
    if(ui->actionWriteToFile->isChecked())
    {
        QString filename = QFileDialog::getSaveFileName(this, tr("write file "), 0, tr("save to  (*.*)"));  //
        if(filename.isEmpty())
        {
            ui->actionWriteToFile->setChecked(false);
        }
        else
        {
            write2fileName = filename;
            ui->textBrowser->setEnabled(false);
            ui->actionWriteToFile->setToolTip(tr("stop write file "));
        }
    }
    else
    {
        write2fileName.clear();
        ui->textBrowser->setEnabled(true);
        ui->actionWriteToFile->setToolTip(tr("write to file"));
    }

}

void MainWindow::closeEvent(QCloseEvent *e)
{
    exit(0);
 }

void MainWindow::moveEvent(QMoveEvent *)
{
    this->move(QPoint(0,0));
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    this->showMaximized();
}



void MainWindow::on_nomal_clicked()
{
    ui->sendmsgBtn->setEnabled(true);
    ui->sendMsgLineEdit->setEnabled(true);
    ui->obocheckBox->setEnabled(true);

    ui->textBrowser->setEnabled(true);
    ui->clearUpBtn->setEnabled(true);
    ui->ccradioButton->setEnabled(true);
    ui->dcradioButton->setEnabled(true);
    ui->chradioButton->setEnabled(true);

}


void MainWindow::on_send485_clicked()
{
    ui->sendmsgBtn->setEnabled(true);
    ui->sendMsgLineEdit->setEnabled(true);
    ui->obocheckBox->setEnabled(true);

    ui->textBrowser->setDisabled(true);
    ui->clearUpBtn->setDisabled(true);
    ui->ccradioButton->setDisabled(true);
    ui->dcradioButton->setDisabled(true);
    ui->chradioButton->setDisabled(true);

    if(ctl_485_flags == CTL_485_FLAGS_YES)
    {
        system("echo 1 > /sys/class/misc/io_ctl/gpio_state");

    }
}

void MainWindow::on_recv485_clicked()
{
    ui->sendmsgBtn->setDisabled(true);
    ui->sendMsgLineEdit->setDisabled(true);
    ui->obocheckBox->setDisabled(true);

    ui->textBrowser->setEnabled(true);
    ui->clearUpBtn->setEnabled(true);
    ui->ccradioButton->setEnabled(true);
    ui->dcradioButton->setEnabled(true);
    ui->chradioButton->setEnabled(true);

    if(ctl_485_flags == CTL_485_FLAGS_YES)
    {
        system("echo 0 > /sys/class/misc/io_ctl/gpio_state");
    }
}
