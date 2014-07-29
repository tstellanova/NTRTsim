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

#ifndef TG_DATA_LOGGER_ROD_BASIC_H
#define TG_DATA_LOGGER_ROD_BASIC_H

/**
 * @file tgDataLoggerRodBasic.h
 * @brief Contains the definition of class tgDataLoggerRodBasic
 * @author Drew Sabelhaus and Brian Tietz
 * $Id$
 */
 
#include "core/tgModelVisitor.h"

#include <string>

// Forward declarations
class tgLinearString;
class tgModel;
class tgRod;

/**
 * This actual data logger is a tgModelVisitor, which has the methods
 * 'render' that are called upon visit.
 */
class tgDataLoggerRodBasic : public tgModelVisitor {
    
public:

    /* The constuctor only takes the filename of the output */
    tgDataLoggerRodBasic(std::string fileName);
    
    /** Virtual base classes must have a virtual destructor. */
    virtual ~tgDataLoggerRodBasic();
  
    virtual void render(const tgRod& rod) const;
  
    /**
     * Render a tgLinearString
     * @param[in] linearString a const reference to a tgLinearString to log data
     */
    virtual void render(const tgLinearString& linearString) const;
    
    virtual void render(const tgModel& model) const;

private:
    
    std::string m_fileName;

};

#endif
