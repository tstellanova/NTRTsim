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

#ifndef TG_DATA_OBSERVER_BASIC_H
#define TG_DATA_OBSERVER_BASIC_H

/**
 * @file tgDataObserverBasic.h
 * @brief Definition of tgDataObserverBasic class
 * @author Drew Sabelhaus and Brian Tietz Mirletz
 * @date July 27, 2014
 * $Id$
 */

#include <fstream>
#include <string>
#include "core/tgObserver.h"
#include "core/tgModel.h"

// Forward declarations of classes
// class tgModel;
//class T6Model_tgDLR;
class tgDataLoggerRodBasic;

/**
 * tgDataObserverBasic is a type of tgDataLogger, which observes a model.
 * When notified, this observer calls individual data loggers for different
 * Loggable type objects inside the model.
 * This basic data observer only logs the center-of-mass of rods.
 * Note that there should never be model-specific information in any of these
 * data observer or logger classes.
 * Also note that unlike the original tgDataObserver class, this one is
 * actually a real tgObserver!
 */

class tgDataObserverBasic : public tgObserver<tgModel>
{
public:

    /**
     * Construct a tgDataObserverBasic.
     * @param filePrefix, the base path to the file we save data into.
     * This is just the prefix: the actual saved file has the timestamp
     * of the start of the simulation appended to this prefix.
     */
    tgDataObserverBasic(std::string filePrefix);
    
    /** A class with virtual member functions must have a virtual destructor. */
    virtual ~tgDataObserverBasic();
    
    /**
     * Create the log file and header during setup of the simulation.
     * @param[in] model, the tgModel we're observing.
     */
    virtual void onSetup(tgModel& model);
    
    /**
     * Dispatch the visitors to the given model and log data
     * @param[in] the number of seconds since the previous call; must be
     * positive
     */
    virtual void onStep(tgModel& model, double dt);
    
    /** @todo add reset method so we can start a new file when
     * the simulation resets */

private:
    
    std::ofstream tgOutput;
    
    std::string m_fileName;

    // How often do we want to sample?
    // This parameter indicates how many timesteps to take before recording
    // a sample: for example 10 timesteps * 0.0001 seconds = logs every 0.001 sec.
    int logging_period;

    // counting how many timesteps, to compare to frequency
    int count_logging_timesteps;
    
    ///@todo find a way to move things to the constructor and remove this
    std::string m_filePrefix;
    
    double m_totalTime;
    
    // References to all the individual loggers we'll call to actually
    // output data.
    tgDataLoggerRodBasic * m_dataLoggerRodBasic;
    
};
   
#endif
