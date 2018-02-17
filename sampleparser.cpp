#include "sampleparser.h"

#include <vector>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QCoreApplication>

#define SAMPLE_PARSER_NO_PAKET  0
#define SAMPLE_PARSER_ADC_PAKET 1
#define SAMPLE_PARSER_AUX_PAKET 2
#define SAMPLE_PARSER_ADC_PAKET_SIGNATURE 0x4587AD75
#define SAMPLE_PARSER_AUX_PAKET_SIGNATURE 0x4587AD76
#define SAMPLE_PARSER_PAKET_END_SIGNATURE 0xFFFF

SampleParser::SampleParser(QObject *parent) : QObject(parent){}


uint32_t SampleParser::toEquivalentUint32(const uint8_t *data)
{
    uint32_t result = data[3];
    result = result | (data[2] << 8);
    result = result | (data[1] << 16);
    result = result | (data[0] << 24);
    return result;
}

uint16_t SampleParser::toEquivalentUint16(const uint8_t *data)
{
    uint16_t result = data[1];
    result = result | (data[0] << 8);
    return result;
}

void SampleParser::decodeAuxData(const uint8_t *data, size_t length)
{

}

void SampleParser::resendRange(unsigned int from, unsigned int to)
{
    for(size_t i = from; i < to; i++)gotAdcSample(adcSamples[i], adcSamples.size());
}

void SampleParser::decodeAdcData(const uint8_t *data, size_t length)
{
    if(_expectedCount < 0 && length > 3)
    {
        _expectedCount = data[0];
        _totalCount = _expectedCount;
        _currentDelta = toEquivalentUint16(data+1);
        qDebug()<<data[0]<<','<<data[1]<<','<<data[2];
    }
    for(size_t i = 3; i < length; i+=2)
    {
        if( i + 2 < length && _expectedCount > 0)
        {
            AdcSample tmp;
            tmp.value = toEquivalentUint16(data+i);
            tmp.deltaTime = _currentDelta/_totalCount;
            tmp.id = _currentAdcSampleId;

            tmp.timeStamp=timeStampHead + tmp.deltaTime*(_totalCount-_expectedCount);
            tmp.value = tmp.value*_offset;

            if(adcSamples.size() > sampleCountLimit) adcSamples.erase(adcSamples.begin()); //limit samples
            adcSamples.push_back(tmp);
            gotAdcSample(tmp, adcSamples.size());

            _expectedCount--;
            _currentAdcSampleId++;
        }
        else if(i + 1 < length && data[i] == 0xFF &&  _expectedCount == 0)
        {
            _currentPaketType = SAMPLE_PARSER_NO_PAKET;
            timeStampHead = timeStampHead+_currentDelta;
            _totalCount = 0;
            _currentDelta = 0;
            _expectedCount = -1;
        }
        qDebug()<<length<<i<<','<<data[i];
    }
}

void SampleParser::newData(const uint8_t *data, size_t length)
{
    if(length > 4)
    {
        size_t i = 0;
        while(i < length-4 && _currentPaketType == SAMPLE_PARSER_NO_PAKET)
        {
            uint32_t currentSignature = toEquivalentUint32(data+i);
                 if( currentSignature == SAMPLE_PARSER_ADC_PAKET_SIGNATURE ) _currentPaketType = SAMPLE_PARSER_ADC_PAKET_SIGNATURE;
            else if( currentSignature == SAMPLE_PARSER_AUX_PAKET_SIGNATURE ) _currentPaketType = SAMPLE_PARSER_AUX_PAKET_SIGNATURE;
            i++;
        }
        i--;
        if( length-i-4 > 2 )
        {
            if(_currentPaketType == SAMPLE_PARSER_ADC_PAKET_SIGNATURE) decodeAdcData(data+i+4, length-i-4);
            //else if(_currentPaketType == SAMPLE_PARSER_ADC_PAKET_SIGNATURE);
        }
    }
}

void SampleParser::setOffset(double offset)
{
    for(unsigned int i = 0; i < adcSamples.size(); i++)
    {
        adcSamples[i].value = (adcSamples[i].value/_offset)*offset;
    }
    _offset = offset;
}

void SampleParser::saveCsv(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream fileStream(&file);
        fileStream<<"ID,TIMESTAMP,ADCVALUE,,ID,TIMESTAMP,ACC X,ACC Y,ACC Z,MGN X,MGN Y, MGN Z,TEMP\n";
        for(unsigned int i = 0; i < adcSamples.size(); i++)
        {
            fileStream<<adcSamples[i].id<<','<<adcSamples[i].timeStamp<<','<<adcSamples[i].value<<",,";
            fileStream<<",,,,,,,\n";
        }
        fileStream.flush();
        file.close();
    }
}

void SampleParser::loadCsv(QString fileName)
{
    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        clear();
        QTextStream fileStream(&file);
        fileStream.readLine();
        unsigned int count = 0;
        while(!fileStream.atEnd())
        {
            QStringList currentlineTokens = fileStream.readLine().split(',');
            if(currentlineTokens.size() >= 12)
            {

                AdcSample tmp;
                tmp.id = currentlineTokens[0].toInt();
                tmp.timeStamp = currentlineTokens[1].toInt();
                tmp.value = currentlineTokens[2].toInt();
                adcSamples.size() == 0 ? tmp.deltaTime = tmp.timeStamp : tmp.deltaTime = tmp.timeStamp - adcSamples.back().timeStamp;
                adcSamples.push_back(tmp);
                gotAdcSample(tmp, adcSamples.size());
            }
            if(count % 1024 == 0)QCoreApplication::processEvents();
            count++;
        }
        file.close();
    }
}

void SampleParser::clear()
{
    adcSamples.clear();
    auxSamples.clear();
    _currentAdcSampleId = 0;
    timeStampHead = 0;
}


