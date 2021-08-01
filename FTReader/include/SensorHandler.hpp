#pragma once
#include <string>
#include <vector>

class SensorHandler
{
public:
	SensorHandler(std::string calPath) : m_calPath(calPath){};
	virtual ~SensorHandler(){};

	void setFrequency(int freq);

	virtual int initSensor();

	virtual int getData(std::vector<double> &data);

	virtual int getData(double *data);

	virtual void printCalInfo();

protected:
	std::string m_calPath = "FT27374.cal";
	unsigned int m_freq = 1;
};
