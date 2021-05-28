#include "dev_usbcam.h"
#include <QTime>

/** Foci plane quality check function**/
/** This Function tells you the distance to the foci plane**/
using namespace cv;
using namespace std;
double dev_usbcam::cv_focu_adj(Mat IMG, double cv_sobel_pre){


}


dev_usbcam::dev_usbcam(QObject* parent){
    QCameraInfo dev_info = QCameraInfo::defaultCamera();

    name = dev_info.description();
    dev_cam = new QCamera(dev_info);
    cam_imgCapture = new QCameraImageCapture(dev_cam);
    connect(cam_imgCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(img_grabbed(int,QImage)));

    enableLOG = true;
    rstCam();
    enableLOG = false;

}

dev_usbcam::~dev_usbcam(){
    LOG("DeInit dev_cam");
}

void dev_usbcam::stopGrab(){
    isRunning = false;
}

void dev_usbcam::run(){
    LOG("dev_usbcam Running.");

    /*reset state*/
    grabbing = false;
    isRunning = true;
    double timeout = 0;

    /*init device*/
    if(rstCam() == false)return;

    /*start running*/
    LOG("Start continous grabbing...");
    while(isRunning){
        if(grabbing == false){
            timeout = 0;
            timer.start();
            cam_imgCapture->capture();
            grabbing = true;
        }
        timeout += 20;
        if(timeout > 2000)break;
        msleep(20);
    }

    stop();
}

void dev_usbcam::stop(){
    double timeout;
    /*end running*/
    timeout = 0;
    isRunning = false;
    while(grabbing){
        msleep(20);
        if(timeout++>10){
            LOG("Continous grab stop timeout");
        }else{
            LOG("Continous grab stopped.");
        }
    }
    timer.elapsed();
}

bool dev_usbcam::rstCam(){

    dev_cam->setCaptureMode(QCamera::CaptureStillImage);
    dev_cam->start();
    cam_imgCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    double timeout = 0;
    while (cam_imgCapture->isReadyForCapture() == false){
        msleep(10);
        if(timeout++>10){
            LOG("camera rst timeout");
            return  false;
        }
    }

    if(dev_cam->status() == QCamera::Status::ActiveStatus){
        LOG("camera ready to go.");
        return true;
    }else{
        LOG("camera rst failed. exit with code " + QString::number(dev_cam->status()));
        return false;
    }
}

QImage dev_usbcam::grabSingle(){
    QImage img = QImage("C:/Users/Administrator/Pictures/Camera Roll/WIN_20210515_16_01_16_Pro.jpg");
    LOG("grab single");
    dev_cam->setCaptureMode(QCamera::CaptureStillImage);
    while (cam_imgCapture->isReadyForCapture() == false){}
    cam_imgCapture->capture();

    return img;
}

QImage dev_usbcam::getImg(){ return dst;}

void dev_usbcam::LOG(double num){    LOG(QString::number(num));}
void dev_usbcam::LOG(QString str){
    if(enableLOG){
        QString log = "dev_usbcam > " +  str;
        qDebug() << log;
        emit cam_log(log);
    }
}

void dev_usbcam :: img_grabbed(int id, QImage image){
    QString str /*= QString::number(id)*/;
    str = " grabbed done \t[" + QString::number(timer.elapsed()) + " ms] ";

    dst = image;
    cv_src = qimg2mat(dst);

    emit img_rdy(id);
    LOG(str);

    int delta_stepper = 0;

    /*running cv process start from [ Mat cv_src ]*/
    if(false){
        Mat IMG;
        cv_src.copyTo(IMG);
        double cv_focu_cur = 0;

        Mat IMG_Grey;
        Mat IMG_Sobel;
        cvtColor(IMG,IMG_Grey,1);//
        Sobel(IMG_Grey, IMG_Sobel, CV_16U, 1, 1);
        double mean_val = mean(IMG_Sobel)[0]; // the mean value of the image. When this reaches max, we arrive at foci plane

        cv_focu_cur = mean_val;



        delta_stepper = (int)cv_focu_cur;

        emit cv_cmd(delta_stepper);
        str += "\t, cv output ";
        str += QString::number(delta_stepper);
        LOG(str);
    }
    /*running cv process done output [ int delta_stepper ]*/


    grabbing = false;
}

Mat dev_usbcam::qimg2mat(QImage src){
    Mat dst;

    LOG(src.format());

    return dst;
}

void dev_usbcam :: img_saved(){
    LOG("img saved.");
}
