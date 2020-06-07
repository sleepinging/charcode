#include "widget.h"
#include "ui_widget.h"

#include <QDebug>
#include <QTextCodec>
#include <QByteArray>
#include <QMessageBox>
#include <QCryptographicHash>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    for(const auto& a:QTextCodec::availableCodecs()){
        ui->cb_cs->addItem(a);
//        qDebug()<<QString(a);
    }
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_btn_encode_clicked()
{
    auto str1=ui->te_org->toPlainText();
//    QString str1="ABC中国";
    auto utf8 = QTextCodec::codecForName("UTF-8");
    auto strUnicode= utf8->toUnicode(str1.toUtf8().data());
    auto qtc = get_select_charset();
    auto bs=qtc->fromUnicode(strUnicode);

    //HEX
    {
        if(!ui->cb_cl->isChecked()){
            ui->te_hex->setText(bs.toHex(' ').toUpper());
        }else{
            auto t=bs.toHex(' ').toUpper().replace(" ",",0x");
            t="{0x"+t+"}";
            ui->te_hex->setText(t);
        }
    }

    //url
    ui->te_url->setText(bs.toPercentEncoding());

    //base64
    ui->te_base64->setText(bs.toBase64());

    //md5
    {
        QString md5;
        auto data=QCryptographicHash::hash(bs, QCryptographicHash::Md5);
        auto hex=data.toHex();
        //32位大写
        ui->te_md5->setText(hex.toUpper());
        //32位小写
        ui->te_md5->append(hex);
        //16位大写
        ui->te_md5->append(hex.mid(8,16).toUpper());
        //16位小写
        ui->te_md5->append(hex.mid(8,16));
    }

    //转unicode显示
    {
        QString us;
        auto bb=bs;
        if(bb.size()%2!=0){
            bb.append('\0');
        }
        //UTF-16大小端转化(辣鸡windows)
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
    if(ui->cb_cl->isChecked()){
        t=t.replace("{","").replace("}","").replace(",0x"," ").replace("0x","");
    }
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
    auto tc=get_select_charset();
    ui->te_org->setText(tc->toUnicode(bs));
}

void Widget::on_btn_url_clicked()
{
    auto t=ui->te_url->toPlainText();
    auto bs=QByteArray::fromPercentEncoding(t.toLatin1());
    auto tc=get_select_charset();
    ui->te_org->setText(tc->toUnicode(bs));
}

void Widget::on_btn_base64_clicked()
{
    auto t=ui->te_base64->toPlainText();
    auto bs=QByteArray::fromBase64(t.toLatin1());
    auto tc=get_select_charset();
    ui->te_org->setText(tc->toUnicode(bs));
}

void Widget::on_btn_uni_clicked()
{
    auto t=ui->te_uni->toPlainText();
    t=t.replace("\\u","");
    auto bb=QByteArray::fromHex(t.toLatin1());
    //UTF-16大小端转化(辣鸡windows)
    for(int i=0;i<bb.size();i+=2){
        auto t=bb.at(i+1);
        bb[i+1]=bb[i];
        bb[i]=t;
    }
    auto tc=get_select_charset();
    ui->te_org->setText(tc->toUnicode(bb));
}

void Widget::on_btn_md5_clicked()
{

}

void Widget::on_cb_cl_stateChanged(int arg1)
{
    auto t=ui->te_hex->toPlainText();
    //C=>hex
    if(arg1==Qt::Unchecked){
        t=t.replace("{","").replace("}","").replace(",0x"," ").replace("0x","");
    }else{
        //hex=>C
        t=t.replace(" ",",0x");
        t="{0x"+t+"}";
    }
    auto tc=get_select_charset();
    ui->te_hex->setText(tc->toUnicode(t.toLatin1()));
}

QTextCodec* Widget::get_select_charset()
{
    auto cs=ui->cb_cs->currentText().toUtf8();
    auto pcs=static_cast<const char*>(cs.data());
//    qDebug()<<pcs;
    return QTextCodec::codecForName(pcs);
}
