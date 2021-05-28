#include "dev_machine.h"
#include "dev_machine.h"
#include <QDebug>

dev_ctler :: dev_ctler(QObject* parent){
    /*init serial communicate*/
    initCOM(2);
    memset(RX_BUFF,0,sizeof (RX_BUFF));

    connect(COM, SIGNAL(readyRead()), this, SLOT(rxCOM()));
// init : [uint32_t id], [int32_t speed], [uint32_t target_pos], [uint32_t curr_thresold]
    xyz_x = new dev_stepper(2,7500,0,3000);
    xyz_y = new dev_stepper(1,7500,0,3000);
    xyz_z = new dev_stepper(0,7500,0,3000);
    whl_h = new dev_stepper(3,7500,0,3000);
    whl_w = new dev_stepper(4,0,0,3000);

    enableLOG = true;
    tx_cnt = 0;
    rx_cnt = 0;
    //rstCtler();
    LOG("Ctrler at " + port_name + " ready to go");

}

dev_ctler :: ~dev_ctler(){
    LOG("Ctrler deInit out");
}

dev_stepper :: dev_stepper(){
    /* set by cfg*/
    this->id = 0xffffffff;
    this->cmd.id = this->id;
    this->cmd.vel = -1;
    this->cmd.status = 0;
    this->cmd.pos = 0;
    this->cmd.curr_threshold = 0;
    this->ifLock = true;
    /* read from slave*/
    this->sta.vel = -1;
    this->sta.ifCalied = 0;
    this->sta.curr = 0;
    this->sta.curr_pos = 0;
//    this->sta.pos = 0;
}

dev_stepper :: dev_stepper(uint32_t id, int32_t speed, uint32_t target_pos, uint32_t curr_thresold){
    /* set by cfg*/
    this->id = id;
    this->cmd.id = this->id;
    this->cmd.vel = speed;
    this->cmd.status = 0;
    this->cmd.pos = target_pos;
    this->cmd.curr_threshold = curr_thresold;
    this->ifLock = true;
    /* read from slave*/
    this->sta.vel = -1;
    this->sta.ifCalied = 0;
    this->sta.curr = 0;
    this->sta.curr_pos = 0;
//    this->sta.pos = 0;
}


void dev_ctler :: run(){
//    ctrler_spf->cmdStepper(ctrler_spf->xyz_y,pos_y);
//    ctrler_spf->cmdStepper(ctrler_spf->xyz_z,pos_z);
//    ctrler_spf->cmdStepper(ctrler_spf->whl_w,0x7fffffff, vel_w);
//    ctrler_spf->cmdStepper(ctrler_spf->whl_h,pos_h);

}

void dev_ctler::get_para(int index, uint32_t para){
    switch (index) {
        case 0: cmdStepper(xyz_x,para);break;
        case 1: cmdStepper(xyz_y,para);break;
        case 2: cmdStepper(xyz_z,para);break;
        case 3: cmdStepper(whl_h,para);break;
        case 4: cmdStepper(whl_w,para);break;
    }

    LOG("steper cmd > " + QString::number(index) + " / " + QString::number(para));
}

void dev_ctler :: echo_en_board(bool isPwrOn){
    if(isPwrOn){
        cmdRST();
        LOG("board start running");
    }else{
        cmdShutdown();
        LOG("board shutdown");
    }
}

void dev_ctler :: echo_ctrl_rst(){
    cmdRST();
}

void dev_ctler :: echo_light(uint8_t lium){
    cmdLight(lium);
}

void dev_ctler :: rxCOM(){

    rx_buffer_pool += COM->readAll();

    if(rx_buffer_pool.count()>128){
        /* start read COM data */
        memset(RX_BUFF,0,128);
        memcpy(RX_BUFF, rx_buffer_pool.data(), 128);
        rx_buffer_pool.remove(0,128);

        /* read done start proccess */
        if(memcmp(RX_BUFF, "TJCFGSPF", 8) == 0){//checked
            staStepper((cfg_pack*)RX_BUFF);
            LOG("Pack Checked.");
        }else{
            rx_buffer_pool.clear();
            LOG("Pack wrong.");
        }
        rx_cnt++;

        emit ui_Hex(toStr(RX_BUFF,128), 1);
    }
}

bool dev_ctler :: cmdShutdown(){
    uint8_t body[112];
    memset(body,0xff,sizeof (body));
    txPack(cfg_type::shutdown, body, sizeof(body));

    LOG("Ctrler RST");

    return true;
}

bool dev_ctler :: cmdLight(uint8_t lium){
    uint8_t body[112];
    memset(body,0xff,sizeof (body));
    body[0] = lium;

    LOG("set light to " + QString::number(lium));

    return true;
}

bool dev_ctler :: cmdRST(){
    uint8_t rstbuf[112];
    memset(rstbuf,0xff,sizeof (rstbuf));
    txPack(cfg_type::rst, rstbuf, sizeof(rstbuf));

    LOG("Ctrler RST");

    return true;
}

bool dev_ctler :: cmdStepper(dev_stepper *obj, bool ifArm){
    obj->cmd.status = (ifArm?1:2);
    uint8_t* pCmd = (uint8_t*)&obj->cmd;

    txPack(cfg_type::stepper, pCmd, sizeof(cfg_bodyStepper));

    return true;
}
bool dev_ctler :: cmdStepper(dev_stepper *obj, uint32_t target_pos){
    obj->cmd.pos = target_pos;
    uint8_t* pCmd = (uint8_t*)&obj->cmd;

    txPack(cfg_type::stepper, pCmd, sizeof(cfg_bodyStepper));

    return true;
}

