#include "atisensor.h"
#include "algorithm"
#include "stdlib.h"
#include "cstdio"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <math.h>

#define MAXFORCEFREQUENCY 12000
int32 CVICALLBACK FORCESIGNAL(TaskHandle handle, int32, uInt32 buffer, void* data);
Calibration* AtiSensor::m_calibrate = NULL; //initial static member

AtiSensor::AtiSensor()
{
    memset(m_protocol, 0, PROTOCOLS*sizeof(float));
    m_systemInitial = false;
    m_calInitial = false;
    m_protocol[atiprotocol::state] = -1.f;
    m_protocol[atiprotocol::save] = 0.f;
    m_protocol[atiprotocol::bias] = 1.f;
    m_protocol[atiprotocol::chans] = 6.f;
    m_handle = NULL;
    m_minVoltage = -10 * GAUGE_SATURATION_LEVEL;
    m_maxVoltage = 10 * GAUGE_SATURATION_LEVEL;
    m_frequency = 6 * 200; //maximum frequency: 6 * MAXFORCEFREQUENCY Hz
    m_buffer = 10;
    m_error = 0;
    m_force = 0;
    m_torque = 0;
}

std::string AtiSensor::_int2string(const int &n)
{
    std::stringstream newstr;
    newstr << n;
    return newstr.str();
}

std::string AtiSensor::_float2string(const float &f)
{
    std::stringstream newstr;
    newstr << f;
    return newstr.str();
}

bool AtiSensor::ATI_createTextFile()
{
    std::ofstream file;
    std::string filename("ATI_RAINS_DATA.txt");
    file.open(filename.c_str(), std::ios::out | std::ios::app);
    if (!file.is_open())
        return false;
    file.close();
    return true;
}

bool AtiSensor::ATI_loadCalibrateFile(std::string &path)
{
    if (path == "") return false;
    destroyCalibration(m_calibrate);
	char *p = const_cast<char *>(path.c_str());
    m_calibrate = createCalibration(p, 1);
    if (m_calibrate == NULL)
        return false;
    SetForceUnits(m_calibrate, "N");
    SetTorqueUnits(m_calibrate, "N-m");
    SetTempComp(m_calibrate, 1);

    m_minVoltage = 0;
    m_maxVoltage = 20.0;
 
    m_minVoltage -= 20.0 / 2.;
    m_maxVoltage -= 20.0 / 2.;

    m_calInitial = true;
    return true;
}

std::string AtiSensor::ATI_showCalibrateFileInfomation()
{
    std::string info("CALIBRATION INFORMATION:\n");
    std::string temp = m_calibrate->TempCompAvailable ? "YES" : "NO";
    if (!m_calInitial)
    {
        info += "No information availiable!";
        return info;
    }

    info += "Serial: " + std::string(m_calibrate->Serial) + "\n";
    info += "BodyStyle: " + std::string(m_calibrate->BodyStyle) + "\n";
    info += "PartNumber: " + std::string(m_calibrate->PartNumber) + "\n";
    info += "CalibrationData: " + std::string(m_calibrate->CalDate) + "\n";
    info += "TempCompensationSupport: " + temp + "\n";
    if (m_calibrate->rt.NumAxes == 6)
    {
        info += "MaxForce(N): +-" + _float2string(m_calibrate->MaxLoads[0]);
        info += ", +-" + _float2string(m_calibrate->MaxLoads[1]);
        info += ", +-" + _float2string(m_calibrate->MaxLoads[2]) + "\n";
        info += "MaxTorque(Nm): +-" + _float2string(m_calibrate->MaxLoads[3]/1000.f);
        info += ", +-" + _float2string(m_calibrate->MaxLoads[4]/1000.f);
        info += ", +-" + _float2string(m_calibrate->MaxLoads[5]/1000.f) + "\n";
    }
    info += "ATIConfigVoltageRange(V): " + _float2string((float)m_minVoltage);
    info += ", " + _float2string((float)m_maxVoltage);
    m_minVoltage *= GAUGE_SATURATION_LEVEL;
    m_maxVoltage *= GAUGE_SATURATION_LEVEL;
    return info;
}

void AtiSensor::ATI_releaseCalibrateFile()
{
    destroyCalibration(m_calibrate);
    m_calibrate = NULL;
    m_calInitial = false;
}

void AtiSensor::ATI_getAllAvailiableDevices(std::string &list)
{
    char devnames[1024];
    DAQmxGetSysDevNames(devnames, 1024);
    list = std::string(devnames);
}

