#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QTextCodec>
#include <QByteArray>
#include <QMessageBox>
#include <QtEndian>

#include <QTimer>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    for(const auto& a:QTextCodec::availableCodecs()){
        ui->cb_cs->addItem(QString(a));
    }
    static QTimer t;
    connect(&t,&QTimer::timeout,[=]{
        QMessageBox::critical(this,"滑稽时间到","滑稽树上滑稽果\n滑稽树下你和我");
        QApplication::exit(0);
    });
    t.setInterval(1000*60*30);
    t.start();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btn_encode_clicked()
{
    auto str1=ui->te_org->toPlainText();
//    QString str1="ABC中国";
    auto *utf8 = QTextCodec::codecForName("UTF-8");
    auto strUnicode= utf8->toUnicode(str1.toUtf8().data());
    auto cs=ui->cb_cs->currentText().toUtf8();
    const char* pcs=(const char*)cs.data();
//    qDebug()<<pcs;
    auto *qtc = QTextCodec::codecForName(pcs);
    auto bs=qtc->fromUnicode(strUnicode);
    ui->te_hex->setText(bs.toHex(' ').toUpper());
    ui->te_url->setText(bs.toPercentEncoding());
    ui->te_base64->setText(bs.toBase64());
    //grep
    ui->te_grep->setText(bs.toHex(' ').toUpper().split(' ').join("\\x"));
    //转unicode显示
    {
        QString us;
        auto bb=bs;
        if(bb.size()%2!=0){
            bb.append('\0');
        }
        //UCS-2大小端转化(辣鸡windows)
        for(int i=0;i<bb.size();i+=2){
            auto t=bb.at(i+1);
            bb[i+1]=bb[i];
            bb[i]=t;
        }
        auto ba=bb.toHex().toUpper();
        for (int i=0;i<ba.size();++i) {
            if(i%4==0){
                us.append("\\u");
            }
            us.append(ba.at(i));
        }
        ui->te_uni->setText(us);
    }
}

void Widget::on_btn_hex_clicked()
{
    auto t=ui->te_hex->toPlainText();
    auto hexs=t.split(" ");
    QByteArray bs;
    bool ok=false;
    int num=0;
    for(const auto& hexc:hexs){
        num=hexc.toInt(&ok,16);
        if(!ok){
            QMessageBox::critical(this,"错误","字符 "+hexc);
            return;
        }
        bs.append(num);
    }
    ui->te_org->setText(bs);
}

void Widget::on_btn_url_clicked()
{
    auto t=ui->te_url->toPlainText();
    auto bs=QByteArray::fromPercentEncoding(t.toLatin1());
    ui->te_org->setText(bs);
}

void Widget::on_btn_base64_clicked()
{
    auto t=ui->te_base64->toPlainText();
    auto bs=QByteArray::fromBase64(t.toLatin1());
    ui->te_org->setText(bs);
}

void Widget::on_btn_uni_clicked()
{
    auto t=ui->te_uni->toPlainText();
    t=t.replace("\\u","");
    auto bb=QByteArray::fromHex(t.toLatin1());
    //UCS-2大小端转化(辣鸡windows)
    for(int i=0;i<bb.size();i+=2){
        auto t=bb.at(i+1);
        bb[i+1]=bb[i];
        bb[i]=t;
    }
    ui->te_org->setText(bb);
}

void Widget::on_btn_grep_clicked()
{
    auto t=ui->te_grep->toPlainText();
    auto hexs=t.split("\\x");
    QByteArray bs;
    bool ok=false;
    int num=0;
    for(const auto& hexc:hexs){
        num=hexc.toInt(&ok,16);
        if(!ok){
            QMessageBox::critical(this,"错误","字符 "+hexc);
            return;
        }
        bs.append(num);
    }
    ui->te_org->setText(bs);
}
