#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QApplication>
#include<QScreen>
#include<QMenu>
#include<QMenuBar>
#include<QAction>
#include<QWidget>
#include<QLayout>
#include<QTextEdit>
#include<QStatusBar>
#include<QFileDialog>
#include<QTextStream>
#include<QFile>
#include<QMessageBox>
#include<QLabel>
#include<QLineEdit>
#include<QDebug>
#include<QCloseEvent>
#include<QPushButton>
#include<QPointer>
#include<QTextCursor>
#include<QSyntaxHighlighter>
#include<QRegularExpression>
#include<QRegularExpressionMatchIterator>
#include<QVector>
#include<QPair>
#include<QColor>
#include<QDragEnterEvent>
#include<QDropEvent>
#include<QMimeData>
#include<QUrl>
#include<QList>
#include<QEvent>
#include"mytextedit.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void set_windowsize();
    void set_mainmenu();
    void set_nextmenu();
    void set_workarea();
    void set_status();
    void set_longstatus(QString longstatus);
    void set_editfont(qreal font_size);

    //其他函数
    bool save_before_close();
    bool save_file_content(QFile* savefile,QString &content);
    void set_text_color();



public slots:
    void open_file(QString dropname = "");
    void new_file();
    void save_file(bool isresave);

    void do_seacher();

    //查找替换
    void find_text(QString findtext);
    void change_text(QString findtext,QString change_t);
    void change_all_text(QString findtext,QString change_t);


    //重写关闭
    void closeEvent(QCloseEvent *event);
protected:
    //重写拖动函数
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private:
    //Ui::MainWindow *ui;
    //功能类
    QAction *ac_newnote;
    QAction *ac_opennote;
    QAction *ac_seacher;
    QAction *ac_replace;
    QAction *ac_save;
    QAction *ac_resave;

    QPointer<QWidget> seach_wid;


    //其他全局变量
    MyTextEdit *work_edit;
    QFont edit_font;
    QStatusBar *now_status;
    QLabel *label;

    int s_width;
    int s_height;
    QString filename;
    QString content;

    QFile *file;

};
#endif // MAINWINDOW_H
