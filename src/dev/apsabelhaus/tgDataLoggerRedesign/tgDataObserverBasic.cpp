/*
 * Copyright © 2012, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All rights reserved.
 * 
 * The NASA Tensegrity Robotics Toolkit (NTRT) v1 platform is licensed
 * under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
*/

/**
 * @file tgDataObserverBasic.cpp
 * @brief Implementation of tgDataObserverBasic class
 * @author Drew Sabelhaus and Brian Tietz Mirletz
 * @date July 27, 2014
 * $Id$
 */

#include "tgDataObserverBasic.h"

#include "tgDataLoggerRodBasic.h"

#include "core/tgCast.h"
#include "core/tgModel.h"
#include "core/tgRod.h"
#include "core/tgString.h"

// todo REMOVE THIS. Observer shouldn't be dependent on specific model.
//#include "T6Model_tgDLR.h"

#include "core/tgLinearString.h"

#include <iostream>
#include <sstream>  
#include <time.h>

/**
 * Instantiate a tgDataObserverBasic.
 * Open up our output file, make sure we release all previous data loggers
 * @todo make sure we're free-ing old loggers here...
 */

tgDataObserverBasic::tgDataObserverBasic(std::string filePrefix) :
m_totalTime(0.0),
m_dataLoggerRodBasic(NULL),
m_filePrefix(filePrefix)
{
  logging_period = 10;
  count_logging_timesteps = 0;
}

tgDataObserverBasic::~tgDataObserverBasic()
{ 
    delete m_dataLoggerRodBasic;
    tgOutput.close();
}

/**@todo move functions to constructor when possible */
void tgDataObserverBasic::onSetup(tgModel& model)
{
    /**
     * Adapted from: http://www.cplusplus.com/reference/clibrary/ctime/localtime/
     * Also http://www.cplusplus.com/forum/unices/2259/
     */
    time_t rawtime;
    tm* currentTime;
    int fileTimeSize = 64;
    char fileTime [fileTimeSize];
    
    time (&rawtime);
    currentTime = localtime(&rawtime);
    strftime(fileTime, fileTimeSize, "%m%d%Y_%H%M%S.txt", currentTime);
    m_fileName = m_filePrefix + fileTime;
    std::cout << m_fileName << std::endl;
    
    if (m_dataLoggerRodBasic != NULL)
    {
        // prevent leaks on loop behavior (better than teardown?)
        delete m_dataLoggerRodBasic;
    }
    
    m_dataLoggerRodBasic = new tgDataLoggerRodBasic(m_fileName);
    
    m_totalTime = 0.0;
    
    // First time opening this, so nothing to append to
    tgOutput.open(m_fileName.c_str());
    
    std::vector<tgModel*> children = model.getDescendants();
    
    /*
     * Numbers to ensure uniqueness of variable names in log
     * May be redundant with tag
     */
    int stringNum = 0;
    int rodNum = 0;
    
    tgOutput << "Time" << ",";
    
    for (std::size_t i = 0; i < children.size(); i++)
    {
        /* If its a type we'll be logging, record its name and the 
         * variable types we'll be logging later
         */
        std::stringstream name;
        
        if(tgCast::cast<tgModel, tgLinearString>(children[i]) != 0) 
        {
            name << children[i]->getTags() <<  " " << stringNum;
            tgOutput <<  name.str() << "_RL" << ","
            <<  name.str() << "_AL" << ","
            <<  name.str() << "_Ten" << ",";
            stringNum++;
        }
        else if(tgCast::cast<tgModel, tgRod>(children[i]) != 0)
        {
            name << children[i]->getTags() <<  " " << rodNum;
            tgOutput << name.str() << "_X" << ","
            << name.str() << "_Y" << ","
            << name.str() << "_Z" << ","
            << name.str() << "_mass" << ",";
            rodNum++;
        }
        // Else do nothing since tgDataLogger won't touch it
    }
    
    tgOutput << std::endl;
    
    tgOutput.close();
}

/**
 * Dispatch the visitors to the given model and log data
 * @param[in] the number of seconds since the previous call; must be
 * positive
 */
void tgDataObserverBasic::onStep(tgModel& model, double dt)
{
    std::cout << "stepping!" << std::endl;

    // check if we log during this iteration of the simulation
    count_logging_timesteps += 1;
    if( count_logging_timesteps >= logging_period){

      // reset the count
      count_logging_timesteps = 0;

      // Append Timestamp
      m_totalTime += dt;
      tgOutput.open(m_fileName.c_str(), std::ios::app);
      tgOutput << m_totalTime << ",";
      tgOutput.close();
    
      // Call each of the loggers to output their data
      model.onVisit(*m_dataLoggerRodBasic);
    
      // Append a newline character
      tgOutput.open(m_fileName.c_str(), std::ios::app);
      tgOutput << std::endl;
      tgOutput.close();
    }

}
