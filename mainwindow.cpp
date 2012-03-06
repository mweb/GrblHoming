#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rs232.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setFixedSize(670,400);
    connect(ui->DecXBtn,SIGNAL(clicked()),this,SLOT(decX()));
    connect(ui->DecYBtn,SIGNAL(clicked()),this,SLOT(decY()));
    connect(ui->DecZBtn,SIGNAL(clicked()),this,SLOT(decZ()));
    connect(ui->IncXBtn,SIGNAL(clicked()),this,SLOT(incX()));
    connect(ui->IncYBtn,SIGNAL(clicked()),this,SLOT(incY()));
    connect(ui->IncZBtn,SIGNAL(clicked()),this,SLOT(incZ()));
    connect(ui->rbutSend,SIGNAL(toggled(bool)),this,SLOT(sendBtn()));
    connect(ui->rbutAdj,SIGNAL(toggled(bool)),this,SLOT(adjustBtn()));
    connect(ui->openFile,SIGNAL(clicked()),this,SLOT(open()));
    connect(ui->Begin,SIGNAL(clicked()),this,SLOT(begin()));
    connect(ui->Stop,SIGNAL(clicked()),this,SLOT(stop()));
    connect(ui->Stop,SIGNAL(clicked()),&readthread,SLOT(stopsig()));
    connect(this,SIGNAL(Stop()),&readthread,SLOT(stopsig()));
    connect(&readthread,SIGNAL(addList(QString)),this,SLOT(receiveList(QString)));
    connect(&readthread,SIGNAL(sendMsg(QString)),this,SLOT(receiveMsg(QString)));
    connect(&readthread,SIGNAL(sendAxis(QString)),this,SLOT(receiveAxis(QString)));
    ui->comboStep->addItem("0.0001");
    ui->comboStep->addItem("0.001");
    ui->comboStep->addItem("0.01");
    ui->comboStep->addItem("0.1");
    ui->comboStep->addItem("1");
    ui->Begin->setEnabled(false);
    ui->Stop->setEnabled(false);
#ifdef Q_WS_X11
    ui->cmbPort->addItem("USB0");
    ui->cmbPort->addItem("USB1");
    ui->cmbPort->addItem("USB2");
    ui->cmbPort->addItem("USB3");
    ui->cmbPort->addItem("USB4");
    ui->cmbPort->addItem("USB5");
    ui->cmbPort->addItem("ACM0");
    ui->cmbPort->addItem("ACM1");
    ui->cmbPort->addItem("ACM2");
    ui->cmbPort->addItem("ACM3");
    ui->cmbPort->addItem("ACM4");
    ui->cmbPort->addItem("ACM5");
#ifdef DEBUG
    ui->cmbPort->setCurrentIndex(6);
#endif
#else
    ui->cmbPort->addItem("COM1");
    ui->cmbPort->addItem("COM2");
    ui->cmbPort->addItem("COM3");
    ui->cmbPort->addItem("COM4");
    ui->cmbPort->addItem("COM5");
    ui->cmbPort->addItem("COM6");
    ui->cmbPort->addItem("COM7");
    ui->cmbPort->addItem("COM8");
    ui->cmbPort->addItem("COM9");
    ui->cmbPort->addItem("COM10");
    ui->cmbPort->addItem("COM11");
    ui->cmbPort->addItem("COM12");
    ui->cmbPort->addItem("COM13");
    ui->cmbPort->addItem("COM14");
    ui->cmbPort->addItem("COM15");
    ui->cmbPort->addItem("COM16");
#ifdef DEBUG
    ui->cmbPort->setCurrentIndex(11);
