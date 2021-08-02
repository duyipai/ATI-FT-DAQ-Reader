# ATI FT DAQ Reader
This is a simple C++ implementation of reading data from an ATI DAQ Force Sensor device. It uses ATIDAQ C Library to manage the force sensor control and data read. Tested with ATI Nano 17 DAQ.

## Requirements
- NI-DAQmx drivers (https://www.ni.com/de-de/support/downloads/drivers/download.ni-daqmx.html#333268)
- ATIDAQ C Library (https://www.ati-ia.com/Products/ft/software/daq_software.aspx)
- Visual Studio (tested with Visual Studio 2017 x86 compiler)

### Usage
From the cmake GUI: 

- Run configure (Select Win32 platform for the project generator)
- Add the path to the ATIDAQ C Library in **DAQmx_DIR** (This directory must contain both the .h and NIDAQmx.lib and aticonvert.lib files)
- Eventually change the **CMAKE_INSTALL_PREFIX** to the directory where you want your .exe file to be installed
- Run configure again
- Run generate
- Open project in Visual Studio
- Compile ALL_BUILD
- Compile INSTALL

In your CMAKE_INSTALL_PREFIX you should now have the .exe file together with a configuration .json file. 
Before running the program, you should copy in the folder the calibration file for your force sensor as well as the .dll of the shared libraries. In the config .json file you can set parameters as the server port and the streaming frequency. 
