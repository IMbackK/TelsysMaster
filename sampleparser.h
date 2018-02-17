#ifndef SAMPLEPARSER_H
#define SAMPLEPARSER_H

#include <QObject>
#include <QString>

#include <stdint.h>

#include "point3D.h"

class AdcSample
{
public:
    uint16_t value;
    uint16_t deltaTime;
    uint64_t timeStamp;
    uint64_t id;
};

class AuxSample
{
public:
    Point3D<int16_t> accel;
    Point3D<int16_t> magn;
    uint8_t temperature;
    uint64_t id;
};

class SampleParser : public QObject
{
    Q_OBJECT
private:

    uint64_t timeStampHead = 0;

    //unsigned long _paketNumber = 0;
    unsigned _currentPaketType = 0;
    int _expectedCount = -1;
    uint_fast8_t _totalCount = 0;
    uint_fast16_t _currentDelta = 0;

    double _offset = 1.0;

    uint64_t _currentAdcSampleId = 0;
    uint64_t _currentAuxSampleId = 0;

    unsigned sampleCountLimit = 100000;

public:

    std::vector<AdcSample> adcSamples;
    std::vector<AuxSample> auxSamples;

public:
    explicit SampleParser(QObject *parent = nullptr);

signals:
    void gotAdcSample(AdcSample sample, int amountNowStored);
    void gotAuxSample(AuxSample sample, int amountNowStored);

public slots:
    void newData(const uint8_t* data, size_t length);
    void clear();
    void setOffset(double offset);
    void resendRange(unsigned int from, unsigned int to);
    void saveCsv(QString fileName);
    void loadCsv(QString fileName);

private:
    static uint32_t toEquivalentUint32(const uint8_t* data);
    static uint16_t toEquivalentUint16(const uint8_t* data);
    void decodeAdcData(const uint8_t *data, size_t length);
    void decodeAuxData(const uint8_t *data, size_t length);
};

#endif // SAMPLEPARSER_H