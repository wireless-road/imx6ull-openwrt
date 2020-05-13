#include <QCoreApplication>
#include <QObject>
#include <QSerialPort>
#include <QProcess>
#include <QThread>
#include <QDir>
#include <QDebug>
#include <QTelnet.h>


void usage(){
    qInfo() << "Usage:";
    qInfo() << "   -h | --help                    Show this message and quit.";
    qInfo() << "   -o | --ocd <path-to-exe>       Path to ocd binary. Default, 'openocd.exe'";
    qInfo() << "   -s | --serial <com-port>       Serial port name to use. Mandatory.";
    qInfo() << "   -p | --port <ocd-telnet-port>  Telnet port of ocd to connect. Default, '3000'";
    qInfo() << "                                   Must be the same as in openocd.cfg for 'telnet_port'.";
    qInfo() << "   -u | --uboot <uboot.imx-image> Path to uboot.imx image to run. Default, 'u-boot.imx'";
    qInfo() << "   -f | --fw-factory <firmware>   Path to factory image to flash. If not set, firmware flash skipped.";
    qInfo();
}

bool serial_wait_for_line(QSerialPort *s, QString line, int timeout_msec){
    qint16 counter = 0;
    bool ret = false;
    QString temp;
    while(true){
        if(s->waitForReadyRead(timeout_msec)){
            counter = 0;
            temp.append(QString::fromLocal8Bit(s->readAll()).simplified());
            if(temp.contains(line, Qt::CaseInsensitive)){
                ret = true;
                break;
            }
        }else{
            counter++;
            if(counter == 5)
                break;
        }
    }
    return ret;
}
void serial_send_cmd(QSerialPort *s, QString line, int timeout_msec){
    /* To prevent error on TX write command char by char. */
    for(int i=0; i< line.size();i++){
        s->putChar(line.at(i).toLatin1());
        s->waitForBytesWritten(timeout_msec);
    }
    s->putChar('\n');
    s->waitForBytesWritten(timeout_msec);
}

bool serial_init(QSerialPort *s, QString serial_port){
    s->setPortName(serial_port);
    s->setBaudRate(QSerialPort::Baud115200);
    s->setDataBits(QSerialPort::Data8);
    s->setParity(QSerialPort::NoParity);
    s->setStopBits(QSerialPort::OneStop);
    s->setFlowControl(QSerialPort::NoFlowControl);
    s->setReadBufferSize(65536);

    if(!s->open(QSerialPort::ReadWrite)){
        qWarning() << "Cannot open serial port. Bailing.";
        return false;
    }
    s->setTextModeEnabled(true);
    return true;
}

bool process_wait_for_line(QProcess *p, QString line, int timeout_msec){
    qint16 counter = 0;
    bool ret = false;
    QString temp;
    while(true){
        if(p->waitForReadyRead(timeout_msec)){
            counter=0;
            temp = QString::fromLocal8Bit(p->readAll()).simplified();
            if(temp.contains(line, Qt::CaseInsensitive)){
                ret = true;
                break;
            }
        }else{
            counter++;
            if(counter == 3)
                break;
        }
    }
    return ret;
}

bool process_init(QProcess *p, QString program){
    if(program.endsWith(".bat")){
        p->setProgram("cmd.exe");
        p->setArguments({"/C", program});
    }else{
        p->setProgram(program);
    }
    p->setWorkingDirectory(QDir::currentPath());
    p->setProcessChannelMode(QProcess::MergedChannels);
    p->start();
    if(! p->waitForStarted()){
        qWarning() << "Cannot run OpenOCD. Bailing.";
        return false;
    }
    p->setTextModeEnabled(true);
    return true;
}

void telnet_send_cmd(QTelnet *t, QString cmd){
    t->sendData(cmd.toLatin1());
    t->sendData("\n");
    t->waitForBytesWritten(5000);
}

bool telnet_init(QTelnet *t, quint16 telnet_port){
    t->connectToHost(QString("127.0.0.1"), telnet_port);
    t->waitForConnected(5000);
    if(!t->isConnected()){
        qWarning() << "Cannot connect to OpenOCD with telnet. Bailing.";
        return false;
    }
    return true;
}

