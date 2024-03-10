/**
@file	 ModuleFMS.cpp
@brief   A_brief_description_of_this_module
@author  Tobias Blomberg / SM0SVX
@date	 2005-08-28

\verbatim
A module (plugin) for the multi purpose tranciever frontend system.
Copyright (C) 2004  Tobias Blomberg

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



/****************************************************************************
 *
 * System Includes
 *
 ****************************************************************************/

#include <stdio.h>

#include <iostream>
#include <sstream>


/****************************************************************************
 *
 * Project Includes
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Local Includes
 *
 ****************************************************************************/

#include "ModuleFMS.h"



/****************************************************************************
 *
 * Namespaces to use
 *
 ****************************************************************************/


/****************************************************************************
 *
 * Defines & typedefs
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Local class definitions
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Prototypes
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Exported Global Variables
 *
 ****************************************************************************/




/****************************************************************************
 *
 * Local Global Variables
 *
 ****************************************************************************/



/****************************************************************************
 *
 * Pure C-functions
 *
 ****************************************************************************/


extern "C" {
  Module *module_init(void *dl_handle, Logic *logic, const char *cfg_name)
  {
    return new ModuleFMS(dl_handle, logic, cfg_name);
  }
} /* extern "C" */



/****************************************************************************
 *
 * Public member functions
 *
 ****************************************************************************/


ModuleFMS::ModuleFMS(void *dl_handle, Logic *logic, const std::string& cfg_name)
  : Module(dl_handle, logic, cfg_name)
{
  std::cout << "\tModuleFMS starting...\n";
  responseTimer.expired.connect(sigc::hide(mem_fun(*this, &ModuleFMS::responseTimerExpired)));
} /* ModuleFMS */


ModuleFMS::~ModuleFMS(void)
{
  std::cout << "FMS ~ModuleFMS();" << std::endl;
  delete demod;
  delete mod;
  delete sync;
  //delete pacer;

} /* ~ModuleFMS */

/****************************************************************************
 *
 * Protected member functions
 *
 ****************************************************************************/

void ModuleFMS::resumeOutput(void)
{
  std::cout << "FMS resumeOutput();" << std::endl;
} /* ModuleFMS::resumeOutput */


void ModuleFMS::allSamplesFlushed(void)
{
  std::cout << "FMS allSamplesFlushed();" << std::endl;
} /* ModuleFMS::allSamplesFlushed */


int ModuleFMS::writeSamples(const float *samples, int count)
{
  std::cout << "FMS writeSamples();" << std::endl;
  return count;
} /* ModuleFMS::writeSamples */


void ModuleFMS::flushSamples(void)
{
  std::cout << "FMS flushSamples();" << std::endl;
  sourceAllSamplesFlushed();
} /* ModuleFMS::flushSamples */





/****************************************************************************
 *
 * Private member functions
 *
 ****************************************************************************/

void ModuleFMS::responseTimerExpired(void) {
  responseTimer.setEnable(false);
  std::cout << "FMS responseTimerExpired();" << std::endl;
  sendMessage(0);
}

void ModuleFMS::setNibble(std::vector<bool> &bits, unsigned char value, bool invert) {
  value &= 0xf;
  if(invert) {
    bits.push_back((value>>0) & 1);
    bits.push_back((value>>1) & 1);
    bits.push_back((value>>2) & 1);
    bits.push_back((value>>3) & 1);
  }
  else {
    bits.push_back((value>>3) & 1);
    bits.push_back((value>>2) & 1);
    bits.push_back((value>>1) & 1);
    bits.push_back((value>>0) & 1);
  }
}

void ModuleFMS::sendMessage(uint64_t message) {
  std::vector<bool> bits;

  for(int i=0;i<36;i++) {

    bits.push_back(false);
  }
  unsigned int fms = 0x6902008a;
  unsigned int status = 0xf;
  unsigned char bs = 1;
  unsigned int dir = 1;
  unsigned int tk = 3;

  tk--;
  setNibble(bits, 0x7, false);
  setNibble(bits, 0xf, false);
  setNibble(bits, 0xf, false);
  setNibble(bits, 0x1, false);
  setNibble(bits, 0xa, false);

  setNibble(bits, fms>>28); // BOS
  setNibble(bits, fms>>24); // Land
  setNibble(bits, fms>>20); // Ort
  setNibble(bits, fms>>16); // Ort
  setNibble(bits, fms>>12); // Kfz
  setNibble(bits, fms>>8);  // Kfz
  setNibble(bits, fms>>4);  // Kfz
  setNibble(bits, fms>>0);  // Kfz
  setNibble(bits, status);
  setNibble(bits, (bs<<3) | (dir<<2) | tk, false);

  //0100
  bits.push_back(false);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);

  //1100
  bits.push_back(true);
  bits.push_back(true);
  bits.push_back(false);
  bits.push_back(false);

  mod->sendBits(bits);
  //mod->sendBits(bits);
}

