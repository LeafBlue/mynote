#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    //, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    //初始化窗口大小
    int s_width = 800;
    int s_height = 600;
    //初始化文档信息
    content = "";
    work_edit = new MyTextEdit(this);


    file = new QFile();
    filename = "";
    //初始化搜索查找窗口
    seach_wid = new QWidget();
    //接收拖动
    setAcceptDrops(false);

    //初始化状态显示
    this->setWindowTitle(QObject::tr("mynote"));
    set_status();
    set_longstatus(QObject::tr("就绪。"));

    //初始化各部件
    set_windowsize();
    set_nextmenu();
    set_mainmenu();
    set_workarea();
    set_editfont(16);

    connect(work_edit,&MyTextEdit::filedropped,this,&MainWindow::open_file);

}

MainWindow::~MainWindow()
{
    //写完后统一处理指针
    //delete ui;
}

//设置窗口首次打开大小
void MainWindow::set_windowsize()
{


    QScreen *screen = QApplication::primaryScreen();
    if(!screen){
        //qWarning("获取屏幕大小失败");
        this->resize(s_width,s_height);
        return;
    }

    QRect screen_data = screen->availableGeometry();

    s_width = screen_data.width() * 0.8;
    s_height = screen_data.height()*0.8;

    this->resize(s_width,s_height);
}

//初始化主菜单栏
void MainWindow::set_mainmenu()
{
    QMenuBar *mainmenu = this->menuBar();

    QMenu *menu1 = mainmenu->addMenu("文件");
    QMenu *menu2 = mainmenu->addMenu("编辑");
    QMenu *menu3 = mainmenu->addMenu("查找");
    QMenu *menu4 = mainmenu->addMenu("其他");

    menu1->addAction(ac_newnote);
    menu1->addAction(ac_opennote);
    menu1->addAction(ac_save);
    menu1->addAction(ac_resave);
    menu3->addAction(ac_seacher);
    menu3->addAction(ac_replace);
}

//此处对action初始化，必须先调用它再调用其他函数
void MainWindow::set_nextmenu()
{

    ac_newnote = new QAction("新建",this);
    ac_newnote->setShortcut(QKeySequence("Ctrl+N"));
    connect(ac_newnote,&QAction::triggered,this,&MainWindow::new_file);

    ac_opennote = new QAction("打开",this);
    ac_opennote->setShortcut(QKeySequence("Ctrl+O"));
    connect(ac_opennote,&QAction::triggered,this,[this](){
        this->open_file();
    });

    ac_save = new QAction("保存",this);
    ac_save->setShortcut(QKeySequence("Ctrl+S"));
    connect(ac_save,&QAction::triggered,this,[this](){
        this->save_file(false);
    });

    ac_resave = new QAction("另存为",this);
    connect(ac_resave,&QAction::triggered,this,[this](){
        this->save_file(true);
    });

    ac_seacher = new QAction("查找",this);
    ac_seacher->setShortcut(QKeySequence("Ctrl+F"));
    connect(ac_seacher,&QAction::triggered,this,&MainWindow::do_seacher);

    ac_replace = new QAction("替换",this);
    ac_replace->setShortcut(QKeySequence("Ctrl+H"));
    connect(ac_replace,&QAction::triggered,this,&MainWindow::do_seacher);


}
//设置工作区
void MainWindow::set_workarea()
{
    QWidget *center_widget = new QWidget(this);
    this->setCentralWidget(center_widget);

    QVBoxLayout *layout = new QVBoxLayout(center_widget);
    layout->setContentsMargins(0,0,0,0);
    work_edit->setStyleSheet("QTextEdit { selection-background-color: yellow; selection-color :black; }");
    layout->addWidget(work_edit);

    //作为textedit的父部件，也要开启拖动
    center_widget->setAcceptDrops(false);
    //设置接受拖放
    work_edit->setAcceptDrops(true);
    //设置事件过滤
    work_edit->installEventFilter(this);
}
//初始化状态栏
void MainWindow::set_status()
{
    now_status = this->statusBar();

    label = new QLabel(this);
    QHBoxLayout *layout = new QHBoxLayout(now_status);
    layout->addWidget(label);
    layout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //添加为永久部件
    now_status->addPermanentWidget(label);
    now_status->showMessage("临时消息",5000);

}

