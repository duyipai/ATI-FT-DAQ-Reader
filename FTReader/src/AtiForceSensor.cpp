#include "AtiForceSensor.hpp"

AtiForceSensor::~AtiForceSensor()
{
	delete m_ati;
}

int AtiForceSensor::initSensor()
{
	if (!m_ati->ATI_loadCalibrateFile(m_calPath))
	{
		std::cout << "ATI Sensor: Failed to Load Calibration" << std::endl;
		return -1;
	}

	// Setting the streaming frequency
	m_ati->ATI_configSingleChannelFrequency(m_freq);

	// setting the device
	if (!m_ati->ATI_configNIDevice("dev1", false))
	{
		std::cout << "ATI Sensor: Failed to configure NI device" << std::endl;
		return -1;
	}

	// Setting the buffer size
	unsigned int m_size = 6;
	m_ati->ATI_configBufferSize(m_size);

	// Starting the acquisition
	if (!m_ati->ATI_startTask())
	{
		std::cout << "ATI Sensor: Failed to start task" << std::endl;
		return -1;
	}

	return 0;
};

int AtiForceSensor::getData(std::vector<double>& data)
{
	data.resize(6);

	data[0] = m_ati->ATI_readForceData(0);
	data[1] = m_ati->ATI_readForceData(1);
	data[2] = m_ati->ATI_readForceData(2);
	data[3] = m_ati->ATI_readForceData(3);
	data[4] = m_ati->ATI_readForceData(4);
	data[5] = m_ati->ATI_readForceData(5);

	return 0;
}

// dangerous function - data array has to be allocated correctly before being passed to the function
int AtiForceSensor::getData(double* data)
{
	data[0] = m_ati->ATI_readForceData(0);
	data[1] = m_ati->ATI_readForceData(1);
	data[2] = m_ati->ATI_readForceData(2);
	data[3] = m_ati->ATI_readForceData(3);
	data[4] = m_ati->ATI_readForceData(4);
	data[5] = m_ati->ATI_readForceData(5);

	return 0;
}

void AtiForceSensor::printCalInfo()
{
	std::string info = m_ati->ATI_showCalibrateFileInfomation();
	std::cout << info << std::endl;
}