void ModuleFMS::packetReceived(uint64_t message) {
  std::cout << "FMS packetReceived();" << std::endl;
  if(message==7277623484877635584)
    return;

  sendResponse = true;
  //responseTimer.setEnable(true);
}

/*
 *----------------------------------------------------------------------------
 * Method:    initialize
 * Purpose:   Called by the core system right after the object has been
 *    	      constructed. As little of the initialization should be done in
 *    	      the constructor. It's easier to handle errors here.
 * Input:     None
 * Output:    Return \em true on success or else \em false should be returned
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2005-08-28
 * Remarks:   The base class initialize method must be called from here.
 * Bugs:
 *----------------------------------------------------------------------------
 */
bool ModuleFMS::initialize(void)
{
  std::cout << "FMS initialize();" << std::endl;
  if (!Module::initialize())
  {
    return false;
  }

  

  demod = new AfskDemodulator(1800, 1200, 1200, 16000);
  mod = new AfskModulator(1800, 1200, 1200, -6, 16000);
  sync = new FmsSync(1200, 16000);
  debugger = new Async::AudioDebugger();
  debugger->setName("mod");
  //pacer = new Async::AudioPacer(16000, 256, 0);
  //mod->registerSink(pacer);
  //mod->registerSink(debugger);
  demod->registerSink(sync);

  AudioSink::setHandler(demod);
  AudioSource::setHandler(mod);

  sync->packetReceived.connect(mem_fun(*this, &ModuleFMS::packetReceived));

  responseTimer.setTimeout(100);
  return true;

} /* initialize */


/*
 *----------------------------------------------------------------------------
 * Method:    activateInit
 * Purpose:   Called by the core system when this module is activated.
 * Input:     None
 * Output:    None
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2004-03-07
 * Remarks:
 * Bugs:
 *----------------------------------------------------------------------------
 */
void ModuleFMS::activateInit(void)
{
  std::cout << "FMS activateInit();" << std::endl;
} /* activateInit */


/*
 *----------------------------------------------------------------------------
 * Method:    deactivateCleanup
 * Purpose:   Called by the core system when this module is deactivated.
 * Input:     None
 * Output:    None
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2004-03-07
 * Remarks:   Do NOT call this function directly unless you really know what
 *    	      you are doing. Use Module::deactivate() instead.
 * Bugs:
 *----------------------------------------------------------------------------
 */
void ModuleFMS::deactivateCleanup(void)
{
  std::cout << "FMS deactivateCleanup();" << std::endl;
  responseTimer.setEnable(false);
} /* deactivateCleanup */


/*
 *----------------------------------------------------------------------------
 * Method:    dtmfDigitReceived
 * Purpose:   Called by the core system when a DTMF digit has been
 *    	      received. This function will only be called if the module
 *    	      is active.
 * Input:     digit   	- The DTMF digit received (0-9, A-D, *, #)
 *            duration	- The length in milliseconds of the received digit
 * Output:    Return true if the digit is handled or false if not
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2004-03-07
 * Remarks:
 * Bugs:
 *----------------------------------------------------------------------------
 */
bool ModuleFMS::dtmfDigitReceived(char digit, int duration)
{
  std::cout << "DTMF digit received in module " << name() << ": " << digit << std::endl;

  return false;

} /* dtmfDigitReceived */

void ModuleFMS::dtmfCmdReceivedWhenIdle(const std::string &cmd)
{

} /* dtmfCmdReceivedWhenIdle */


/*
 *----------------------------------------------------------------------------
 * Method:    squelchOpen
 * Purpose:   Called by the core system when the squelch open or close.
 * Input:     is_open - Set to \em true if the squelch is open or \em false
 *    	      	      	if it's not.
 * Output:    None
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2005-08-28
 * Remarks:
 * Bugs:
 *----------------------------------------------------------------------------
 */
void ModuleFMS::squelchOpen(bool is_open)
{
  std::cout << "FMS squelchOpen();" << std::endl;
  if(!is_open && sendResponse) {
    sendResponse = false;
    responseTimer.setEnable(true);
  }
} /* squelchOpen */


/*
 *----------------------------------------------------------------------------
 * Method:    allMsgsWritten
 * Purpose:   Called by the core system when all announcement messages has
 *    	      been played. Note that this function also may be called even
 *    	      if it wasn't this module that initiated the message playing.
 * Input:     None
 * Output:    None
 * Author:    Tobias Blomberg / SM0SVX
 * Created:   2005-08-28
 * Remarks:
 * Bugs:
 *----------------------------------------------------------------------------
 */
void ModuleFMS::allMsgsWritten(void)
{

} /* allMsgsWritten */



/*
 * This file has not been truncated
 */