void MainWindow::set_longstatus(QString longstatus)
{
    label->setText(longstatus);
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}



//初始化并设置字体
void MainWindow::set_editfont(qreal font_size)
{
    edit_font.setPointSize(font_size);
    work_edit->setFont(edit_font);
}

void MainWindow::do_seacher()
{
    if(seach_wid && seach_wid->isVisible()){
        return;
    }
    //制作一个小窗口
    seach_wid = new QWidget(this);
    seach_wid->setWindowFlags(Qt::Window);
    //用这个代码确保关闭时能够删除自身
    seach_wid->setAttribute(Qt::WA_DeleteOnClose);

    seach_wid->setWindowTitle(QObject::tr("查找与替换"));

    QLineEdit *searchedit = new QLineEdit(seach_wid);
    QLineEdit *changeedit = new QLineEdit(seach_wid);

    QPushButton *search_btn = new QPushButton(seach_wid);
    search_btn->setText("查找");
    QPushButton *change_btn = new QPushButton(seach_wid);
    change_btn->setText("替换");
    QPushButton *change_all = new QPushButton(seach_wid);
    change_all->setText("替换全部");

    QVBoxLayout *layout = new QVBoxLayout(seach_wid);

    QHBoxLayout *lay1 = new QHBoxLayout(seach_wid);
    lay1->addWidget(searchedit);
    lay1->addWidget(search_btn);
    layout->addLayout(lay1);
    QHBoxLayout *lay2 = new QHBoxLayout(seach_wid);
    lay2->addWidget(changeedit);
    lay2->addWidget(change_btn);
    layout->addLayout(lay2);
    QHBoxLayout *lay3 = new QHBoxLayout(seach_wid);
    lay3->addWidget(change_all);
    layout->addLayout(lay3);

    connect(search_btn,&QPushButton::clicked,this,[this,searchedit](){
        QString findtext = searchedit->text();
        this->find_text(findtext);
    });

    connect(change_btn,&QPushButton::clicked,this,[this,searchedit,changeedit](){
        QString findtext = searchedit->text();
        QString change_t = changeedit->text();
        this->change_text(findtext,change_t);
    });

    connect(change_all,&QPushButton::clicked,this,[this,searchedit,changeedit](){
        QString findtext = searchedit->text();
        QString change_t = changeedit->text();
        this->change_all_text(findtext,change_t);
    });

    seach_wid->resize(320,160);
    seach_wid->show();
    seach_wid->raise();
    seach_wid->activateWindow();
}

void MainWindow::find_text(QString findtext)
{
    if(!work_edit->find(findtext)){
        //移动光标到最前面
        QTextCursor cursor =work_edit->textCursor();
        cursor.movePosition(QTextCursor::Start);
        work_edit->setTextCursor(cursor);
    }else{
        this->setFocus();
    }
}

void MainWindow::change_text(QString findtext,QString change_t)
{
    QTextCursor cursor =work_edit->textCursor();
    QString choose = cursor.selectedText();
    if(choose == ""){
        //说明当前什么都米选择，执行一遍查找
        find_text(findtext);
        //再次判断，还是没有，点击一次替换，只能查找一次，除非用户再点击
        choose = cursor.selectedText();
        if(choose == ""){
            return;
        }
    }
    if(choose == findtext){
        //当前选中的是要替换的文本
        cursor.beginEditBlock();//开始编辑事务（事务，代表这几步是一个整体）
        cursor.removeSelectedText();
        cursor.insertText(change_t);
        cursor.endEditBlock();//结束编辑事务

        //替换结束，再次查找定位到下一个
        find_text(findtext);
    }else{
        //选中了文本，但是不是要找的
        find_text(findtext);
        change_text(findtext,change_t);
    }
}

void MainWindow::change_all_text(QString findtext,QString change_t)
{
    QString content_1 = work_edit->toPlainText();
    content_1.replace(findtext,change_t);
    work_edit->setText(content_1);
}

