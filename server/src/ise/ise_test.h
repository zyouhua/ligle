///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_test.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_TEST_H_
#define _ISE_TEST_H_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "ise_system.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CAppExtension

class CAppExtension : public CCustomExtension
{
public:
    virtual void ShowWelcomeMessage();
    virtual void ShowStartedMessage();
    virtual void ShowStartFailMessage();

    virtual void InitSystemOptions(CSystemOptions& SysOpt);
    virtual void InitServerOptions(CServerOptions& SvrOpt);

    virtual void ProgramExit();
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_TEST_H_ 
