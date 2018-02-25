#ifndef UTILITYS_H
#define UTILITYS_H

#include <QString>
#include <stdint.h>
#include <array>


#include "sampleparser.h"
#include "bleserial.h"

void saveToCsv(const QString& filename, std::vector<AdcSample>* adcSampels, std::vector<AuxSample>* auxSampels );

void sendStart(BleSerial* bleSerial);

void sendStop(BleSerial* bleSerial);

void sendReset(BleSerial* bleSerial);

void sendOfset(BleSerial* bleSerial);

void sendRate(BleSerial* bleSerial, uint16_t rate);

void sendCalValues(BleSerial* bleSerial, std::array<double, 10> in);


#endif // UTILITYS_H