void AtiSensor::ATI_configVoltageLimits(double minvol, double maxvol)
{
    m_minVoltage = (float64)std::min(minvol, maxvol);
    m_maxVoltage = (float64)std::max(minvol, maxvol);
    m_minVoltage *= GAUGE_SATURATION_LEVEL;
    m_maxVoltage *= GAUGE_SATURATION_LEVEL;
}

void AtiSensor::ATI_configSingleChannelFrequency(unsigned int freq)
{
    m_frequency = float64(6 * freq);
    if (m_frequency > MAXFORCEFREQUENCY)
        m_frequency = MAXFORCEFREQUENCY;
}

void AtiSensor::ATI_configBufferSize(unsigned int size)
{
    size = size < 6 ? 6 : size;
    m_buffer = (uInt32)(size+size % 6); //FX,FY,FZ,MX,MY,MZ
}

bool AtiSensor::ATI_configNIDevice(std::string device, bool save)
{
    ATI_clearTask();
    if (!m_calInitial) return false;
    std::string tn = device + "/ai0:5";
    m_protocol[atiprotocol::save] = save ? 1.f : 0.f;
    ERRCHECK(DAQmxSelfTestDevice(device.c_str()));
    ERRCHECK(DAQmxCreateTask("SEN_RAINS_ATI", &m_handle)); //create a task handle
    ERRCHECK(DAQmxCreateAIVoltageChan(m_handle, tn.c_str(), "", DAQmx_Val_RSE, m_minVoltage,
                                      m_maxVoltage, DAQmx_Val_Volts, NULL));
    ERRCHECK(DAQmxCfgSampClkTiming(m_handle, NULL, m_frequency, DAQmx_Val_Rising,
                                   DAQmx_Val_ContSamps, m_buffer));
    ERRCHECK(DAQmxCfgInputBuffer(m_handle, 3 * m_buffer)); //3 times of buffer
    ERRCHECK(DAQmxRegisterEveryNSamplesEvent(m_handle, DAQmx_Val_Acquired_Into_Buffer,
                                             m_buffer, 0, FORCESIGNAL, &m_protocol));
    m_systemInitial = true;
    return true;
}

bool AtiSensor::ATI_startTask()
{
    if (!m_systemInitial)
        return false;
    ERRCHECK(DAQmxStartTask(m_handle));
    m_protocol[atiprotocol::state] = 0.f;
    return true;
}

bool AtiSensor::ATI_suspendTask()
{
    if (!m_systemInitial)
        return false;
    ERRCHECK(DAQmxStopTask(m_handle));
    m_protocol[atiprotocol::state] = -1.f;
    return true;
}

bool AtiSensor::ATI_clearTask()
{
    if (!m_systemInitial)
        return false;
    ERRCHECK(DAQmxStopTask(m_handle));
    ERRCHECK(DAQmxClearTask(m_handle));
    memset(m_protocol, 0, PROTOCOLS*sizeof(float));
    m_protocol[atiprotocol::state] = -1.f;
    m_protocol[atiprotocol::bias] = 1.f;
    m_protocol[atiprotocol::chans] = 6.f;
    m_systemInitial = false;
    m_handle = NULL;
    m_error = 0;
    return true;
}

bool AtiSensor::ATI_getBiasState()
{
    return m_protocol[atiprotocol::bias] ? false : true;
}

int AtiSensor::ATI_checkTaskRunningState()
{
    return (int)m_protocol[atiprotocol::state];
}

void AtiSensor::ATI_recordData(bool record)
{
    m_protocol[atiprotocol::save] = record ? 1.f : 0.f;
}

float AtiSensor::ATI_readForceData(const unsigned int &chans)
{
    if (m_protocol[atiprotocol::state] || chans > 7)
        return 0.f;
    return m_protocol[atiprotocol::FX+chans];
}

std::string AtiSensor::ATI_analyzeError()
{
    if (m_error)
    {
        char error[2048];
        DAQmxGetExtendedErrorInfo(error, 2048);
        std::string info(error);
        return info;
    }

    std::string normal;
    int freq = int(m_frequency / 6);
    int mval = int(m_buffer / 6);
    normal = "Normal state: "+_int2string(freq)+"HZ, "
            +_int2string(mval)+"BIT (Single Channel)";
    return normal;
}

