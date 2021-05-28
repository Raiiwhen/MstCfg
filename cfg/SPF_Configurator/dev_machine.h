#ifndef DEV_MACHINE_H
#define DEV_MACHINE_H

#include <qserialport.h>
#include <qserialportinfo.h>
#include <QThread>


typedef enum{
    shake,
    stepper,
    status,
    ack,
    tube,
    rst,
    shutdown
}cfg_type;

typedef struct{
    uint32_t id;
    uint32_t status;
    int32_t  vel;
    uint32_t pos;
    uint32_t curr_threshold;
}cfg_bodyStepper;

typedef struct{
    uint32_t ifCalied;
    uint32_t curr_pos;
    int32_t  vel;
//    uint32_t pos;
    uint32_t curr;
}cfg_bodyStatus;

typedef struct{
    uint8_t head[8];
    uint32_t type;
    uint8_t body[112];
    uint32_t CCR;
}cfg_pack;

class dev_stepper{
public:
    dev_stepper();
    dev_stepper(uint32_t id, int32_t speed, uint32_t target_pos, uint32_t curr_thresold);

public:/* set from ui , Tx to ctrler board */
    uint32_t id;
    double ifLock;
    cfg_bodyStepper cmd;
    cfg_bodyStatus sta;
};

class dev_ctler : public QThread{
    Q_OBJECT
public:
    dev_ctler(QObject* parent);
    ~dev_ctler();
    QString port_name;
    bool isRunning;
    bool enableLOG;
    void run();
    dev_stepper* xyz_x;
    dev_stepper* xyz_y;
    dev_stepper* xyz_z;
    dev_stepper* whl_h;
    dev_stepper* whl_w;
    bool cmdStepper(dev_stepper *obj, bool ifArm);
    bool cmdStepper(dev_stepper *obj, uint32_t target_pos);
    bool cmdStepper(dev_stepper *obj, uint32_t target_pos, int32_t vel);
    bool cmdStepper(dev_stepper* obj, int32_t speed, uint32_t target_pos, uint32_t curr_thresold, bool ifLock);
    bool cmdRST();
    bool cmdShutdown();
    bool cmdLight(uint8_t lium);
    int tx_cnt;
    int rx_cnt;

signals:
    void ui_log(QString log);
    void ui_update();
    void ui_Hex(QString hex, int index);

public slots:
    void rxCOM();
    void get_para(int index, uint32_t para);
    void echo_en_board(bool isPwrOn);
    void echo_ctrl_rst();
    void echo_light(uint8_t lium);

private:
    QList<QSerialPortInfo> COM_info_list;
    QSerialPortInfo COM_info;
    QSerialPort* COM;
    bool initCOM(int index);
    void LOG(QString log);
    bool txCOM(uint8_t* src, int len);
    void txPack(cfg_type type, uint8_t* body, int len);
    QString toStr(uint8_t* pSrc, int len);
    QString toStr(char* pSrc, int len);
    void staStepper(cfg_pack* pPack);
    char RX_BUFF[256];
    QByteArray rx_buffer_pool;
};


#endif // DEV_MACHINE_H
