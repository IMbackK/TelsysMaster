#include "utilitys.h"

#include <vector>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QMessageBox>
#include <thread>
#include <stdexcept>

bool saveToCsv(const QString& filename, const std::vector<AdcSample>* adcSamples, const std::vector<AuxSample>* auxSamples )
{
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text) && adcSamples != nullptr)
    {
        QTextStream fileStream(&file);
        if(auxSamples != nullptr)fileStream<<"ID,TIMESTAMP,ADCVALUE,,ID,TIMESTAMP,ACC X,ACC Y,ACC Z,MGN X,MGN Y, MGN Z,TEMP\n";
        else fileStream<<"ID,TIMESTAMP,ADCVALUE\n";
        for(unsigned int i = 0; i < adcSamples->size(); i++)
        {
            fileStream<<adcSamples->at(i).id<<','<<adcSamples->at(i).timeStamp<<','<<adcSamples->at(i).value;
            if(auxSamples != nullptr && i < auxSamples->size())
            {
                fileStream<<",,"<<auxSamples->at(i).id<<','<<auxSamples->at(i).timeStamp<<','
                          <<auxSamples->at(i).accel.x<<','<<auxSamples->at(i).accel.y<<','<<auxSamples->at(i).accel.z<<','
                          <<auxSamples->at(i).magn.x<<','<<auxSamples->at(i).magn.y<<','<<auxSamples->at(i).magn.z<<','
                          <<auxSamples->at(i).temperature<<'\n';
            }
            else if(auxSamples != nullptr) fileStream<<",,,,,,,,,\n";
            else if(auxSamples == nullptr)fileStream<<'\n';
        }
        fileStream.flush();
        file.close();
        return true;
    }
    return false;
}

bool saveToCsv(const QString& filename, const std::vector<double> &keys, const std::vector<double> &values, const QString& keyLable, const QString& valueLable )
{
    if(keys.size() != values.size())
    {
        throw std::invalid_argument("keys and values need to be the same size");
        return false;
    }
    QFile file(filename);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileStream<<"id"<<','<<keyLable<<','<<valueLable<<'\n';
        for(size_t i = 0; i < keys.size(); i++)
        {
            fileStream<<i<<','<<keys[i]<<','<<values[i]<<'\n';
        }
        fileStream.flush();
        file.close();
        return true;
    }
    return false;
}

void sendStart(BleSerial* bleSerial)
{
    uint8_t buffer[] = "on\n";
    bleSerial->write(buffer, sizeof(buffer));
}

void sendStop(BleSerial* bleSerial)
{
    uint8_t buffer[] = "off\n";
    bleSerial->write(buffer, sizeof(buffer));
}

void sendReset(BleSerial* bleSerial)
{
    uint8_t buffer[] = "rst\n";
    bleSerial->write(buffer, sizeof(buffer));
}

void sendOfset(BleSerial* bleSerial)
{
    //sendStop(bleSerial);
    uint8_t buffer[] = "ost\n";
    bleSerial->write(buffer, sizeof(buffer));
}

void sendRate(BleSerial* bleSerial, uint16_t rate)
{
    if(bleSerial->isConnected())
    {
        sendStop(bleSerial);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        uint8_t data[7];
        data[0] = 'r';
        data[1] = 'a';
        data[2] = 't';
        data[3] = (rate & 0xFF00) >> 8;
        data[4] = rate & 0x00FF;
        data[5] = '\n';
        data[6] = '\0';
        bleSerial->write(data, sizeof(data));
    }
}

void sendCalValues(BleSerial* bleSerial, std::array<double, 10> in)
{
    if(bleSerial->isConnected())
    {
        sendStop(bleSerial);
        uint8_t data[14];
        data[0] = 'c';
        data[1] = 'a';
        data[2] = 'l';

        data[13] = '\n';

        for(unsigned int i = 3; i < 13; i++) data[i] = (uint8_t)(in[i-3]*100);

        bleSerial->write(data, sizeof(data));
    }
    else
    {
        QMessageBox::information(nullptr, "Warning", "No BTLE device connected. Applying Scale only.", QMessageBox::Ok);
    }
}
