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

unsigned char crc(unsigned char * buf,int size){
    unsigned char sum = 0;
    for( int i = 0; i<size; i++){
        sum ^=buf[i];
    }
    return sum;
}

void Console:: handle_data( const QByteArray data){
    //AC 03 03 01 01 00 CC
    static unsigned char rev[512] = {0,};
    static bool is_start = false;
    static bool is_end = false;
    int res = 0;
    static int index = 0;
  //  qDebug("length:%d\n",data.length());
    for(  int i = 0; i<data.length();i += 2){
        res = (this->asciitonum(data.at(i))<<4 | this->asciitonum(data.at(i+1)))& 0x00FF;
       // qDebug()<<"receive info:"<<data.length()<<data.at(i)<<data.at(i+1)<<res<<endl;
      //  qDebug("Test:%x %x %x %x\n",data.length(),data.at(i),data.at(i+1),res);
        if(res == 0xAC){
            is_start = true;
            index = 0;
        }
        if(is_start){
            rev[index]= res;
           // qDebug("Test:%x\n",rev[index]);
            index++;
            // qDebug()<<"data:"<< rev[index]<<endl;

        }

        if(res == 0xCC ){
            is_end = true;
        }
    }
    if(is_start && is_end)
    {
 #if 0
    addSubject(0,model, "RS485测试" );
    addSubject(1,model, "看门狗" );
    addSubject(2,model, "spiflash" );
    addSubject(3,model, "蜂鸣器" );
    addSubject(4,model, "雷达测试" );
    addSubject(5,model, "ICCID" );
    addSubject(6,model, "网络质量" );
    addSubject(7,model, "蓝牙MAC" );
 #endif
       int sum = 0;

       sum = crc(rev+1,3+rev[3]);
       if( sum != rev[4+rev[3]]){
            qDebug("data error: %x %x\n",sum,rev[4+rev[3]]);
            return;
       }
       if(rev[1] == 0x01 && rev[2] == 0x01){ //启动测试

            emit TestSignal(0,0);
             qDebug("wacht:====> %x \n",rev[4]);
            if( rev[4] == 0x01){ //看门狗测试正常
                emit TestSignal(1,0);
            }

       }
       else if(rev[1] == 0x02 && rev[2] == 0x02){ //spi
                if( rev[4] == 0x01 )
                    emit TestSignal(2,0);
                else
                    emit TestSignal(2,1);
       }
       else if(rev[1] == 0x02 && rev[2] == 0x04){ //雷达
           long data = (((rev[4]<<24)&0xff0000)|((rev[5]<<16)&0x00ff0000)|((rev[6]<<8)&0x0000ff00)|(rev[7]&0x0000ff));
            qDebug("radar:%x %x %x %x\n",rev[4],rev[5],rev[6],rev[7]);
            emit TestSignal(4,data);
       }
       else if(rev[1] == 0x01 && rev[2] == 0x02){ //网络质量
            if( rev[4] == 0x01){ //联网中
                emit TestSignal(6,1);
            }
            else if( rev[4] == 0x02){ //成功
                emit TestSignal(61,rev[6]);
            }
            else if( rev[4] == 0x03){ //失败
                emit TestSignal(62,rev[5]);
            }

       }


       else if(rev[1] == 0x02 && rev[2] == 0x06){ //iccid

            QByteArray byte;
            byte = QByteArray((char *)rev+3,rev[3]);

           emit stringSignal(5,byte);
       }
       else if(rev[1] == 0x03 && rev[2] == 0x03){ //mac
            QByteArray byte;
            byte = QByteArray((char *)rev+3,rev[3]);
           emit stringSignal(7,byte);
       }
       qDebug("re:%x %x\n",rev[1],rev[2]);
        memset(rev,0,sizeof (rev));
    }
}
void Console::putData(const QByteArray &data)
{
  //  emit getData(data);
    // qDebug()<<"hello hhh "<<data<<"!"<<endl;
    this->handle_data(data);
}
