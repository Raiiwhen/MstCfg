#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QKeyEvent>
#include <qmenu.h>
#include <qaction.h>
#include <qmenubar.h>
#include <QVBoxLayout>
#include <QTime>
#include <QTimer>
#include <QImage>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LOG("start");
    /* paramter default */
    pos_x = 0;
    pos_y = 0;
    pos_z = 0;
    pos_h = 0;
    vel_w = 0;
    step_val = 20;
    isShutdown = false;

    /*camera and cv proccess*/
    cam_hy500 = new dev_usbcam(this);
    cam_hy500->isRunning = false;
    cam_hy500->enableLOG = false;
    connect(cam_hy500,  SIGNAL(img_rdy(int)),       this,   SLOT(update_img(int))       );
    connect(cam_hy500,  SIGNAL(cam_log(QString)),   this,   SLOT(echo_log(QString))     );

    /*spf view serial port*/
    ctrler_spf = new dev_ctler(this);
    connect(cam_hy500,  SIGNAL(cv_cmd(int)),            this,       SLOT(echo_CV_cmd(int))          );
    connect(ctrler_spf, SIGNAL(ui_log(QString)),        this,       SLOT(echo_log(QString))         );
    connect(ctrler_spf, SIGNAL(ui_update()),            this,       SLOT(ui_update())               );
    connect(ctrler_spf, SIGNAL(ui_Hex(QString,int)),    this,       SLOT(echo_viewHex(QString,int)) );
    connect(this,       SIGNAL(ctrl_set(int,uint32_t)), ctrler_spf, SLOT(get_para(int,uint32_t))    );
    connect(this,       SIGNAL(ctrl_en(bool)),          ctrler_spf, SLOT(echo_en_board(bool))       );
    connect(this,       SIGNAL(ctrl_rst()),             ctrler_spf, SLOT(echo_ctrl_rst())           );
    connect(this,       SIGNAL(ctrl_light(uint8_t)),   ctrler_spf, SLOT(echo_light(uint8_t))      );
    ctrler_spf->start();

    /*ui*/
    ui_init();
    ui_update();

    echo_check_step(3);
    on_Bt_CAM_cmd_clicked();
    LOG("ui Init Done");
}


MainWindow::~MainWindow()
{

    emit ctrl_en(false);
    LOG("device shutdown");
    delete ui;
}




void MainWindow::on_Bt_Connect_clicked(){
//    openCom(ui->Cbox_Serial->currentIndex());
}




void MainWindow::on_Cbox_Camera_activated(int index){
//    //更新combox控件
//    ui->Cbox_Camera->clear();
//    initCamera();
//    ui->Cbox_Camera->setCurrentIndex(index);
//    //重启相机框
//    dev_cam->stop();
//    switchCamera(index);
}

void MainWindow::on_Cbox_Serial_activated(int index)
{
    ui->Cbox_Serial->setCurrentIndex(index);
//    openCom(index);

}

void MainWindow::echo_whl(void){
    if(vel_w > 0){
        vel_w = 0;
        ui->Bt_Grab->setText("启动磨轮");
    }else{
        vel_w = 3000;
        ui->Bt_Grab->setText("关闭磨轮");
    }
    emit ctrl_set(4, vel_w);
    ui_update();
}

void MainWindow::echo_CV_cmd(int delta_stepper){
    uint32_t val = (uint32_t)abs(delta_stepper);
    if(val > 0){
        pos_z += val;
    }else{
        if(pos_z>val){
            pos_z-=val;
        }else{
            pos_z = 0;
        }
    }
//    emit ctrl_set(2, pos_z);
    ui_update();
}

void MainWindow::echo_X_up(void){
    pos_x += step_val;
    emit ctrl_set(0, pos_x);
    ui_update();
}
void MainWindow::echo_X_down(void){
    if(pos_x > step_val){
        pos_x -= step_val;
    }else{
        pos_x = 0;
    }
    emit ctrl_set(0, pos_x);
    ui_update();
}
void MainWindow::echo_Y_up(void){
    pos_y += step_val;
    emit ctrl_set(1, pos_y);

    ui_update();
}
void MainWindow::echo_Y_down(void){
    if(pos_y > step_val){
        pos_y -= step_val;
    }else{
        pos_y = 0;
    }

    emit ctrl_set(1, pos_y);
    ui_update();
}
void MainWindow::echo_Z_up(void){
    pos_z += step_val;

    emit ctrl_set(2, pos_z);
    ui_update();
}
void MainWindow::echo_Z_down(void){
    if(pos_z > step_val){
        pos_z -= step_val;
    }else{
        pos_z = 0;
    }

    emit ctrl_set(2, pos_z);
    ui_update();
}
void MainWindow::echo_H_up(void){
    pos_h += step_val;

    emit ctrl_set(3, pos_h);
    ui_update();
}
void MainWindow::echo_H_down(void){
    if(pos_h>step_val){
        pos_h -= step_val;
    }else{
        pos_h = 0;
    }

    emit ctrl_set(3, pos_h);
    ui_update();
}

