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

#ifndef TG_CPG_CABLE_CONTROL_H
#define TG_CPG_CABLE_CONTROL_H

#include "examples/learningSpines/tgCPGStringControl.h"
#include "core/tgSpringCableActuator.h"
// The Boost library
#include "boost/multi_array.hpp"

// Forward declarations
class tgPIDController;

class tgCPGCableControl : public tgCPGStringControl
{
public:
 
    tgCPGCableControl(const double controlStep = 1.0/10000.0);
    
    virtual ~tgCPGCableControl();
    
    virtual void onSetup(tgSpringCableActuator& subject);
    
    virtual void onStep(tgSpringCableActuator& subject, double dt);

protected:
   tgPIDController* m_PID; 

};


#endif
