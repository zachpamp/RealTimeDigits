#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMediaService>
#include <QMediaRecorder>
#include <QCameraViewfinder>
#include <QCameraInfo>
#include <QMediaMetaData>
#include <QMessageBox>
#include <QPalette>
#include <QtWidgets>
#include <QCamera>
#include <QCameraImageCapture>
#include <QGraphicsVideoItem>
#include <iostream>
#include <stdio.h>
#include <QAbstractVideoSurface>
#include <QCameraViewfinderSettingsControl>
#include <unistd.h>
using namespace std;

string GetData(FILE* file)
{
    char line[200]="";;
    std::string result="";
    while (fgets(line, 200, file))
    {
        result+= line;
    }
    return result;

}

string ExecuteCommand(string command)
{
    FILE* file = popen(command.c_str(), "r");
    string response=GetData(file);
    pclose(file);
    return response;
}






void messagebox(string title, string message)
{

    #ifdef linux
        char command[4000]="";
        sprintf(command,"notify-send '%s' '%s'",title.c_str(), message.c_str());
        FILE* file= popen(command, "r");
        pclose(file);
    #else _WIN32
        ::MessageBox( title.c_str(), string message.c_str(),MB_OK);
    #endif
}


bool checkCameraAvailability()
{
    if (QCameraInfo::availableCameras().count() > 0)
        return true;
    else
        return false;
}

QTimer qTimer;
QCameraImageCapture *imageCapture=NULL;
QCamera *camera=NULL;

void CaptureImage(QString path=NULL)
{
    //on half pressed shutter button
    camera->searchAndLock();
    //on shutter button pressed
    imageCapture->capture();
    //on shutter button released
    camera->unlock();
}

string DigitResponseMock()
{
  string tmp="";
  tmp+="Processed 1/1 images ...\n";
  tmp+="Classification took 0.00332999229431 seconds.\n";
  tmp+="------------------------ Prediction for /tmp/picture.jpeg ------------------------\n";
  tmp+="94.1835% - \"frog\"\n";
  tmp+=" 5.8165% - \"ship\"\n";
  tmp+=" 0.0000% - \"automobile\"\n";
  tmp+=" 0.0000% - \"bird\"\n";
  tmp+=" 0.0000% - \"cat\"\n";
  tmp+="\n";
  tmp+="Script took 0.272104024887 seconds.\n";
  return tmp;
}

class MyVideoSurface : public QAbstractVideoSurface
{
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const
    {
        Q_UNUSED(handleType);

        // Return the formats you will support
        return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_BGR32;
    }

    QImage::Format m_imageFormat = QImage::Format_RGB32;

    QImage currentFrame;

    bool present(const QVideoFrame &frame)
    {
        Q_UNUSED(frame);
        // Handle the frame and do your processing
        QVideoFrame newFrame(frame);
        newFrame.map(QAbstractVideoBuffer::ReadOnly);
        currentFrame = QImage(newFrame.bits(),newFrame.width(),newFrame.height(),newFrame.bytesPerLine(),m_imageFormat);
        qDebug()<<currentFrame.size()<<" "<<newFrame.bits()<<" "<<newFrame.width()<<" "<<newFrame.height()<<
                      " "<<newFrame.bytesPerLine()<<" "<<m_imageFormat;
        //QSize(1600, 1200)   0x7f44b0763010   1600   1200   6400   4
        currentFrame.save("/tmp/picture.jpg");
        string rsp="";
        #ifdef linux
            //remove( "/tmp/picture.jpg" );
            currentFrame.save("/tmp/picture.jpg");// /dev/shm/picture.jpg
            //QString command = ui->?????->???->QLineEdit->toPlainText();// TO DO 1:
            QString command="";
            rsp=ExecuteCommand(command.toStdString().c_str());
            qDebug()<<rsp.c_str();

            //TO DO 2:
            //Parse prosentvalue1,Category1,prosentvalue1,Category1, ... pairs from the response string and add them to std::list

            //TO DO 3;

            //Get highest (prosentvalue,category) pair from std::list and draw the gategory name to currentFrame

            //TO DO 4;
            //write currentFrame to QFrame "dialog" box





        #elif _WIN32
           // remove( "c:/temp/picture.jpg" );
            currentFrame.save("c:/temp/picture.jpg");
            rsp=DigitResponseMock();//Digits not support windows yet
            qDebug()<<rsp.c_str();

        #else
              std::cout<<"Unknown OS";
        #endif



        newFrame.unmap();

        //qDebug()<<"Video frame unmapped";


        //CaptureImage("/tmp/picture.jpg");


        return true;
    }
}mySurface;




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    if(checkCameraAvailability()==true)
    {

        QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        foreach (const QCameraInfo &cameraInfo, cameras)
        {
            //if (cameraInfo.deviceName()!=NULL)
            //{
               std::cout<<"Camera name "<<cameraInfo.deviceName().toStdString();
            //}
        }
        camera = new QCamera();
        imageCapture = new QCameraImageCapture(camera);
        camera->setCaptureMode(QCamera::CaptureStillImage);
        camera->setViewfinder(&mySurface);
        camera->start();
    }




    //qTimer = new QTimer(this);
    //qTimer->setInterval(25);//25 ms timer
    //connect(qTimer, SIGNAL(timeout()), this, SLOT(displayFrame()));

}

MainWindow::~MainWindow()
{
    delete ui;
}






/*
void MainWindow::displayFrame()
{
    //capture a frame from the webcam
   // frame = captureFrame(640, 360);
   // image = getQImageFromFrame(frame);

    //set the image of the label to be the captured frame and resize the label appropriately
    //ui->label->setPixmap(QPixmap::fromImage(image));
    //ui->label->resize(ui->label->pixmap()->size());
}*/
