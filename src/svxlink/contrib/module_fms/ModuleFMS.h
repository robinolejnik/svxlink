/**
@file	 ModuleFMS.h
@brief   FMS (Funkmeldesystem) module main file
@author  Robin Olejnik DF2RO
@date	 2022-04-07

\verbatim
A module (plugin) for the svxlink server, a multi purpose tranciever
frontend system.
Copyright (C) 2022 Robin Olejnik DF2RO

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
\endverbatim
*/


#ifndef MODULE_FMS_INCLUDED
#define MODULE_FMS_INCLUDED


/****************************************************************************
 *
 * System Includes
 *
 ****************************************************************************/

#include <string>
#include <vector>



/****************************************************************************
 *
 * Project Includes
 *
 ****************************************************************************/

#include <Module.h>
#include <version/SVXLINK.h>
#include <AsyncTimer.h>
#include <AsyncAudioSink.h>
#include <AsyncAudioPacer.h>
#include <AsyncAudioDebugger.h>
#include <AfskModulator.h>
#include <AfskDemodulator.h>


/****************************************************************************
 *
 * Local Includes
 *
 ****************************************************************************/

#include "FmsSync.h"


/****************************************************************************
 *
 * Forward declarations
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Namespace
 *
 ****************************************************************************/

//namespace MyNameSpace
//{


/****************************************************************************
 *
 * Forward declarations of classes inside of the declared namespace
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Defines & typedefs
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Exported Global Variables
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Class definitions
 *
 ****************************************************************************/

/**
@brief	A_brief_description_of_this_class
@author Tobias Blomberg
@date   2005-08-28
*/
class ModuleFMS : public Module
{
  public:
    ModuleFMS(void *dl_handle, Logic *logic, const std::string& cfg_name);
    ~ModuleFMS(void);
    const char *compiledForVersion(void) const { return SVXLINK_VERSION; }

  protected:
    virtual void resumeOutput(void) override;
    virtual void allSamplesFlushed(void) override;
    virtual int writeSamples(const float *samples, int count) override;
    virtual void flushSamples(void) override;
  
  private:
    Async::Timer responseTimer;
    Async::AudioDebugger *debugger;
    AfskModulator *mod;
    AfskDemodulator *demod;
    //Async::AudioPacer *pacer;

    FmsSync *sync;

    bool sendResponse;
    
    bool initialize(void) override;
    void activateInit(void) override;
    void deactivateCleanup(void) override;
    bool dtmfDigitReceived(char digit, int duration) override;
    void dtmfCmdReceivedWhenIdle(const std::string &cmd) override;
    void squelchOpen(bool is_open) override;
    void allMsgsWritten(void) override;

    void setNibble(std::vector<bool> &bits, unsigned char value, bool invert = true);
    void packetReceived(uint64_t message);
    void sendMessage(uint64_t message);
    void responseTimerExpired(void);

};  /* class ModuleFMS */


//} /* namespace */

#endif /* MODULE_FMS_INCLUDED */



/*
 * This file has not been truncated
 */
