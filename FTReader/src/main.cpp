#pragma once
#include <iostream>
#include <fstream>
#include <sys/timeb.h>
#include <time.h>
#include <string>
#include "nlohmann/json.hpp"
#include "Config.hpp"
#include "SensorHandler.hpp"
#include "AtiForceSensor.hpp"

int parse_config(Config &config)
{
	std::string configDir = std::string("config.json");

	try
	{
		std::ifstream fid(configDir);

		json j;
		fid >> j;

		config = j;
	}
	catch (std::exception &e)
	{
		std::cout << "Failed to load config file: " << e.what() << std::endl;
		return -1;
	}
	return 0;
};

int main()
{
	Config config;
	if (parse_config(config) < 0)
	{
		char a;
		std::cout << "Press any key to exit ... ";
		std::cin >> a;
		return -1;
	}

	std::shared_ptr<SensorHandler> sensor;

	if ((config.sensor.compare("FT_ATI")) == 0)
	{
		sensor.reset(new AtiForceSensor(config.calFilePath));
	}
	else
	{
		std::cout << "Unknown sensor" << std::endl;

		char a;
		std::cout << "Press any key to exit ... ";
		std::cin >> a;
		return -1;
	}

	std::cout << "setting frequency: " << config.freq << std::endl;
	sensor->setFrequency(config.freq * 2);
	if (sensor->initSensor() < 0)
	{
		std::cout << "Unable to initialize sensor" << std::endl;

		char a;
		std::cout << "Press any key to exit ... ";
		std::cin >> a;
		return -1;
	}
	sensor->printCalInfo();

	while (true)
	{
		double data[6];
		sensor->getData(data);
		// Do anything you want with data here
		std::cout << data[0] << data[1] << data[2] << data[3] << data[4] << data[5] << std::endl;
		Sleep(1000 / (config.freq));
	}
	return 0;
}