#ifndef REGESSIONCALCULATOR_H
#define REGESSIONCALCULATOR_H

#include <vector>

class RegessionCalculator
{
public:
    std::vector<double> xValues;
    std::vector<double> yValues;

    double slope;
    double offset;
    double stdError;

    RegessionCalculator(const std::vector<double>& xValues, const std::vector<double>& yValues);
};

#endif // REGESSIONCALCULATOR_H
