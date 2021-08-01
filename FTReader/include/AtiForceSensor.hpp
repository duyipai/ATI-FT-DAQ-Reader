#pragma once
#include "SensorHandler.hpp"
#include "NIDAQmx.h"
#include "aticonvert.h"
#include "atisensor.h"
#include <iostream>

class AtiForceSensor : public SensorHandler
{
public: 
	AtiForceSensor(std::string cal_path) : SensorHandler(cal_path) { 
		m_ati = new AtiSensor; 
	};

	~AtiForceSensor() override;

	int initSensor() override;
	int getData(std::vector<double>& data) override;
	int getData(double* data) override;
	void printCalInfo() override;

protected:
	AtiSensor* m_ati;
};

