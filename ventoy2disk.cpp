#include "ventoy2disk.h"
#include "ui_ventoy2disk.h"

#include <QTimer>
#include <QMessageBox>
#include <QFileInfo>
#include <QClipboard>

#include<QTranslator>
#include<QDateTime>
#include<QMutex>
#include<QFile>
#include<QTextStream>
#include<QTextCodec>


Ventoy2Disk::Ventoy2Disk(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Ventoy2Disk)
{
    ui->setupUi(this);
    //注册启动事件
    //QTimer::singleShot(0, this, SLOT(init()));
}

Ventoy2Disk::~Ventoy2Disk()
{
    delete ui;
}

void Ventoy2Disk::init(QString word){
   pass=word;
   Ventoy2Disk::mySystem();
   Ventoy2Disk::show();                      
}

void Ventoy2Disk::log( QString msg)
{
    static QMutex mutex;
    mutex.lock();
   //日志写到文件
   QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
   QString message = QString("%1 %2%3").arg(current_date_time).arg("Debug:").arg(msg);
   QFile file(LOG_FILE);
   //方式：Append为追加，WriteOnly，ReadOnly
   file.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream text_stream(&file);
   text_stream << message << "\r\n";
   file.flush();
   file.close();
   mutex.unlock();
}

void Ventoy2Disk::mySystem()
{
    //取磁盘信息
    /*
    Disk /dev/nvme0n1: 477 GiB, 512110190592 bytes, 1000215216 sectors
    Disk model: WDC PC SN720 SDAPNTW-512G-1101
    ...
    Disk /dev/sda: 119.3 GiB, 128043712512 bytes, 250085376 sectors
    Disk model: Extreme Pro

    QString shell;
    //检测视图授权程序
    shell="pkexec -v foo >/dev/null 2>&1 || { echo >&2 'not command!'; exit 1; }";
   if(Ventoy2Disk::shell(shell)=="not command!"){
        shell="gksudo fdisk -l" ;
    }else{
        shell="pkexec fdisk -l" ;
    }
*/
    QString shell="echo "+pass+"|sudo -S fdisk -l";
    QString done=Ventoy2Disk::shell(shell);

    if(done==""){
        perror("shell fail");
        printf("shell error: %s/n", strerror(errno));
        return;
    }

    //编码转换
    QTextCodec *codec = QTextCodec::codecForLocale();
    char* ch; QByteArray ba = done.toLatin1(); ch=ba.data();
    done = codec->toUnicode(ch);

    Ventoy2Disk::log("\r\n"+done);

    QRegExp rxlen("Disk.(/dev/sd\\w*).(.+)(?:,|，)");
    rxlen.setMinimal (true) ;
    int pos=0;
    ui->comboBox->clear();
    //循环获取所有移动设备
    while ((pos=rxlen.indexIn(done,pos))!=-1){
        QString value = rxlen.cap(1);
        QString unit = rxlen.cap(2);
        ui->comboBox->addItem(value);
        pos+= rxlen.matchedLength();
        ui->textEdit->append(value+" "+unit);
        Ventoy2Disk::log(value+" "+unit);

    }

    if(ui->comboBox->currentText()==""){
         ui->textEdit->append("没有找到移动设备！");

    }
}

QString Ventoy2Disk::shell(QString shell)
{
    FILE * fp;
    int res;
    QString  pword;
    char resultStr[100] = {"0"}; //define MAX_USB_NUM 3
    char* ch; QByteArray ba = shell.toLatin1(); ch=ba.data();
    //执行预先设定的命令，并读出该命令的标准输出
     fp = popen(ch, "r");
     if(!fp)
     {
        perror("popen fail");
        printf("popen error: %s/n", strerror(errno));
        return "";
     }

       /* 循环读管道 #define MAX_USB_NUM 3  */
       while(fgets(resultStr, 3, fp) ){pword+=QString(QLatin1String(resultStr));}
       /*等待命令执行完毕并关闭管道及文件指针*/
       res = pclose(fp);
       if ( res == -1 || pword=="")
       {
           Ventoy2Disk::log("close popen file pointer fp error!");
           //printf("close popen file pointer fp error!\n");
           return "";
       }
       else
       {
           //printf("popen res is :%d\n", res);
           return pword;

       }
}

void Ventoy2Disk::install(QString type)
{
    QString dir=ui->comboBox->currentText();
    if(dir==""){
        QMessageBox::warning(nullptr, "操作提示", "没有发现移动设备",QMessageBox::Yes);
        return;
    }

    QFileInfo fileInfo("./Ventoy2Disk.sh");
    if(!fileInfo.isFile()){
        QMessageBox::warning(nullptr, "操作提示", "请在程序安装目录运行",QMessageBox::Yes);
        return;

    }

    QMessageBox:: StandardButton result= QMessageBox::information(nullptr, "操作警告", "非升级安装会清空所有数据,确定安装吗？\r\n如果是深度终端将直接打开终端,输入用户密码即可完成安装\r\n非深度终端需在程序目录里右键打开终端后粘贴执行并输入密码后完成安装",QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes){
        QString shell="sudo ./Ventoy2Disk.sh "+type +" "+dir;
        //复制到剪切板+
        QClipboard *clip=QApplication::clipboard();clip->setText(shell);
        //输出操作提示
        ui->textEdit->append("如果不是深度终端,请在程序目录里右键打开终端后粘贴执行。");
        ui->textEdit->append("已经拷贝安装脚本到剪切版了,如果失败请手动复制：\r\n"+shell);
        ui->textEdit->find(shell,QTextDocument::FindBackward);
        //卸载U盘主分区
        Ventoy2Disk::shell("umount "+dir+"1");
        //调用深度终端执行
        Ventoy2Disk::shell("deepin-terminal -e "+shell);
    }

 }

void Ventoy2Disk::on_pushButton_fdisk_clicked()
{
    QString dir=ui->comboBox->currentText();

    if(dir==""){
        QMessageBox::warning(nullptr, "警告", "没有发现移动设备！",QMessageBox::Yes);
         return;
    }

    QMessageBox:: StandardButton result= QMessageBox::information(nullptr, "操作警告", "确定将设备:"+dir+" 进行空间回收吗？数据将全部删除",QMessageBox::Yes|QMessageBox::No);
    if(result==QMessageBox::Yes){

        if(dir!=""){
             //卸载U盘主分区
             Ventoy2Disk::shell("umount "+dir+"1");
             //快速格式化
            // QString shell="pkexec dd if=/dev/zero of=" +dir+" bs=512K count=1";
             QString shell="echo "+pass+"|sudo -S dd if=/dev/zero of=" +dir+" bs=512K count=1";
             QString pword=  Ventoy2Disk::shell(shell);
             ui->textEdit->append("执行成功,建议重新插拔U盘！");
        }

    }

}

void Ventoy2Disk::on_pushButton_ref_clicked()
{
  ui->textEdit->setText("");
Ventoy2Disk::mySystem();
}

void Ventoy2Disk::on_pushButton_install_clicked()
{
Ventoy2Disk::install("-i");
}

void Ventoy2Disk::on_pushButton_reinstrll_clicked()
{
Ventoy2Disk::install("-I");
}

void Ventoy2Disk::on_pushButton_update_clicked()
{
Ventoy2Disk::install("-u");
}