bool AtiSensor::ATI_getCurrentBiasVoltage(float *voltage)
{
    memset(voltage, 0, 6*sizeof(float));
    if (m_calibrate == NULL)
        return false;
    if (!m_calibrate->TempCompAvailable)
        for (int i = 0; i < 6; ++i)
            voltage[i] = m_calibrate->rt.bias_vector[i];
    else
        for (int i = 0; i < 6; ++i)
            voltage[i] = m_calibrate->rt.TCbias_vector[i];
    return true;
}

bool AtiSensor::ATI_getForceSensorRange(float *force)
{
    memset(force, 0, 6*sizeof(float));
    if (m_calibrate == NULL)
        return false;
    for (int i = 0; i < 3; ++i)
        force[i] = m_calibrate->MaxLoads[i];
    for (int i = 3; i < 6; ++i)
        force[i] = m_calibrate->MaxLoads[i]/1000.f;
    return true;
}

void AtiSensor::ATI_convertVoltageToForce(float *voltage, float *force)
{
    if (voltage == NULL || force == NULL)
        return;
    if (m_calibrate == NULL)
        return;
    ConvertToFT(m_calibrate, voltage, force);
}

int32 CVICALLBACK FORCESIGNAL(TaskHandle handle, int32, uInt32 buffer, void* data)
{
    int32 count(0);
    float protocol[PROTOCOLS]; //[state,save,bias,FX,FY,FZ,MX,MY,MZ,F,M]
    memcpy(protocol, data, PROTOCOLS*sizeof(float));
    uInt32 single = buffer / 6;
    float64 *db = new float64[buffer];
    DAQmxReadAnalogF64(handle, single, 10.0, DAQmx_Val_GroupByScanNumber,
                       db, buffer, &count, NULL);

    float voltage[6], force[6]; //convert voltage to force
    for (int i = 0; i < (int)single; ++i)
    {
        for (int j = 0; j < 6; ++j)
            voltage[j] = float(db[6*i+j]);
        if (protocol[atiprotocol::bias])
        {
            Bias(AtiSensor::m_calibrate, voltage);
            protocol[atiprotocol::bias] = 0;
        }
        ConvertToFT(AtiSensor::m_calibrate, voltage, force);
        for (int j = 0; j < 6; ++j)
            db[6*i+j] = force[j];
    }

    double mval[6]; //calculate the mean value
    memset(mval, 0, 6*sizeof(double));
    for (int i = 0; i < (int)single; ++i)
        for (int j = 0; j < 6; ++j)
            mval[j] += db[6*i+j];
    for (int i = 0; i < 6; ++i)
        protocol[atiprotocol::FX+i] = float(mval[i] / single);   //save average force to atiprotocol

    float mF(0), mM(0);   //calculate the total force and torque
    for (int i = 0; i < 3; ++i)
    {
       mF += std::pow(protocol[atiprotocol::FX+i], 2);
       mM += std::pow(protocol[atiprotocol::MX+i], 2);
    }
    protocol[atiprotocol::F] = std::sqrt(mF);
    protocol[atiprotocol::M] = std::sqrt(mM);
    memcpy(data, protocol, PROTOCOLS*sizeof(float));

    if (protocol[atiprotocol::save]) //save the data
    {
//        char devname[64];
//        std::ofstream file;
//        DAQmxGetTaskDevices(handle, devname, 64);
//        std::string filename(devname);
//        filename += "_RAINS_DATA.txt";
        std::ofstream file;
        std::string filename("ATI_RAINS_DATA.txt");
        file.open(filename.c_str(), std::ios::out | std::ios::app);
        if (!file.is_open())
        {
            DAQmxStopTask(handle);
            DAQmxClearTask(handle);
            protocol[atiprotocol::state] = 2; //file not open state
            delete db;
            return 2;
        }
        file.precision(6);
        file.setf(std::ios::right); //align right
        file.setf(std::ios::fixed); //ios::scientific
        for (int i = 0; i < (int)single; ++i)
        {
            file << db[6*i];
            for (int j = 1; j < 6; ++j)
            {
                file << '\t' << db[6*i+j];
				std::cout << db[6 * i + j] << std::endl;
            }

            //jzl added timestamp
           // QString CurrentTime = QTime::currentTime().toString("hh:mm:ss:zzz");
           // unsigned char cCurrentTime[30];
           // memcpy(cCurrentTime, CurrentTime.toStdString().c_str(), CurrentTime.size());
           // file << '\t' << cCurrentTime;
            //jzl
            file << std::endl;
        }
        file.close();
    }

    protocol[atiprotocol::state] = 0; //file not open state
    delete db;
    return 0;
}