void  worker(QString ocd, QString serial_port, quint16 telnet_port, QString uboot, QString fw){
    QSerialPort qserial;
    QTelnet qtelnet;
    QProcess qprocess;
    QString line;
    qint32 counter;

    if(!serial_init(&qserial, serial_port))
        goto exit;

    if(!process_init(&qprocess, ocd))
        goto exit;

    /* Waiting for board to init with JTAG. */
    counter = 0;
    while(true){
        if(qprocess.waitForReadyRead(5000)){
            counter=0;
            line = QString::fromLocal8Bit(qprocess.readAll()).simplified();
            if(line.contains(QString("Info : imx6ull.cpu.0: hardware has 6 breakpoints, 4 watchpoints"), Qt::CaseInsensitive)){
                qInfo() << "   Got JTAG connected and ok signal with openOCD";
                break;
            }
            if(line.contains(QString("Error: No J-Link device found."), Qt::CaseInsensitive)){
                qWarning() << "No J-Link device found error from openOCD. Bailing.";
                goto exit;
            }
        }else{
            counter++;
            if(counter == 3){
                qWarning() << "Error waiting for OpenOCD. Bailing.";
                goto exit;
            }
        }
    }

    /* Trying to connect to OpenOCD with telnet */
    if(!telnet_init(&qtelnet, telnet_port))
        goto exit;

    if(process_wait_for_line(&qprocess, "Info : accepting 'telnet' connection on", 5000)){
        qInfo() << "   OpenOCD telnet connection started.";
    }else{
        qWarning() << "Cannot establish telnet connection. Bailing.";
        goto exit;
    }

    /* PowerOn board and halt execution, so we can load images to RAM. */
    qInfo() << "   Resetting Board.";
    telnet_send_cmd(&qtelnet, "reset init; arm core_state arm; halt;");
    if(process_wait_for_line(&qprocess, "core state: ARM", 5000)){
        qInfo() << "   Board reset complete.";
    }else{
        qWarning() << "Board reset failed. Bailing.";
        goto exit;
    }


    QThread::sleep(1);
    /* Loading images to RAM, UBoot to loadaddr so we can start and FW just to RAM. */
    qInfo() << "   Loading UBoot to RAM. Waiting...";
    line.clear();
    line = "load_image ";
    line.append(uboot);
    line.append(" 0x877ff400");

    telnet_send_cmd(&qtelnet, line);
    if(process_wait_for_line(&qprocess, "downloaded ", 30000)){
        qInfo() << "   UBoot load complete.";
    }else{
        qWarning() << "UBoot load failed. Bailing.";
        goto exit;
    }

    if(!fw.isEmpty()){
        qInfo() << "   Loading factory firmare (FW) to RAM. It may take long time (5-10min). Waiting...";
        QThread::sleep(1);
        line.clear();
        line = "load_image ";
        line.append(fw);
        line.append(" 0x82000000");
        telnet_send_cmd(&qtelnet, line);
        if(process_wait_for_line(&qprocess, "downloaded ", 600000)){
            qInfo() << "   FW load complete.";
        }else{
            qWarning() << "FW load failed. Bailing.";
            goto exit;
        }
    }

    qInfo() << "   Starting UBoot.";
    QThread::sleep(1);
    telnet_send_cmd(&qtelnet, "resume 0x87800000");

    if(serial_wait_for_line(&qserial, "CPU", 5000)){
        serial_send_cmd(&qserial, " ", 1000);
        serial_send_cmd(&qserial, "\n", 1000);
        QThread::sleep(1);
        serial_send_cmd(&qserial, " ", 1000);
        serial_send_cmd(&qserial, "\n", 1000);
        if(serial_wait_for_line(&qserial, "=>", 5000)){
            qInfo() << "   Uboot started and interrupted to cmd line.";
        }else{
            qWarning() << "Cannot catch uboot output on serial. Bailing.";
            goto exit;
        }
    }else{
        if(serial_wait_for_line(&qserial, "Hit any", 5000)){
            serial_send_cmd(&qserial, " ", 1000);
            serial_send_cmd(&qserial, "\n", 1000);
            QThread::sleep(1);
            serial_send_cmd(&qserial, " ", 1000);
            serial_send_cmd(&qserial, "\n", 1000);
            if(serial_wait_for_line(&qserial, "=>", 5000)){
                qInfo() << "   Uboot started and interrupted to cmd line.";
            }else{
                qWarning() << "Cannot catch uboot output on serial. Bailing.";
                goto exit;
            }
        }else{
            qWarning() << "Cannot catch uboot output on serial. Bailing.";
            goto exit;
        }
    }

    if(!fw.isEmpty()){
        qInfo() << "   Flashing Firmware to flash. It may take some time (5-10min). Waiting...";
        serial_send_cmd(&qserial, "sf probe ", 3000);
        if(!serial_wait_for_line(&qserial, "SF: Detected", 5000)){
            qWarning() << "Error on UBoot detecting spi flash. 'sf probe' not returned properly.";
            goto exit;
        }
        QThread::sleep(1);
        serial_send_cmd(&qserial, "sf update 0x82000000 0x0 0x800000 ", 3000);
        if(serial_wait_for_line(&qserial, "bytes written,", 300000)){
            qInfo() << "   FW flashed to spi.";
        }else{
            qWarning() << "Some error occured on flashing factory image. Timeout waiting of finish.";
            goto exit;
        }
    }

    QThread::sleep(1);

    qInfo() << "   Writing FUSE for ECSPI3.";
    serial_send_cmd(&qserial, "fuse prog -y 0 5 0x0a000030 ", 1000);
    if(serial_wait_for_line(&qserial, "Programming bank", 1000)){
        qInfo() << "   ECSPI3 programming ok (0x450 = 0x0a000030)";
    }else{
        qWarning() << "Some error programming fuse for ECSPI3. Bailing.";
        goto exit;
    }

    QThread::sleep(1);
    serial_send_cmd(&qserial, "fuse prog -y 0 6 0x00000010 ", 1000);
    if(serial_wait_for_line(&qserial, "Programming bank", 1000)){
        qInfo() << "   BT_FUSE_SEL programming ok (0x460 = 0x00000010)";
    }else{
        qWarning() << "Some error programming fuse for BT_FUSE_SEL. Bailing.";
        goto exit;
    }

    qInfo() << "Resetting board to load firmware.";
    serial_send_cmd(&qserial, "reset ", 1000);

    qInfo() << "Everything finished ok.";
    /* Kill process */
exit:
    qtelnet.disconnectFromHost();
    qprocess.close();
    qprocess.kill();
    qprocess.waitForFinished();
}


