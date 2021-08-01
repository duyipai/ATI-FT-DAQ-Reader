Paste your NIDAQmx.h and NIDAQmx.lib here, and then from Visual Studio 32bit compiler, do
    
    cl /c /EHsc *.c
    
    lib ftconfig.obj ftrt.obj dom.obj expatls.obj node.obj stack.obj xmlparse.obj xmlrole.obj xmltok.obj /OUT:aticonvert.lib