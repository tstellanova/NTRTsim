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
 * @file AppT6Model_tgDLR.cpp
 * @brief Contains the definition function main() for the redesign example of tgDL
 * application.
 * $Id$
 */

// This application
#include "T6Model_tgDLR.h"
#include "T6RestLengthController_tgDLR.h"
// This library
#include "core/terrain/tgBoxGround.h"
#include "core/tgModel.h"
#include "core/tgSimViewGraphics.h"
#include "core/tgSimulation.h"
#include "core/tgWorld.h"
#include "tgDataObserverBasic.h"
// Bullet Physics
#include "LinearMath/btVector3.h"
// The C++ Standard Library
#include <iostream>

/**
 * The entry point.
 * @param[in] argc the number of command-line arguments
 * @param[in] argv argv[0] is the executable name
 * @return 0
 */
int main(int argc, char** argv)
{
    std::cout << "AppT6Model_tgDLR" << std::endl;

    // First create the ground and world
    
    // Determine the angle of the ground in radians. All 0 is flat
    const double yaw = 0.0;
    const double pitch = M_PI/15.0;
    const double roll = 0.0;
    const tgBoxGround::Config groundConfig(btVector3(yaw, pitch, roll));
    // the world will delete this
    tgBoxGround* ground = new tgBoxGround(groundConfig);
    
    const tgWorld::Config config = 
    {
        // Note, by changing the setting below from 981 to 98.1, we've
        // scaled the world length scale to decimeters not cm.

        // Gravity, in cm/sec^2. Use this to adjust length scale of world.
        98.1
    };
    tgWorld world(config, ground);

    // Second create the view
    const double timestep_physics = 0.0001; // Seconds
    const double timestep_graphics = 1.f/60.f; // Seconds
    tgSimViewGraphics view(world, timestep_physics, timestep_graphics);

    // Third create the simulation
    tgSimulation simulation(view);

    // Fourth create the models with their controllers and add the models to the
    // simulation
    T6Model_tgDLR* const myModel = new T6Model_tgDLR();

    // Fifth, select the controller to use. Uncomment desired controller.

    // For the T6RestLengthController, pass in the amount of cable to contract
    // in. This is the "rest length difference": the static offset of cable
    // length between geometric length in equilibrium and the actual rest length
    // of an individual cable. 
    // Note for the above scale of gravity, this is in decimeters.
    T6RestLengthController_tgDLR* const pTC = new T6RestLengthController_tgDLR(4);
    //tgObserver<tgModel>* const pTC = dynamic_cast<tgObserver<tgModel>* >(new T6RestLengthController_tgDLR(4));

    // For the T6TensionController,
    // Set the tension of the controller units of kg * length / s^2
    // So 10000 units at this scale is 1000 N

    // T6TensionController* const pTC = new T6TensionController(10000);

    //myModel->attach(pTC);

    // Finally, attach a data observer if we want to log data
    //tgObserver<T6Model_tgDLR>* myDataObserver = dynamic_cast<tgObserver<T6Model_tgDLR>* >(new tgDataObserverBasic("/home/drew/NTRTsim_logs/tgDLR"));
    tgDataObserverBasic* const myDataObserver = new tgDataObserverBasic("/home/drew/NTRTsim_logs/tgDLR");

    // Note that we need to cast this observer to the supertype of an observer
    // for this specific model, or else we get errors, since attach() assumes
    // its argument is of type tgObserver<(thisclass)> not tgObserver<tgModel>.
    // TO-DO: fix this.
    //myModel->attach(dynamic_cast<tgObserver<T6Model_tgDLR>* const>(myDataObserver));

    myModel->attach(myDataObserver);

    myModel->attach(pTC);

    // Add model to simulation...
    simulation.addModel(myModel);
    
    // And run until the user stops
    simulation.run();

    //Teardown is handled by delete, so that should be automatic
    return 0;
}