void MainWindow::ui_update(){
    ui->Lb_X->setText("镜头X: "   + QString::number(ctrler_spf->xyz_x->cmd.pos) + " / " + QString::number(ctrler_spf->xyz_x->sta.curr_pos));
    ui->Lb_Y->setText("镜头Y: "   + QString::number(ctrler_spf->xyz_y->cmd.pos) + " / " + QString::number(ctrler_spf->xyz_y->sta.curr_pos));
    ui->Lb_Z->setText("镜头高度: " + QString::number(ctrler_spf->xyz_z->cmd.pos) + " / " + QString::number(ctrler_spf->xyz_z->sta.curr_pos));
    ui->Lb_H->setText("磨轮高度: " + QString::number(ctrler_spf->whl_h->cmd.pos) + " / " + QString::number(ctrler_spf->whl_h->sta.curr_pos));
    ui->Lb_w->setText("磨轮转速: " + QString::number(ctrler_spf->whl_w->cmd.vel) + " / " + QString::number(ctrler_spf->whl_w->sta.vel)     );

//    ui->Lb_X->setText("镜头X: "   + QString::number(ctrler_spf->xyz_x->cmd.pos));
//    ui->Lb_Y->setText("镜头Y: "   + QString::number(ctrler_spf->xyz_y->cmd.pos));
//    ui->Lb_Z->setText("镜头高度: " + QString::number(ctrler_spf->xyz_z->cmd.pos));
//    ui->Lb_H->setText("磨轮高度: " + QString::number(ctrler_spf->whl_h->cmd.pos));
//    ui->Lb_w->setText("磨轮转速: " + QString::number(ctrler_spf->whl_w->cmd.pos));
}

void MainWindow::ui_init(){
    //初始化窗体
    setFixedSize(1920,1010);
    this->move(0,0);
    setWindowTitle("光纤研磨机控制软件 - TJStudio");
    ui->Bt_dirUp->setAutoRepeat(true);
    ui->Bt_dirUp->setAutoRepeatDelay(400);
    ui->Bt_dirUp->setAutoRepeatInterval(200);
    ui->Bt_dirDown->setAutoRepeat(true);
    ui->Bt_dirDown->setAutoRepeatDelay(400);
    ui->Bt_dirDown->setAutoRepeatInterval(200);
    ui->Bt_dirLeft->setAutoRepeat(true);
    ui->Bt_dirLeft->setAutoRepeatDelay(400);
    ui->Bt_dirLeft->setAutoRepeatInterval(200);
    ui->Bt_dirRight->setAutoRepeat(true);
    ui->Bt_dirRight->setAutoRepeatDelay(400);
    ui->Bt_dirRight->setAutoRepeatInterval(200);

    //按键捕捉esc
    this->grabKeyboard();
}

void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Escape)        {   qApp->quit();  }
//    else if(event->key()==Qt::Key_Up)       {   echo_X_up();   }
//    else if(event->key()==Qt::Key_Down)     {   echo_X_down(); }
//    else if(event->key()==Qt::Key_Right)    {   echo_Y_up();   }
//    else if(event->key()==Qt::Key_Left)     {   echo_Y_down(); }
//    else if(event->key()==Qt::Key_Greater)  {   echo_Z_up();   }
//    else if(event->key()==Qt::Key_Less)     {   echo_Z_down(); }
//    else if(event->key()==Qt::Key_W)        {   echo_H_down(); }
//    else if(event->key()==Qt::Key_S)        {   echo_H_down(); }
//    else if(event->key()==Qt::Key_Enter)    {   echo_whl();    }
//    else if(event->key()==Qt::Key_Return)   {   echo_whl();    }
}

