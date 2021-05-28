#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qdebug.h>

#include "dev_usbcam.h"
#include "dev_machine.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void ctrl_set(int index, uint32_t para);
    void ctrl_en(bool isPowerOn);
    void ctrl_rst();
    void ctrl_light(uint8_t lium);

private slots:
    void on_Bt_Connect_clicked();
    void on_Bt_Grab_clicked();
    void keyPressEvent(QKeyEvent* event);
    void on_Cbox_Camera_activated(int index);
    void on_Cbox_Serial_activated(int index);
    void on_Bt_dirUp_clicked();
    void on_Bt_dirDown_clicked();
    void on_Bt_dirRight_clicked();
    void on_Bt_dirLeft_clicked();
    void on_Bt_CAM_cmd_clicked();
    void on_Bt_dirForward_clicked();
    void on_Bt_Forward_clicked();
    void on_Bt_Backward_clicked();
    void on_Bt_dirBackward_clicked();
    void update_img(int i);
    void echo_log(QString echo_log);
    void echo_CV_cmd(int delta_stepper);
    void on_Bt_Ctrl_Cmd_clicked();
    void ui_update();
    void echo_viewHex(QString hex, int index);
    void on_rd_20um_clicked();
    void on_rd_100um_clicked();
    void on_rd_1mm_clicked();
    void on_rd_1cm_clicked();
    void on_Bt_Push_clicked();

    void on_Bt_Ctrl_shutdown_clicked();

    void on_Bt_Ctrl_rst_clicked();

private:
    Ui::MainWindow *ui;
    dev_usbcam* cam_hy500;
    dev_ctler* ctrler_spf;
    uint32_t pos_x, pos_y, pos_z, pos_h;
    uint32_t vel_w;
    uint32_t step_val;
    bool isShutdown;

    void echo_X_up(void);
    void echo_X_down(void);
    void echo_Y_up(void);
    void echo_Y_down(void);
    void echo_Z_up(void);
    void echo_Z_down(void);
    void echo_H_up(void);
    void echo_H_down(void);
    void echo_whl(void);
    void echo_check_step(int index);

    void ui_init();

    void LOG(QString str);
    void view(QImage img);
};

#endif // MAINWINDOW_H
