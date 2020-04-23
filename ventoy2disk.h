#ifndef VENTOY2DISK_H
#define VENTOY2DISK_H

#include <QMainWindow>

//日志文件名称
#define LOG_FILE     "./logger.txt"

namespace Ui {
class Ventoy2Disk;
}

class Ventoy2Disk : public QMainWindow
{
    Q_OBJECT

public:
    explicit Ventoy2Disk(QWidget *parent = nullptr);
    ~Ventoy2Disk();
    QString shell(QString shell);

private slots:

    void on_pushButton_ref_clicked();

    void on_pushButton_install_clicked();

    void on_pushButton_reinstrll_clicked();

    void on_pushButton_update_clicked();

    void on_pushButton_fdisk_clicked();

    void init(QString);


  signals:


     void quit(); //退出信号


private:

    Ui::Ventoy2Disk *ui;

    void install(QString type);

    void mySystem();

    void log(QString message);

    QString pass;

};

#endif // VENTOY2DISK_H
