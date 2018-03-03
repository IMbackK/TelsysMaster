#ifndef UTILITYS_H
#define UTILITYS_H

#include <QString>
#include <stdint.h>
#include <array>


#include "sampleparser.h"
#include "bleserial.h"

bool saveToCsv(const QString& filename, const std::vector<AdcSample>* adcSampels, const std::vector<AuxSample>* auxSampels );

bool saveToCsv(const QString& filename, const std::vector<double> &keys, const std::vector<double> &values, const QString& keyLable, const QString& valueLable);

void sendStart(BleSerial* bleSerial);

void sendStop(BleSerial* bleSerial);

void sendReset(BleSerial* bleSerial);

void sendOfset(BleSerial* bleSerial);

void sendRate(BleSerial* bleSerial, uint16_t rate);

void sendCalValues(BleSerial* bleSerial, std::array<double, 10> in);


#endif // UTILITYS_H
