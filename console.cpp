#include "console.h"
#include <QDebug>

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{

}

int Console::asciitonum( char data ){
    int ret = 0;
//    qDebug()<<""<<data;
    if( data >='0' && data <='9' )
        ret = data-'0';
    else if (data >='a' && data <='f') {
        ret = data-'a'+10;
    }
    else if (data >='A' && data <='F') {
        ret = data-'A'+10;
    }
  //   qDebug()<<"----"<<ret;
    return ret;
}

void Console:: handle_data( const QByteArray data){
    static int rev[8] = {0,};
    static bool is_start = false;
    int res = 0;
    static int index = 0;
    for(  int i = 0; i<data.length();i += 2){
        res = (this->asciitonum(data.at(i))<<4 | this->asciitonum(data.at(i+1)))& 0x00FF;
        qDebug()<<"receive info:"<<data.length()<<data.at(i)<<data.at(i+1)<<res;
        if(res == 0x5A){
            is_start = true;
            index = 0;
        }
        if(is_start){
            rev[index]= res;
            index++;
        }
        if(index >= 7 ){
            is_start = false;
        }
    }
    if(rev[5] == 0x57 )
    {
        if( rev[2] == 0x10 && rev[3] == 0x01){ //接收到上升指令，开始上升

        }
        else if( rev[2] == 0x10 && rev[3] == 0x02){//接收到下降指令，开始下降

        }
        else if( rev[2] == 0x10 && rev[3] == 0x11){//车位锁上升完成

        }
        else if( rev[2] == 0x10 && rev[3] == 0x22){//车位锁下降完成

        }
        else if( rev[2] == 0x20 && rev[3] == 0x00){//车位锁处于下降状态

        }
        else if( rev[2] == 0x20 && rev[3] == 0x01){//车位锁处于上升状态

        }

        qDebug()<<"receive info success:"<<rev[2] << rev[3];
        memset(rev,0,sizeof (rev));
    }
}
void Console::putData(const QByteArray &data)
{
  //  emit getData(data);
    // qDebug()<<"hello hhh "<<data<<"!"<<endl;
    this->handle_data(data);
}