#endif
#endif

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::incX()
{
    if(SendJog(ui->comboStep->currentText().toFloat(),0,0))
    {
        ui->lcdNumberX->display(ui->lcdNumberX->value()+ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::incY()
{
    if(SendJog(0,ui->comboStep->currentText().toFloat(),0))
    {
        ui->lcdNumberY->display(ui->lcdNumberY->value()+ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::incZ()
{
    if(SendJog(0,0,ui->comboStep->currentText().toFloat()))
    {
        ui->lcdNumberZ->display(ui->lcdNumberZ->value()+ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::decX()
{
    if(SendJog(-ui->comboStep->currentText().toFloat(),0,0))
    {
        ui->lcdNumberX->display(ui->lcdNumberX->value()-ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::decY()
{
    if(SendJog(0,-ui->comboStep->currentText().toFloat(),0))
    {
        ui->lcdNumberY->display(ui->lcdNumberY->value()-ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::decZ()
{
    if(SendJog(0,0,-ui->comboStep->currentText().toFloat()))
    {
        ui->lcdNumberZ->display(ui->lcdNumberZ->value()-ui->comboStep->currentText().toFloat());
        ui->centralWidget->setStatusTip("");
    }
    else
        ui->centralWidget->setStatusTip("Connection error.");
}

void MainWindow::sendBtn()
{
    if(ui->rbutSend->isChecked())
    {
        ui->groupBoxAxis->setEnabled(false);
        ui->groupBoxSend->setEnabled(true);
        ui->rbutAdj->setChecked(false);
        ui->returnHome->setEnabled(true);
    }
}

void MainWindow::adjustBtn()
{
    if(ui->rbutAdj->isChecked())
    {
        ui->groupBoxAxis->setEnabled(true);
        ui->groupBoxSend->setEnabled(false);
        ui->rbutSend->setChecked(false);
        ui->returnHome->setEnabled(false);
    }
}

void MainWindow::open()
{
    ui->filePath->setText(QFileDialog::getOpenFileName(this,"Open file","",".Nc files (*.nc)"));
    if(ui->filePath->text()!="")
    {
        ui->Begin->setEnabled(true);
        ui->Stop->setEnabled(true);
    }
    else
    {
        ui->Begin->setEnabled(false);
        ui->Stop->setEnabled(false);
    }
}

void MainWindow::begin()
{
    ui->listWidget->clear();
    readthread.goHome=ui->returnHome->QAbstractButton::isChecked();
    readthread.path=ui->filePath->text();
    readthread.cport_nr=ui->cmbPort->currentIndex();
    readthread.start();
}

void MainWindow::stop()
{
    ui->centralWidget->setStatusTip("Process stopped.");
    emit Stop();
}

int MainWindow::SendJog(float X, float Y, float Z)
{
    /*QString strline;
    char line[35];
    int i;
    strline="G00 X";
    strline.append(QString::number(X,'g',6));
    strline.append(" Y");
    strline.append(QString::number(Y,'g',6));
    strline.append(" Z");
    strline.append(QString::number(Z,'g',6));
    for(i=0;i<strline.length();i++)
    {
        line[i]=strline.at(i).toAscii();
    }
    line[i++]='\n';
    if(SendGcode(line,i))
        return(1);
    else
        return (0);*/
    QString strline;
    char line[35];
    int i;
    strline="G00 X";
    strline.append(QString::number(X,'g',6));
    strline.append(" Y");
    strline.append(QString::number(Y,'g',6));
    strline.append(" Z");
    strline.append(QString::number(Z,'g',6));
    for(i=0;i<strline.length();i++)
    {
        line[i]=strline.at(i).toAscii();
    }
    line[i++]='\n';
#ifndef DISCONNECTED
    {
    if(SendGcode(line,i))
        return(1);
    else
        return (0);
    }
#else
    return(1);
#endif
}

int MainWindow::SendGcode(char* line, int length)
{
    RS232 port = RS232();
    int cport_nr = ui->cmbPort->currentIndex();
    int i, n=0;
    char buf[50];
    for(i=0;i<50;i++)
        buf[i]=0;
    if(port.OpenComport(cport_nr))
    {
        return(0);
    }
    else
    {
#ifdef Q_WS_WIN32
        buf[0]='\n';
        port.SendBuf(cport_nr,buf,1);
        buf[0]=0;
#endif
        while(n==0)
        {
            //usleep(100000);  // sleep for 100 milliSeconds
            n = port.PollComport(cport_nr, buf, 50);
#ifdef Q_WS_X11
            usleep(100000);  // sleep for 100 milliSeconds
#else
            Sleep(100);
#endif
        }
        //usleep(150000);
        port.SendBuf(cport_nr,line,length);
        //->usleep(50000);
        n = port.PollComport(cport_nr, buf, 50);
#ifdef DEBUG
        if(n > 0)
        {
            buf[n] = 0;   // always put a "null" at the end of a string!
            for(i=0; i < n; i++)
            {
                if(buf[i] < 32)  // replace unreadable control-codes by dots
                {
                    buf[i] = '.';
                }
            }
            printf("received %i bytes: %s\n", n, (char *)buf);
            printf("%s\n",(char *)buf);
        }
        else
        {
            printf("received %i bytes.\n", n);
        }
#endif
        //->usleep(50000);  // sleep for 100 milliSeconds
        //while((n==0)||(port.find_txt(buf)==0))
        while (n==0)
        {
            n = port.PollComport(cport_nr, buf, 50);
        }
        port.CloseComport(cport_nr);
    }
    return(1);
}

void MainWindow::UpdateAxis(QString code)
{
    QStringList list;
    code = code.trimmed();
    code.toUpper();
    if(code.indexOf(QRegExp("[XYZ]"))!=-1)
    {
        list = code.split(QRegExp("\\s+"),QString::SkipEmptyParts);
        QString s;
        foreach(s,list)
        {
            if(s.indexOf('X')!=-1)
            {
                ui->lcdNumberX->display(s.mid(1,-1).toFloat());
            }
            else if(s.indexOf('Y')!=-1)
            {
                ui->lcdNumberY->display(s.mid(1,-1).toFloat());
            }
            else if(s.indexOf('Z')!=-1)
            {
                ui->lcdNumberZ->display(s.mid(1,-1).toFloat());
            }
            else
            {}
        }
    }
}

void MainWindow::receiveMsg(QString msg)
{
    ui->centralWidget->setStatusTip(msg);
}

void MainWindow::receiveList(QString msg)
{
    ui->listWidget->addItem(msg.trimmed());
    if(ui->listWidget->count()>12)
    {
        delete ui->listWidget->item(0);
    }
    MainWindow::repaint();
}

void MainWindow::receiveAxis(QString axis)
{
    axis.replace(QRegExp("\\s+")," ");
    UpdateAxis(axis.trimmed());
    ui->listWidget->addItem(axis.trimmed());
    if(ui->listWidget->count()>12)
    {
        delete ui->listWidget->item(0);
    }
    MainWindow::repaint();
}
