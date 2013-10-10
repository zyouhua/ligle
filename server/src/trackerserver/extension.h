///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// extension.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _EXTENSION_H_
#define _EXTENSION_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "ise_system.h"

///////////////////////////////////////////////////////////////////////////////
// class CAppExtension - 应用程序功能扩展类

class CAppExtension : public CCustomExtension
{
public:
    virtual bool ParseArguments(int nArgc, char *sArgv[]);

    virtual void ShowWelcomeMessage();
    virtual void ShowStartedMessage();
    virtual void ShowStartFailMessage();

    virtual void InitSystemOptions(CSystemOptions& SysOpt);
    virtual void InitServerOptions(CServerOptions& SvrOpt);
    virtual CCustomDispatcher* CreateDispatcher();

    virtual void ProgramInit();
    virtual void ProgramExit();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _EXTENSION_H_ 
