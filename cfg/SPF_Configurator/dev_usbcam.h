#ifndef DEV_USBCAM_H
#define DEV_USBCAM_H

#include <QCamera>
#include <QCameraInfo>
#include <qcameraviewfinder.h>
#include <qcameraimagecapture.h>
#include <QImage>
#include <QThread>
#include <QTime>

#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

using namespace cv;

class dev_usbcam : public QThread{
    Q_OBJECT
public:
    dev_usbcam(QObject* parent);
    ~dev_usbcam();
    QString name;
    bool isRunning;
    bool enableLOG;
    void run();
    void stop();
    QImage grabSingle();
    QImage getImg();
    void stopGrab(void);
    double cv_focu_adj(Mat IMG, double cv_focu_pre); // the focus length adjusting function, -x ~ x, 0 means at focu length


signals:
    void img_rdy(int i);
    void cam_log(QString log);
    void cv_cmd(int delta_stepper);

private slots:
    void img_grabbed(int id, QImage image);
    void img_saved();

private:
    double cv_foci; // the distance to focu plane

    QTime timer;
    Mat cv_src;
    bool grabbing;
    bool isSaved;
    bool isGrabbed;
    QCamera* dev_cam;
    QCameraImageCapture *cam_imgCapture;
    QImage dst;
    void LOG(QString str);
    void LOG(double num);
    bool rstCam();
    Mat qimg2mat(QImage src);
};


#endif // DEV_USBCAM_H