void MainWindow::on_Bt_dirForward_clicked() {   echo_X_down();  } // dirUp
void MainWindow::on_Bt_dirBackward_clicked(){   echo_X_up();    } // dirDown
void MainWindow::on_Bt_dirUp_clicked()      {   echo_Z_up();  } //    >
void MainWindow::on_Bt_dirDown_clicked()    {   echo_Z_down();    } //    <
void MainWindow::on_Bt_dirRight_clicked()   {   echo_Y_up();    } // dirRight
void MainWindow::on_Bt_dirLeft_clicked()    {   echo_Y_down();  } // dirLeft
void MainWindow::on_Bt_Forward_clicked()    {   echo_H_down();  } // W
void MainWindow::on_Bt_Backward_clicked()   {   echo_H_up();    } // S
void MainWindow::on_Bt_Grab_clicked()       {   echo_whl();     } // Enter

void MainWindow::on_Bt_CAM_cmd_clicked(){
    if(cam_hy500->isRunning){
        cam_hy500->stopGrab();
        ui->Bt_CAM_cmd->setText("Start\ndev_cam");
    }else{
        cam_hy500->start();
        ui->Bt_CAM_cmd->setText("Stop\ndev_cam");
    }
}

void MainWindow::view(QImage img){
    QPixmap img_pixmap = QPixmap::fromImage(img);
    img_pixmap.scaled(ui->Lb_Camera_Capture->size(),Qt::KeepAspectRatio);
    ui->Lb_Camera_Capture->setScaledContents(true);
    ui->Lb_Camera_Capture->setPixmap(img_pixmap);
}

void MainWindow::update_img(int i){
    view(cam_hy500->getImg());
}

void MainWindow::LOG(QString str){
    str = "MainWindow> " + str;

    qDebug() << str;
    ui->statusBar->showMessage(str);
}

void MainWindow::echo_log(QString str){
    str = "SIGNAL> " + str;

    qDebug() << str;
    ui->statusBar->showMessage(str);
}

void MainWindow::echo_viewHex(QString echo_log, int index){
    QDateTime msg_time = QDateTime::currentDateTime();
    QString msg_time_str = msg_time.toString("yyyy.MM.dd hh:mm:ss.zzz ddd");
    QString txVSrx = " TX " + QString::number(ctrler_spf->tx_cnt) + " | RX " + QString::number(ctrler_spf->rx_cnt);
    if(index == 1 ){
        echo_log = echo_log + ", Rx at " + msg_time_str;
        ui->Lb_Rx->setText(echo_log);
    }else{
        echo_log = echo_log + ", Tx at " + msg_time_str;
        ui->Lb_Tx->setText(echo_log);
    }
    ui->Lb_QoS->setText(txVSrx);
}


void MainWindow::on_Bt_Ctrl_Cmd_clicked()
{
    emit ctrl_light(80);
}

void MainWindow::echo_check_step(int index){
    ui->rd_20um->setChecked(false);
    ui->rd_100um->setChecked(false);
    ui->rd_1mm->setChecked(false);
    ui->rd_1cm->setChecked(false);
    switch (index) {
        case 1:step_val = 20;          ui->rd_20um->setChecked(true); break;
        case 2:step_val = 100;         ui->rd_100um->setChecked(true);break;
        case 3:step_val = 1000;        ui->rd_1mm->setChecked(true);  break;
        case 4:step_val = 1000000;     ui->rd_1cm->setChecked(true);  break;
        default:step_val = 0;
    }
}

void MainWindow::on_rd_20um_clicked()   {    echo_check_step(1);    }
void MainWindow::on_rd_100um_clicked()  {    echo_check_step(2);    }
void MainWindow::on_rd_1mm_clicked()    {    echo_check_step(3);    }
void MainWindow::on_rd_1cm_clicked()    {    echo_check_step(4);    }
void MainWindow::on_Bt_Push_clicked()   {    ctrler_spf->start();   }

void MainWindow::on_Bt_Ctrl_shutdown_clicked()
{
    if(isShutdown){
        isShutdown = false;
        ui->Bt_Ctrl_shutdown->setStyleSheet("color: black");
        ui->Bt_Ctrl_shutdown->setText("控制板开机");
    }else{
        isShutdown = true;
        ui->Bt_Ctrl_shutdown->setStyleSheet("color: red");
        ui->Bt_Ctrl_shutdown->setText("控制板关机");
    }
    emit ctrl_en(isShutdown);
}

void MainWindow::on_Bt_Ctrl_rst_clicked()
{
    pos_x = 0;
    pos_y = 0;
    pos_z = 0;
    pos_h = 0;
    vel_w = 0;

    emit ctrl_rst();
    ui_update();
    LOG("设备复位");
}
