#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_btn_encode_clicked();

    void on_btn_hex_clicked();

    void on_btn_url_clicked();

    void on_btn_base64_clicked();

    void on_btn_uni_clicked();

    void on_btn_md5_clicked();

    void on_cb_cl_stateChanged(int arg1);
    void on_cb_sha_currentTextChanged(const QString &arg1);

private:
    QTextCodec* get_select_charset();
    //获取输入的数据
    QByteArray get_input_data();
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
