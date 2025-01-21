#ifndef MYTEXTEDIT_H
#define MYTEXTEDIT_H

#include <QObject>
#include <QTextEdit>
#include <QWidget>
#include<QDragEnterEvent>
#include<QDropEvent>
#include<QMimeData>
#include<QUrl>
#include<QList>

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyTextEdit();

    MyTextEdit(QWidget *parent);

    ~MyTextEdit();
signals:
    //定义一个信号
    void filedropped(QString& drop_file);

protected:
    //重写拖动函数
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // MYTEXTEDIT_H
