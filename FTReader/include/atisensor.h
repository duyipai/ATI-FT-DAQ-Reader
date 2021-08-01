#ifndef ATISENSOR_H
#define ATISENSOR_H

#ifndef _NIRELEVENT
#define _NIRELEVENT
#define PROTOCOLS 12
#define GAUGE_SATURATION_LEVEL 0.995
#define ERRCHECK(func) if ((m_error = (func))) { return false; }
namespace myconfig {
enum _myconfig {
    Voltage=0, Frequency, Buffer}; }
#endif
namespace atiprotocol {
enum _myprotocol {
    state=0, save, bias, chans, FX, FY, FZ, MX, MY, MZ, F, M }; }

#include "sstream"
#include "string"
#include "vector"
#include <windows.h>
#include "NIDAQmx.h"
#include "aticonvert.h"

#undef max //avoid the conflict with windows.h
#undef min

class AtiSensor
{
public:
    AtiSensor();

public:
    bool ATI_createTextFile();
    bool ATI_loadCalibrateFile(std::string &path);
    std::string ATI_showCalibrateFileInfomation();
    void ATI_releaseCalibrateFile();
    void ATI_getAllAvailiableDevices(std::string &list);
    void ATI_configVoltageLimits(double minvol, double maxvol);
    void ATI_configSingleChannelFrequency(unsigned int freq);
    void ATI_configBufferSize(unsigned int size);
    bool ATI_configNIDevice(std::string device, bool save);
    bool ATI_startTask();
    bool ATI_suspendTask();
    bool ATI_clearTask();
    bool ATI_getBiasState();
    int ATI_checkTaskRunningState();
    void ATI_recordData(bool record = false);
    float ATI_readForceData(const unsigned int &chans);
    std::string ATI_analyzeError();
    bool ATI_getCurrentBiasVoltage(float *voltage);
    bool ATI_getForceSensorRange(float *force);
    void ATI_convertVoltageToForce(float *voltage, float *force);

public:
    static Calibration* m_calibrate;
    inline double ATI_getCurrentMinVoltage() {
        return (double)m_minVoltage; }
    inline double ATI_getCurrentMaxVoltage() {
        return (double)m_maxVoltage; }
    inline int ATI_getCurrentSamplingFrequency() {
        return (int)m_frequency; }
    inline int ATI_getCurrentBufferSize() {
        return (int)m_buffer; }

private:
    std::string _int2string(const int &n);
    std::string _float2string(const float &f);

private:
    bool m_calInitial;
    bool m_systemInitial;
    float m_protocol[PROTOCOLS]; //[state,save,bias,FX,FY,FZ,MX,MY,MZ]  (averaged data)
    TaskHandle m_handle; //handle of task
    float64 m_minVoltage, m_maxVoltage;
    float64 m_frequency;
    uInt32 m_buffer;
    int32 m_error;
    float64 m_force, m_torque;

};

#endif // ATISENSOR_H