//关闭或新建检查文件是否要保存
bool MainWindow::save_before_close()
{
    QString content_1 = work_edit->toPlainText();
    if(content != content_1){

        QMessageBox msgbox(this);
        msgbox.setWindowTitle(QObject::tr("提示"));
        msgbox.setText(QObject::tr("是否保存文件") + (filename == ""?"": ("  " + filename) )+ QObject::tr("？"));
        QPushButton *button1 = msgbox.addButton(QObject::tr("保存"),QMessageBox::YesRole);
        QPushButton *button2 = msgbox.addButton(QObject::tr("不保存"),QMessageBox::NoRole);
        QPushButton *button3 = msgbox.addButton(QObject::tr("取消"),QMessageBox::RejectRole);
        msgbox.exec();

        QMessageBox::ButtonRole role = msgbox.buttonRole(msgbox.clickedButton());
        if(role == QMessageBox::YesRole){
            //如果文件已经打开，则可以直接保存到它属于的文件。如果文件不存在或者没打开，说明是新建文件，弹窗保存
            if(file && file->isOpen()){
                save_file(false);
            }else{
                save_file(true);
            }
            return true;
        }else if(role == QMessageBox::RejectRole){
            return false;
        }
    }
    return true;
}

//重写关闭事件
void MainWindow::closeEvent(QCloseEvent *event){
    if(save_before_close()){
        event->accept();
    }else{
        event->ignore();
    }
}

void MainWindow::open_file(QString dropname)
{
    //获取当前文件内容，如果与全局内容不一致  或者文件已打开
    QString content_1 = work_edit->toPlainText();
    if((file && file->isOpen()) || (content != content_1)){
        //此函数若是调用了保存，最后会保持file是打开状态
        if(!save_before_close()){
            return;
        }
        //如果新建文件，但不执行保存，会导致file不存在，没有open，所以再判断一次
        if(file && file->isOpen()){
            file->close();
            delete file;
        }
        file = nullptr;
    }

    if(dropname==""){
        filename = QFileDialog::getOpenFileName(this,QObject::tr("打开文件"),"D:",QObject::tr("文本文档(*.txt)"));
    }else{
        filename = dropname;
    }
    if(!filename.isEmpty()){
        file = new QFile(filename);
        //当使用此函数时，会自动创建一个句柄，占用当前文件。当
        if(file->open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream in(file);
            in.setEncoding(QStringConverter::Utf8);

            now_status->showMessage("正在打开文件",3000);
            content = in.readAll();
            work_edit->setText(content);
            set_text_color();
            this->setWindowTitle(QObject::tr("mynote - 当前文件 ") + filename);

        }else{
            QMessageBox::warning(this,QObject::tr("读取文件失败！"),file->errorString());
        }
    }else{
        QMessageBox::warning(this,QObject::tr("文件不存在"),QObject::tr("文件不存在"));
    }
}


//新建
void MainWindow::new_file()
{
    QString content_1 = work_edit->toPlainText();
    if((file && file->isOpen()) || (content != content_1)){
        //此函数若是调用了保存，最后会保持file是打开状态
        if(!save_before_close()){
            return;
        }
        //如果此处是新建的文件，但不执行保存，会导致file不存在，也没有open，所以再判断一次
        if(file && file->isOpen()){
            file->close();
            delete file;
        }
        file = nullptr;
    }
    content = "";
    work_edit->setText(content);
    filename = "";
    this->setWindowTitle(QObject::tr("mynote - 当前文件 临时文件"));
}

