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
 * @file tgDataLoggerRodBasic.cpp
 * @brief Contains the implementation of class tgDataLoggerRodBasic.
 * @author Drew Sabelhaus and Brian Tietz
 * $Id$
 */

#include "tgDataLoggerRodBasic.h"

#include "util/tgBaseCPGNode.h"
#include "core/tgLinearString.h"
#include "core/tgRod.h"

#include <iostream>
#include <fstream>

/**
 * Construct our data logger, passing in just the filename.
 */
tgDataLoggerRodBasic::tgDataLoggerRodBasic(std::string fileName) :
m_fileName(fileName)
{}

/** Virtual base classes must have a virtual destructor. */
tgDataLoggerRodBasic::~tgDataLoggerRodBasic()

{ }

void tgDataLoggerRodBasic::render(const tgRod& rod) const
{
  //std::cout << "we're rendering a rod!" << std::endl;
    std::ofstream tgOutput;
    tgOutput.open(m_fileName.c_str(), std::ios::app);
    
    btVector3 com = rod.centerOfMass();
    
    tgOutput << com[0] << ","
    << com[1] << ","
    << com[2] << ","
    << rod.mass() << ",";
    
    tgOutput.close();
}
    
void tgDataLoggerRodBasic::render(const tgLinearString& linearString) const
{
    std::ofstream tgOutput;
    tgOutput.open(m_fileName.c_str(), std::ios::app);
    
    tgOutput << linearString.getRestLength() << ","    
    << linearString.getCurrentLength() << ","
    << linearString.getTension() << ",";
    
    tgOutput.close();
}

void tgDataLoggerRodBasic::render(const tgModel& model) const
{
  /* nothing to do here, since we don't save data specific to a model,
     but instead just render the individual components. */
}
