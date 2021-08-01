#include "SensorHandler.hpp"

int SensorHandler::initSensor()
{
	return -1;
}

void SensorHandler::setFrequency(int freq) {
	m_freq = freq;
}

int SensorHandler::getData(std::vector<double>& data) {
	return -1;
}

int SensorHandler::getData(double* data) {
	return -1;
}

void SensorHandler::printCalInfo()
{}