bool dev_ctler :: cmdStepper(dev_stepper *obj, uint32_t target_pos, int32_t vel){
    obj->cmd.pos = target_pos;
    obj->cmd.vel = vel;
    uint8_t* pCmd = (uint8_t*)&obj->cmd;

    txPack(cfg_type::stepper, pCmd, sizeof(cfg_bodyStepper));

    return true;
}

bool dev_ctler :: cmdStepper(dev_stepper* obj, int32_t speed, uint32_t target_pos, uint32_t curr_thresold, bool ifLock){
    obj->cmd.vel =speed;
    obj->cmd.pos = target_pos;
    obj->cmd.curr_threshold = curr_thresold;
    if(ifLock == false)obj->cmd.status = 4;
    uint8_t* pCmd = (uint8_t*)&obj->cmd;

    txPack(cfg_type::stepper,pCmd, sizeof(cfg_bodyStepper));

    emit ui_update();

    return true;
}

void dev_ctler::staStepper(cfg_pack* pPack){
    uint8_t* pSrc = (uint8_t*)pPack->body;
    memcpy(&xyz_z->sta,pSrc   ,20);
    memcpy(&xyz_y->sta,pSrc+20,20);
    memcpy(&xyz_x->sta,pSrc+40,20);
    memcpy(&whl_h->sta,pSrc+60,20);
    memcpy(&whl_w->sta,pSrc+80,20);
}


bool dev_ctler::initCOM(int index){

    /* update all available port information */
    COM_info_list.clear();
    COM_info_list = QSerialPortInfo::availablePorts();

    /* check parameter */
    if(index > COM_info_list.size()){
        LOG("invalid com choice");
        return false;
    }

    /* update the port we are using */
    COM_info = COM_info_list.at(index);

    /* init serial port paraments*/
    COM = new QSerialPort;
    COM->setBaudRate(QSerialPort::Baud115200);
    COM->setStopBits(QSerialPort::OneStop);
    COM->setDataBits(QSerialPort::Data8);
    COM->setParity(QSerialPort::NoParity);
    COM->setReadBufferSize(1024);
    COM->setPortName(COM_info.portName());
    port_name = COM_info.portName();

    //exception: port has been opened before
    if(COM->isOpen()) COM->close();

    /* now open the port */
    if(COM->open(QIODevice::ReadWrite)){
        LOG(COM->portName() + " open succeed");
        return true;
    }else{
        LOG(COM->portName() + " open falied");
        return false;
    }
}

bool dev_ctler :: txCOM(uint8_t* src, int len){
    if(COM->isOpen()){
        QByteArray str = QByteArray((char*)src,len);
        COM->write(str);
        while (COM->waitForBytesWritten(10)) {

        }
        tx_cnt++;
        emit ui_Hex(toStr(src,len), 2);
        return true;
    }else{
        return false;
    }
}

void dev_ctler :: txPack(cfg_type type, uint8_t* body, int len){
    cfg_pack dst;
    uint8_t* pDst = (uint8_t*)&dst;
    memset(pDst, 0, sizeof (dst));
    memcpy((uint8_t*)&dst.head, "TJCFGSPF", 8);
    switch (type) {
        case cfg_type::shake:   dst.type = 1;   break;
        case cfg_type::stepper: dst.type = 2;   break;
        case cfg_type::rst:     dst.type = 6;   break;
        case cfg_type::shutdown:dst.type = 7;   break;
        default: dst.type = 0;
    }
    memcpy((uint8_t*)&dst.body, body, (size_t)len);

    uint32_t sum = 0;
    for(uint8_t cnt = 0; cnt < sizeof (dst)-4; cnt++){
        sum += *(pDst + cnt);
    }
    dst.CCR = sum;
    txCOM((uint8_t*)&dst,sizeof(dst));
    LOG("TXing.. [" + QString::number(sizeof(dst)) + "]");
}

void dev_ctler::LOG(QString str){
    if(enableLOG){
        QString log = "dev_machine > " +  str;
//        qDebug() << log;
        emit ui_log(log);
    }
}

QString dev_ctler::toStr(uint8_t* pSrc, int len){
    QString dst;
    QString chr = "0x0000";
    for(int cnt=0;cnt<len;cnt++){
        chr = QString("%1").arg(int(*(pSrc+cnt)), 2, 16, QLatin1Char('0'));
        dst += chr.right(2);
        dst += " ";
        if((cnt+1)%4 == 0){
            dst+= ", ";
        }
        if((cnt+1)%32 == 0){
            dst+= "\n";
        }
    }
    return  dst;
}

QString dev_ctler::toStr(char* pSrc, int len){
    QString dst;
    QString chr = "0x0000";
    for(int cnt=0;cnt<len;cnt++){
        chr = QString("%1").arg(int(*(pSrc+cnt)), 2, 16, QLatin1Char('0'));
        dst += chr.right(2);
        dst += " ";
        if((cnt+1)%4 == 0){
            dst+= ", ";
        }
        if((cnt+1)%32 == 0){
            dst+= "\n";
        }
    }
    return  dst;
}
