#include "getroot.h"
#include "ui_getroot.h"
#include <QKeyEvent>
#define T_PrivPtr( o ) (( StationaryLampSet *) o )


getroot::getroot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::getroot)
{
    ui->setupUi(this);
    //注册监视对象
    ui->lineEdit->installEventFilter(this);

    //设置窗口为对话框模式并设置窗口大小
    setWindowFlags(Qt::Dialog);   //窗体没有最大化最小化按钮
    setFixedSize(308, 160);  //设置窗体的大小

    //设置编辑框为密码输入模式并获取焦点
    ui->lineEdit->setEchoMode(QLineEdit::Password);ui->lineEdit->setFocus();

    //设置密码提示默认颜色
    ui->label_pass->setStyleSheet("color:rgb(100, 100, 100);");

}


getroot::~getroot()
{
    delete ui;
}

//切换密码输入模式
void getroot::on_pushButton_show_clicked()
{
   //检测输入模式是否为密码输入
   if(ui->lineEdit->echoMode()==QLineEdit::Password){
       ui->pushButton_show->setStyleSheet("QPushButton{border-image:url(:/ico/res/hide_pass.png);}");
       ui->lineEdit->setEchoMode(QLineEdit::Normal);

   }else{
        ui->pushButton_show->setStyleSheet("QPushButton{border-image:url(:/ico/res/show_pass.png);}");
        ui->lineEdit->setEchoMode(QLineEdit::Password);
     }

}
//监视对象,处理回车消息
bool getroot::eventFilter(QObject *target, QEvent *event)
{
     /*处理按键消息 */
    if (target == ui->lineEdit && event->type() == QEvent::KeyPress){
            /*强制类型转换 */
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == 16777220) {
                    getroot::on_pushButton_check_clicked();
                    return true;
            }
    }
    /*处理按键消息 */
    return QWidget::eventFilter(target, event);
}



void getroot::on_lineEdit_textEdited(const QString &arg1)
{

    if(ui->lineEdit->text()==""){
       ui->label_pass->show();
   }else{
       ui->label_pass->hide();
   }
}

void getroot::on_pushButton_close_clicked()
{
    getroot::close();
    emit quit();//发射退出信号

}

void getroot::on_getroot_destroyed()
{
    emit quit();//发射退出信号
}

void getroot::on_pushButton_check_clicked()
{
   if(ui->lineEdit->text()==""){
       ui->label_pass->setText("请输入密码！");
       ui->label_pass->setStyleSheet("color:rgb(255, 0, 0);");
       return;
   }

   //安全的强制类型转换
   Ventoy2Disk *main = static_cast<Ventoy2Disk*>(parentWidget());

    QString done=main->shell( "echo "+ui->lineEdit->text()+"|sudo -S fdisk -l");
   if(done==""){
        ui->lineEdit->setText("");
        ui->label_pass->show();
        ui->label_pass->setText("密码错误！");
        ui->label_pass->setStyleSheet("color:rgb(255, 0, 0);");
     }else{
      emit showmainwindow(ui->lineEdit->text()); //发出显示主窗口信号
      getroot::close();
   }

}
