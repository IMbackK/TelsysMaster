#include "regessioncalculator.h"
#include <stdexcept>
#include <math.h>
#include <QDebug>

RegessionCalculator::RegessionCalculator(const std::vector<double>& xValues, const std::vector<double>& yValues)
{
    if(xValues.size() == yValues.size())
    {
        this->xValues = xValues;
        this->yValues = yValues;

        double sumY = 0;
        double sumX = 0;
        double sumXTimesY = 0;
        double sumSquaredX = 0;
        double sumSquaredY = 0;
        for (unsigned i = 0; i < xValues.size(); i++)
        {
            sumY += yValues[i];
            sumX += xValues[i];
            sumSquaredX += xValues[i]*xValues[i];
            sumSquaredY += yValues[i]*yValues[i];
            sumXTimesY += xValues[i]*yValues[i];
        }

        slope = (sumXTimesY - (sumX*sumY)/xValues.size()) / (sumSquaredX - (sumX*sumX)/xValues.size());
        offset = sumY/xValues.size() - slope*(sumX/xValues.size());

        double error = (xValues.size()*sumSquaredY-sumY*sumY-slope*slope*(xValues.size()*sumSquaredX-sumX*sumX)) / (xValues.size()*(xValues.size()-2));
        stdError = sqrt( (error*error*xValues.size() ) / ( xValues.size()*sumSquaredX-sumX*sumX));
    }
    else throw std::invalid_argument("xValues and yValues need to be the same size");
}
