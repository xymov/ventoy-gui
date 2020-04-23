#ifndef GETROOT_H
#define GETROOT_H

#include <QWidget>
#include "ventoy2disk.h"
namespace Ui {
class getroot;
}

class getroot : public QWidget
{
    Q_OBJECT

public:
    explicit getroot(QWidget *parent = nullptr);
    ~getroot();

private slots:
    void on_pushButton_show_clicked();

    void on_lineEdit_textEdited(const QString &arg1);

    void on_pushButton_close_clicked();

    void on_getroot_destroyed();

    void on_pushButton_check_clicked();

signals:

    void showmainwindow(QString);//显示主窗口信号
    void quit();//退出信号

private:

    Ui::getroot *ui;
    bool eventFilter(QObject *target, QEvent *event);
};

#endif // GETROOT_H