int main(int argc, char *argv[]){
    QString ocdpath, serial, uboot, fwfactory;
    quint16 port = 3000;
    QCoreApplication a(argc, argv);

    QStringList args = a.arguments();

    if(args.size() == 1){
        usage();
        return 1;
    }

#define DOUBLE_ARG_CHECK(s,a,b)   if(s.contains(a,Qt::CaseInsensitive)||s.contains(b,Qt::CaseInsensitive))
    for(int i=1; i<args.size(); i++){
        DOUBLE_ARG_CHECK(args.at(i), "-h", "--help"){
            usage();
            return 0;
        }
        DOUBLE_ARG_CHECK(args.at(i), "-o", "--ocd"){
            ocdpath = args.at(i+1);
            i++;
        }
        DOUBLE_ARG_CHECK(args.at(i), "-s", "--serial"){
            serial = args.at(i+1);
            i++;
        }
        DOUBLE_ARG_CHECK(args.at(i), "-p", "--port"){
            port = args.at(i+1).toShort();
            i++;
        }
        DOUBLE_ARG_CHECK(args.at(i), "-u", "--uboot"){
            uboot = args.at(i+1);
            i++;
        }
        DOUBLE_ARG_CHECK(args.at(i), "-f", "--fw-factory"){
            fwfactory = args.at(i+1);
            i++;
        }
    }
#undef DOUBLE_ARG_CHECK

    if(serial.isEmpty()){
        usage();
        qInfo();
        qInfo() << " Serial port not selected. Bailing.";
        return 1;
    }

    if(ocdpath.isEmpty())
        ocdpath = "openocd.exe";

    if(uboot.isEmpty())
        uboot = "u-boot.imx";

    qInfo() << "Running with: ";
    qInfo() << "   OCD path: " << ocdpath;
    qInfo() << "   Serial port: " << serial;
    qInfo() << "   Telnet port to OCD: " << port;
    qInfo() << "   UBoot image: " << uboot;
    qInfo() << "   Factory Firmware: " << fwfactory;

    qInfo() << "=========================================";
    qInfo() << "            Execution log";
    qInfo() << "=========================================";

    worker(ocdpath, serial, port, uboot, fwfactory);

    return 0;
}