//保存-另存为-新建保存
void MainWindow::save_file(bool isresave){
    //获取当前输入的内容
    QString content_1 = work_edit->toPlainText();

    if(isresave || !file || !file->isOpen()){
        //当另存为或者新建保存时要弹窗，进入此逻辑
        QString savename = QFileDialog::getSaveFileName(this,QObject::tr("保存文件"),"D:",QObject::tr("文本文档(*.txt);;所有文件(*)"));
        if(savename.isEmpty()){
            //用户取消了保存
            return;
        }

        QFile *savefile = new QFile(savename);
        //打开临时文件
        if(!savefile->open(QIODevice::ReadWrite | QIODevice::Text)){
            qDebug()<<"文件打开失败";
            delete savefile;
            savefile = nullptr;
            return;
        }
        //尝试保存临时文件
        if(!save_file_content(savefile,content_1)){
            now_status->showMessage("保存失败",3000);
            savefile->close();
            delete savefile;
            savefile = nullptr;
            return;
        }
        //关闭原file文件，清理资源
        //在这一步操作前，file都是open的
        //如果是新建，这个file则没有打开
        if(file && file->isOpen()){
            file->close();
            delete file;
        }
        //将临时资源交给全局变量
        file = std::move(savefile);
        filename = savename;

        //修改状态
        this->setWindowTitle(QObject::tr("mynote - 当前文件 ") + filename);
        now_status->showMessage("另存为成功",3000);
        set_text_color();
    }else{
        if(save_file_content(file,content_1)){
            set_text_color();
            now_status->showMessage("保存成功",3000);
        }else{
            now_status->showMessage("保存失败",3000);
        }
    }
}
//保存操作
bool MainWindow::save_file_content(QFile* savefile,QString &content_1){

    QTextStream out(savefile);
    out.setEncoding(QStringConverter::Utf8);

    out<<content_1;
    if(out.status() != QTextStream::Ok){
        qDebug()<<"文件写入失败";
        return false;
    }
    //每次进行保存行为，都会把新的文字替换到全局变量，这样便可根据全局变量判断是否被修改过
    if(content != content_1){
        content = content_1;
    }
    return true;
}

//设置英文字母高亮
void MainWindow::set_text_color()
{
    //创建一个vector存储样式
    QVector<QPair<QTextCharFormat,QRegularExpression>> vector;

    QTextCharFormat format;
    format.setForeground(Qt::blue);
    QString pattern = "[a-zA-Z]";
    QRegularExpression qre(pattern);

    QPair<QTextCharFormat,QRegularExpression> pair(format,qre);
    vector.push_back(pair);

    //处理汉字
    QTextCharFormat format3;
    format3.setForeground(QColor("#32CD32"));
    QString pattern3 = "[\u4e00-\u9fff\u3400-\u4dbf\U00020000-\U0002a6df]";
    QRegularExpression qre3(pattern3);

    QPair<QTextCharFormat,QRegularExpression> pair3(format3,qre3);
    vector.push_back(pair3);
    //处理注释
    QTextCharFormat format1;
    format1.setForeground(QColor("#228B22"));
    QString pattern1 = "//.*";
    QRegularExpression qre1(pattern1);

    QPair<QTextCharFormat,QRegularExpression> pair1(format1,qre1);
    vector.push_back(pair1);

    QTextCharFormat format2;
    format2.setForeground(QColor("#228B22"));
    QString pattern2 = "#.*";
    QRegularExpression qre2(pattern2);

    QPair<QTextCharFormat,QRegularExpression> pair2(format2,qre2);
    vector.push_back(pair2);

    QTextDocument *doc = work_edit->document();
    QTextCursor cursor(doc);
    cursor.beginEditBlock();

    for(size_t i =0;i < vector.size(); i++){
        QPair<QTextCharFormat,QRegularExpression> qpair = vector[i];
        QRegularExpressionMatchIterator it = (qpair.second).globalMatch(doc->toPlainText());

        while(it.hasNext()){
            QRegularExpressionMatch match = it.next();
            cursor.setPosition(match.capturedStart());
            cursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor,match.capturedLength());
            cursor.mergeCharFormat(qpair.first);
        }
    }
}

//这只能在主窗口生效，如果Qtextedit生效还要做其他的操作
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()){
        event->acceptProposedAction();
    }else {
        event->ignore();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mine = event->mimeData();
    if(mine->hasUrls()){
        QList<QUrl> list = mine->urls();
        QString drop_file =list.at(0).toLocalFile();
        open_file(drop_file);
    }
}




















