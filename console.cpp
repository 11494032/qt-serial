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

int numtoAscii( char data ){
    int ret = 0;
//    qDebug()<<""<<data;
    if( data >=0 && data <=9 )
        ret = data+'0';
    else if (data >=10 && data <=15) {
        ret = data+'A'-10;
    }

  //   qDebug()<<"----"<<ret;
    return ret;
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

            emit TestSignal(99,1);
             qDebug("wacht:====> %x \n",rev[4]);
            if( rev[4] == 0x01){ //看门狗测试正常
                emit TestSignal(1,1);
            }

       }
       else if(rev[1] == 0x02 && rev[2] == 0x02){ //spi
                if( rev[4] == 0x01 )
                    emit TestSignal(2,1);
                else
                    emit TestSignal(2,2);
       }
       else if(rev[1] == 0x02 && rev[2] == 0x04){ //雷达
           long data = (((rev[4]<<24)&0xff0000)|((rev[5]<<16)&0x00ff0000)|((rev[6]<<8)&0x0000ff00)|(rev[7]&0x0000ff));
            qDebug("radar:%lx\n",data);
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
           byte = QByteArray((char *)rev+4,rev[3]);
           emit stringSignal(5,byte);
       }
       else if(rev[1] == 0x03 && rev[2] == 0x03){ //mac
           char  byte[24] ={0,};
            for( int i = 0; i<rev[3]; i++ ){
                byte[3*i] = numtoAscii(( rev[4+i])>>4);
                byte[3*i+1] = numtoAscii(( rev[4+i])&0x0f);
                if( i != rev[3]-1 )
                    byte[3*i+2] = '-';
               // qDebug("mac:%d %d\n", byte[2*i],byte[2*i+1]);
            }
            emit stringSignal(7, byte);
       }
       else if(rev[1] == 0x02 && rev[2] == 0x09){ //write sn
           if( rev[4] == 0x01){ //success
               emit TestSignal(40,1);
           }
           else if( rev[4] == 0x02){ //failed
               emit TestSignal(40,2);
           }
       }
       else if(rev[1] == 0x03 && rev[2] == 0x01){ //read sn
           QByteArray byte;
           byte = QByteArray((char *)rev+4,rev[3]);
           //char sn[32] = {0,};
           //memcpy(sn,rev+4,rev[3]);
           emit stringSignal(41,byte);

       }
       else if(rev[1] == 0x02 && rev[2] == 0x07){ //WRITE imei
           if( rev[4] == 0x01){ //success
               emit TestSignal(42,1);
           }
           else if( rev[4] == 0x02){ //failed
               emit TestSignal(42,2);
           }
       }
       else if(rev[1] == 0x02 && rev[2] == 0x0A){ //WRITE MODE
           if( rev[4] == 0x01){ //success
               emit TestSignal(43,1);
           }
           else if( rev[4] == 0x02){ //failed
               emit TestSignal(43,2);
           }
       }
       else if(rev[1] == 0x03 && rev[2] == 0x02){ //READ VERSION
           if( rev[4] == 0x01){ //success
                char sbuf[24] = {0,};
                sbuf[0] =  numtoAscii(( rev[4])>>4);
                sbuf[1] = numtoAscii(( rev[4])&0x0f);
                sbuf[2] = numtoAscii(( rev[5])>>4);
                sbuf[3] = numtoAscii(( rev[5])&0x0f);
                sbuf[4] = numtoAscii(( rev[6])>>4);
                sbuf[5] = numtoAscii(( rev[6])&0x0f);
                sbuf[6] = numtoAscii(( rev[7])>>4);
                sbuf[7] = numtoAscii(( rev[7])&0x0f);
                sbuf[8] = numtoAscii(( rev[8])>>4);
                sbuf[9] = numtoAscii(( rev[8])&0x0f);
                sbuf[10] = numtoAscii(( rev[9])>>4);
                sbuf[11] = numtoAscii(( rev[9])&0x0f);
                sbuf[12] = numtoAscii(( rev[10])>>4);
                sbuf[13] = numtoAscii(( rev[11])&0x0f);
                emit stringSignal(44,sbuf);
           }
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
