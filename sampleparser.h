#ifndef SAMPLEPARSER_H
#define SAMPLEPARSER_H

#include<functional>
#include <QObject>
#include <QString>

#include <stdint.h>

#include "point3D.h"

class AdcSample
{
public:
    uint16_t value;
    uint32_t deltaTime;
    uint64_t timeStamp;
    uint64_t id;
    double scale = 1;
};

class AuxSample
{
public:
    Point3D<int16_t> accel;
    double accelScale = 1;
    Point3D<int16_t> magn;
    double magnScale = 1;
    uint8_t temperature;
    uint64_t timeStamp;
    uint64_t id;
};

class SampleParser : public QObject
{
    Q_OBJECT
private:

    uint64_t adcTimeStampHead = 0;
    uint64_t auxTimeStampHead = 0;

    double _offset = 1.0;

    uint64_t _currentAdcSampleId = 0;
    uint64_t _currentAuxSampleId = 0;

    unsigned sampleCountLimit = 100000;

    std::function<void(std::vector<AdcSample>::iterator, std::vector<AdcSample>::iterator, unsigned, bool)> adcSampleCallback;
    std::function<void(const AuxSample&)> auxSampleCallback;

public:

    std::vector<AdcSample> adcSamples;
    std::vector<AuxSample> auxSamples;

    void setAdcSampleCallback(std::function<void(std::vector<AdcSample>::iterator, std::vector<AdcSample>::iterator, unsigned, bool)> cb);
    void setAuxSampleCallback(std::function<void(const AuxSample&)> cb);

    int getLimit();

public:
    explicit SampleParser(QObject *parent = nullptr);
    void decodeAdcData(const uint8_t *data, size_t length);
    void decodeAuxData(const uint8_t *data, size_t length);

public slots:
    void clear();
    void setOffset(double offset);
    void resendRange(unsigned int from, unsigned int to);
    void saveCsv(QString fileName);
    void loadCsv(QString fileName);
    void setLimit(unsigned newSampleCountLimit);

private:
    static uint32_t uCTicksToUs(const uint16_t ticks);
    static uint32_t toEquivalentUint32(const uint8_t* data);
    static uint16_t toEquivalentUint16(const uint8_t* data);

};

#endif // SAMPLEPARSER_H
