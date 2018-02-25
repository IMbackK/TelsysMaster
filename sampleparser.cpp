#include "sampleparser.h"

#include <vector>
#include <QDebug>
#include <QTextStream>
#include <QFile>
#include <QStringList>
#include <QCoreApplication>

#include "utilitys.h"

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
    if(adcSampleCallback) adcSampleCallback(adcSamples.begin()+from, adcSamples.begin()+to, adcSamples.size(), true);
}

void SampleParser::decodeAdcData(const uint8_t *data, size_t length)
{
    if(length > 3)
    {
        int expectedCount = data[0];
        uint_fast8_t totalCount = expectedCount;
        uint16_t currentDelta = toEquivalentUint16(data+1);

        for(size_t i = 3; i + 2 <= length && expectedCount > 0; i+=2)
        {
            AdcSample tmp;
            tmp.value = toEquivalentUint16(data+i);
            tmp.deltaTime = currentDelta;
            tmp.id = _currentAdcSampleId;

            tmp.timeStamp=timeStampHead + tmp.deltaTime*(totalCount-expectedCount);
            tmp.value = tmp.value*_offset;

            if(adcSamples.size() > sampleCountLimit-1) adcSamples.erase(adcSamples.begin()); //limit samples
            adcSamples.push_back(tmp);

            expectedCount--;
            _currentAdcSampleId++;
        }
        if(adcSampleCallback) adcSampleCallback(adcSamples.end()-totalCount+expectedCount, adcSamples.end(), adcSamples.size(), false);
        timeStampHead = timeStampHead+currentDelta*totalCount;
        totalCount = 0;
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
    saveToCsv(fileName, &adcSamples, &auxSamples);
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
            if(currentlineTokens.size() == 2)
            {
                AdcSample tmp;
                tmp.id = count;
                tmp.timeStamp = currentlineTokens[0].toULong();
                tmp.value = currentlineTokens[1].toULong();
                adcSamples.size() == 0 ? tmp.deltaTime = tmp.timeStamp : tmp.deltaTime = tmp.timeStamp - adcSamples.back().timeStamp;
                adcSamples.push_back(tmp);
            }
            else if(currentlineTokens.size() >= 3)
            {
                AdcSample tmp;
                tmp.id = currentlineTokens[0].toULong();
                tmp.timeStamp = currentlineTokens[1].toULong();
                tmp.value = currentlineTokens[2].toULong();
                adcSamples.size() == 0 ? tmp.deltaTime = tmp.timeStamp : tmp.deltaTime = tmp.timeStamp - adcSamples.back().timeStamp;
                adcSamples.push_back(tmp);
            }
            if(count % 8192 == 0)QCoreApplication::processEvents();
            count++;
        }
        file.close();
        if(adcSamples.size() > 0 && adcSampleCallback)adcSampleCallback(adcSamples.begin(), adcSamples.end(), adcSamples.size(), true);
    }
}

void SampleParser::setAdcSampleCallback(std::function<void(std::vector<AdcSample>::iterator, std::vector<AdcSample>::iterator, unsigned, bool)> cb)
{
    adcSampleCallback = cb;
}
void SampleParser::setAuxSampleCallback(std::function<void(const AuxSample&)> cb)
{
    auxSampleCallback = cb;
}

void SampleParser::setLimit(unsigned newSampleCountLimit)
{
    sampleCountLimit = newSampleCountLimit;
    while(adcSamples.size() > sampleCountLimit-1) adcSamples.erase(adcSamples.begin());
}

int SampleParser::getLimit()
{
    return sampleCountLimit;
}

void SampleParser::clear()
{
    adcSamples.clear();
    auxSamples.clear();
    _currentAdcSampleId = 0;
    timeStampHead = 0;
    adcSamples.shrink_to_fit();
    auxSamples.shrink_to_fit();
}


