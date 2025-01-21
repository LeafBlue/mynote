#include "mytextedit.h"

MyTextEdit::MyTextEdit() {}

MyTextEdit::MyTextEdit(QWidget *parent = nullptr):QTextEdit(parent)
{
    setAcceptDrops(true);
}

MyTextEdit::~MyTextEdit()
{

}

void MyTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }else {
        event->ignore();
    }
}

void MyTextEdit::dropEvent(QDropEvent *event)
{
    const QMimeData *mine = event->mimeData();
    if(mine->hasUrls()){
        QList<QUrl> list = mine->urls();
        QString drop_file =list.at(0).toLocalFile();
        //发射信号
        emit filedropped(drop_file);
    }else{
        event->ignore();
    }
}
