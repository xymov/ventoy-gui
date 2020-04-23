#include "ventoy2disk.h"
#include "getroot.h"
#include <QApplication>

#include<QTranslator>
#include<QDateTime>
#include<QMutex>
#include<QFile>
#include<QTextStream>

//测试


void MessageOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(MessageOutPut);
    QApplication a(argc, argv);
    Ventoy2Disk w;
    getroot dlg;
    //关联子窗口的show信号
    QObject::connect(&dlg,SIGNAL(showmainwindow(QString)),&w,SLOT(init(QString)));
    //关联子窗口的close信号
    QObject::connect(&dlg,SIGNAL(quit()),&a,SLOT(quit()));
    //显示验证窗口
    dlg.show();
    //w.show();
    return a.exec();
}
void MessageOutPut(QtMsgType type, const QMessageLogContext &context, const QString &msg)

{
    static QMutex mutex;
    mutex.lock();
    QString text;
   switch(type)
   {
   case QtDebugMsg:
       text = QString("Debug:");
       break;

   case QtWarningMsg:
       text = QString("Warning:");
       break;

   case QtCriticalMsg:
       text = QString("Critical:");
       break;

   case QtFatalMsg:
       text = QString("Fatal:");
       break;
   default:
       break;
   }
   //日志写到文件
   QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
   QString message = QString("%1 %2%3").arg(current_date_time).arg(text).arg(msg);
   QFile file(LOG_FILE);
   //方式：Append为追加，WriteOnly，ReadOnly
   file.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream text_stream(&file);
   text_stream << message << "\r\n";
   file.flush();
   file.close();
   mutex.unlock();

}




