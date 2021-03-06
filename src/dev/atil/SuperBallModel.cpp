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
 * @file SuperBallModel.cpp
 * @brief Contains the implementation of class SuperBallModel.
 * $Id$
 */

// This module
#include "SuperBallModel.h"
// This library
#include "core/tgBasicActuator.h"
#include "core/tgRod.h"
#include "tgcreator/tgBuildSpec.h"
#include "tgcreator/tgBasicActuatorInfo.h"
#include "tgcreator/tgRodInfo.h"
#include "tgcreator/tgStructure.h"
#include "tgcreator/tgStructureInfo.h"
// The Bullet Physics library
#include "LinearMath/btVector3.h"
// The C++ Standard Library
#include <stdexcept>
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

namespace
{
    // see tgBaseString.h for a descripton of some of these rod parameters
    // (specifically, those related to the motor moving the strings.)
    // NOTE that any parameter that depends on units of length will scale
    // with the current gravity scaling. E.g., with gravity as 98.1,
    // the length units below are in decimeters.

    // Note: This current model of the SUPERball rod is 1.5m long by 3 cm radius,
    // which is 0.00424 m^3.
    // For SUPERball v1.5, mass = 3.5kg per strut, which comes out to 
    // 0.825 kg / (decimeter^3).

    // similarly, frictional parameters are for the tgRod objects.
    const struct Config
    {
        double density;
        double radius;
        double stiffness;
        double damping;
        double rod_length;
        double rod_space;    
        double friction;
        double rollFriction;
        double restitution;
        double pretension;
        bool   history;  
        double maxTens;
        double targetVelocity;
    } c =
   {
     0.825,    // density (kg / length^3)
     0.31,     // radius (length)
     3000.0,   // stiffness (kg / sec^2)
     200.0,    // damping (kg / sec)
     15.0,     // rod_length (length)
     7.5,      // rod_space (length)
     1.0,      // friction (unitless)
     0.01,     // rollFriction (unitless)
     0.2,      // restitution (?)
     0,        // pretension
     false,    // history
     100000,   // maxTens
     10000,    // targetVelocity
#if (0) // acceleration constrant removed 12/10/14
     20000     // maxAcc
#endif
     // Use the below values for earlier versions of simulation.
     // 1.006,    
     // 0.31,     
     // 300000.0, 
     // 3000.0,   
     // 15.0,     
     // 7.5,      
  };
} // namespace

/*
 * helper arrays for node and rod numbering schema
 */
/*returns the number of the rod for a given node */
const int rodNumbersPerNode[13]={0,1,2,3,3,4,0,1,2,5,5,4,6};

/*returns the node that is at the other end of the given node */
const int otherEndOfTheRod[13]={6,7,8,4,3,11,0,1,2,10,9,5,12};

/*returns the node that is at the parallel rod
 * and at the same end of the given node
 */
const int parallelNode[13]={1,0,5,9,10,2,7,6,11,3,4,8,12};

SuperBallModel::SuperBallModel(tgWorld& world) : tgModel(),m_world(world)
{
	this->m_world=world;
}

SuperBallModel::~SuperBallModel()
{
}

//Node numbers seen from Front
// -----0-------1------
// ---------2----------
// 3------------------4
// ---------5----------
// -----6-------7------
//
//Node numbers seen from Back
// -----0-------1------
// ---------8----------
// 9-----------------10
// ---------11---------
// -----6-------7------
//


void SuperBallModel::addNodes(tgStructure& s)
{
    const double half_length = c.rod_length / 2;

    nodePositions.push_back(btVector3(-half_length,   c.rod_space, 0));            // 0
    nodePositions.push_back(btVector3( half_length,   c.rod_space, 0));            // 1
    nodePositions.push_back(btVector3(0,            half_length,   -c.rod_space)); // 2
    nodePositions.push_back(btVector3(-c.rod_space, 0,           -half_length));   // 3
    nodePositions.push_back(btVector3( c.rod_space, 0,           -half_length));   // 4
    nodePositions.push_back(btVector3(0,           -half_length,   -c.rod_space)); // 5
    nodePositions.push_back(btVector3(-half_length,  -c.rod_space, 0));            // 6
    nodePositions.push_back(btVector3( half_length,  -c.rod_space, 0));            // 7
    nodePositions.push_back(btVector3(0,            half_length,    c.rod_space)); // 8
    nodePositions.push_back(btVector3(-c.rod_space, 0,            half_length));   // 9
    nodePositions.push_back(btVector3( c.rod_space, 0,            half_length));   // 10
    nodePositions.push_back(btVector3(0,           -half_length,    c.rod_space)); // 11

    for(int i=0;i<12;i++)
    {
		s.addNode(nodePositions[i][0],nodePositions[i][1],nodePositions[i][2]);
    }
}

void SuperBallModel::addRods(tgStructure& s)
{
    s.addPair( 0,  6, "r1 rod");
    s.addPair( 1,  7, "r2 rod");
    s.addPair( 2,  8, "r3 rod");
    s.addPair( 3,  4, "r4 rod");
    s.addPair( 5, 11, "r5 rod");
    s.addPair( 9, 10, "r6 rod");

}


void SuperBallModel::addMarkers()
{
    std::vector<tgRod *> rods=find<tgRod>("rod");

	for(int i=0;i<12;i++)
	{
		const btRigidBody* bt = rods[rodNumbersPerNode[i]]->getPRigidBody();
		btTransform inverseTransform = bt->getWorldTransform().inverse();
		btVector3 pos = inverseTransform * (nodePositions[i]);
		abstractMarker tmp=abstractMarker(bt,pos,btVector3(0.08*i,1.0 - 0.08*i,.0),i);
		this->addMarker(tmp);
	}
}

void SuperBallModel::addSensors()
{
	std::cout<<"Adding sensors"<<std::endl;
    std::vector<tgRod *> rods=find<tgRod>("rod");

	for(int i=0;i<12;i++)
	{
		const btRigidBody* bt = rods[rodNumbersPerNode[i]]->getPRigidBody();
		btTransform inverseTransform = bt->getWorldTransform().inverse();
		btVector3 pos = inverseTransform * (nodePositions[i]);
		heightSensor tmp=heightSensor(bt,pos,i,m_world);
		this->heightSensors.push_back(tmp);
	}
}

std::vector<double> SuperBallModel::getSensorInfo()
{
	std::vector<double> sensorInfo;
	for(int i=0;i<heightSensors.size();i++)
	{
		sensorInfo.push_back(heightSensors.at(i).getHeight());
	}

	return sensorInfo;
}

void SuperBallModel::addMuscles(tgStructure& s)
{
	musclesPerNodes.resize(13);
	for(int i=0;i<13;i++)
	{
		musclesPerNodes[i].resize(13);
		for(int j=0;j<13;j++)
			musclesPerNodes[i][j]=NULL;
	}
	for(int i=0;i<13;i++)
		for(int j=0;j<13;j++)
			muscleConnections[i][j]=-1;

	muscleConnections[0][3]=0;
	muscleConnections[3][2]=0;
	muscleConnections[2][0]=0;
	muscleConnections[4][5]=0;
	muscleConnections[5][7]=0;
	muscleConnections[7][4]=0;
	muscleConnections[1][8]=0;
	muscleConnections[8][10]=0;
	muscleConnections[10][1]=0;
	muscleConnections[9][11]=0;
	muscleConnections[11][6]=0;
	muscleConnections[6][9]=0;
	muscleConnections[1][2]=1;
	muscleConnections[2][4]=1;
	muscleConnections[4][1]=1;
	muscleConnections[3][5]=1;
	muscleConnections[5][6]=1;
	muscleConnections[6][3]=1;
	muscleConnections[0][8]=1;
	muscleConnections[8][9]=1;
	muscleConnections[9][0]=1;
	muscleConnections[11][7]=1;
	muscleConnections[7][10]=1;
	muscleConnections[10][11]=1;

	for(int i=0;i<13;i++)
	{
		for(int j=0;j<13;j++)
		{
			if(muscleConnections[i][j]>=0)
			{
				std::stringstream tag;
				tag<<"muscle "<<i<<"-"<<j;
				s.addPair(i, j, tag.str());
			}
		}
	}
}


void SuperBallModel::setup(tgWorld& world)
{

    const tgRod::Config rodConfig(c.radius, c.density, c.friction, 
				c.rollFriction, c.restitution);
    /// @todo acceleration constraint was removed on 12/10/14 Replace with tgKinematicActuator as appropreate
    tgBasicActuator::Config muscleConfig(c.stiffness, c.damping, c.pretension,
					    c.history, c.maxTens, c.targetVelocity);
            
    // Start creating the structure
    tgStructure s;
    addNodes(s);
    addRods(s);
    addMuscles(s);

//    // Add a rotation. This is needed if the ground slopes too much,
//    // otherwise  glitches put a rod below the ground.
//    btVector3 rotationPoint = btVector3(0, 0, 0); // origin
//    btVector3 rotationAxis = btVector3(0, 1, 0);  // y-axis
//    double rotationAngle = M_PI/2;
//    s.addRotation(rotationPoint, rotationAxis, rotationAngle);

    //s.move(btVector3(0,30,0));

    // Create the build spec that uses tags to turn the structure into a real model
    tgBuildSpec spec;
    spec.addBuilder("rod", new tgRodInfo(rodConfig));
    spec.addBuilder("muscle", new tgBasicActuatorInfo(muscleConfig));
    
    // Create your structureInfo
    tgStructureInfo structureInfo(s, spec);

    // Use the structureInfo to build ourselves
    structureInfo.buildInto(*this, world);

    // We could now use tgCast::filter or similar to pull out the
    // models (e.g. muscles) that we want to control. 
    allMuscles = tgCast::filter<tgModel, tgBasicActuator> (getDescendants());

    // call the onSetup methods of all observed things e.g. controllers
    notifySetup();

    // Actually setup the children
    tgModel::setup(world);

    this->m_world = world;

    //map the rods and add the markers to them
    addMarkers();
    addSensors();

    fillNodeNumberingSchema(6,11,9);
    fillMusclesPerNode();

    //move rotate and give initial speed to the structure
    btVector3 location(0,13.0,0);
    btVector3 rotation(0.0,0.6,0.8);
  	btVector3 speed(0,10,0);
    this->moveModel(location,rotation,speed);

}

void SuperBallModel::step(double dt)
{
    // Precondition
    if (dt <= 0.0)
    {
        throw std::invalid_argument("dt is not positive");
    }
    else
    {
        // Notify observers (controllers) of the step so that they can take action
        notifyStep(dt);
        tgModel::step(dt);  // Step any children
    }
}

void SuperBallModel::onVisit(tgModelVisitor& r)
{
    tgModel::onVisit(r);
}

const std::vector<tgBasicActuator*>& SuperBallModel::getAllMuscles() const
{
    return allMuscles;
}
    
void SuperBallModel::teardown()
{
	notifyTeardown();

	std::cout<<"Tearing down model"<<std::endl;
	this->nodeNumberingSchema.clear();
	this->heightSensors.clear();
	this->nodePositions.clear();

    tgModel::teardown();
}

void SuperBallModel::moveModel(btVector3 positionVector,btVector3 rotationVector,btVector3 speedVector)
{
    std::vector<tgRod *> rods=find<tgRod>("rod");

	btQuaternion initialRotationQuat;
	initialRotationQuat.setEuler(rotationVector[0],rotationVector[1],rotationVector[2]);
	btTransform initialTransform;
	initialTransform.setIdentity();
	initialTransform.setRotation(initialRotationQuat);
	initialTransform.setOrigin(positionVector);
	for(int i=0;i<rods.size();i++)
	{
			rods[i]->getPRigidBody()->setLinearVelocity(speedVector);
			rods[i]->getPRigidBody()->setWorldTransform(initialTransform * rods[i]->getPRigidBody()->getWorldTransform());
	}
}

std::vector< btVector3 > SuperBallModel::getSensorPositions()
{
	std::vector<btVector3 > positions;
	for(int i=0;i<heightSensors.size();i++)
	{
		positions.push_back(heightSensors[i].getWorldPosition());
	}
	return positions;
}

std::vector< btVector3 > SuperBallModel::getSensorOrientations()
{
	btVector3 temp;
	std::vector<btVector3 > directions;
	for(int i=0;i<12;i++)
	{
		temp = heightSensors[i].getWorldPosition() - heightSensors[otherEndOfTheRod[i]].getWorldPosition();
		temp /= temp.length();
		directions.push_back(temp);
	}
	return directions;
}


btVector3 SuperBallModel::getCenter()
{
	btVector3 pos(0.0,0.0,0.0);
	for(int i=0;i< heightSensors.size();i++)
	{
		pos +=  heightSensors[i].getWorldPosition();
	}
	pos /= heightSensors.size();
	return pos;
}


btDynamicsWorld *SuperBallModel::getWorld()
{
	return &(tgBulletUtil::worldToDynamicsWorld(this->m_world));
}

void SuperBallModel::fillNodeNumberingSchema(int base1,int base2,int base3)
{
	nodeNumberingSchema.resize(3);
	for (int i = 0; i < 3; ++i)
	{
		nodeNumberingSchema[i].resize(2);
		for (int j = 0; j < 2; ++j)
		{
			nodeNumberingSchema[i][j].resize(2);
			for (int k=0;k<2;k++)
			{
				nodeNumberingSchema[i][j][k]=-1;
			}
		}
	}

	nodeNumberingSchema[0][0][0]=base1;
	nodeNumberingSchema[1][0][0]=base2;
	nodeNumberingSchema[2][0][0]=base3;
	//For each base node
	for(int i=0;i<3;i++)
	{
		//Other end of the rod that this node is in
		nodeNumberingSchema[i][0][1]=otherEndOfTheRod[nodeNumberingSchema[i][0][0]];
		//Same end of the parallel rod
		nodeNumberingSchema[i][1][0]=parallelNode[nodeNumberingSchema[i][0][0]];
		//Other end of the parallel rod
		nodeNumberingSchema[i][1][1]=otherEndOfTheRod[nodeNumberingSchema[i][1][0]];
	}
}

void SuperBallModel::fillNodeMappingFromBasePoints(int a,int b,int c)
{
	for(int i=0;i<13;i++)
	{
		nodeMapping[i]=-1;
		nodeMappingReverse[i]=-1;
	}
	nodeMapping[12]=12;

	nodeMapping[a]=nodeNumberingSchema[0][0][0];
	nodeMapping[b]=nodeNumberingSchema[1][0][0];
	nodeMapping[c]=nodeNumberingSchema[2][0][0];

	nodeMapping[otherEndOfTheRod[a]]=nodeNumberingSchema[0][0][1];
	nodeMapping[otherEndOfTheRod[b]]=nodeNumberingSchema[1][0][1];
	nodeMapping[otherEndOfTheRod[c]]=nodeNumberingSchema[2][0][1];

	nodeMapping[parallelNode[a]]=nodeNumberingSchema[0][1][0];
	nodeMapping[parallelNode[b]]=nodeNumberingSchema[1][1][0];
	nodeMapping[parallelNode[c]]=nodeNumberingSchema[2][1][0];

	nodeMapping[otherEndOfTheRod[parallelNode[a]]]=nodeNumberingSchema[0][1][1];
	nodeMapping[otherEndOfTheRod[parallelNode[b]]]=nodeNumberingSchema[1][1][1];
	nodeMapping[otherEndOfTheRod[parallelNode[c]]]=nodeNumberingSchema[2][1][1];

	for(int i=0;i<13;i++)
	{
//		cout<<"mapping old: "<<i<<" new: "<<nodeMapping[i]<<endl;
		nodeMappingReverse[nodeMapping[i]]=i;
	}
}


int SuperBallModel::getOtherEndOfTheRod(int i)
{
	if(i>=0 && i<13)
		return otherEndOfTheRod[i];
	else
		return -1;
}

void SuperBallModel::fillMusclesPerNode()
{
	for(int i=0;i<13;i++)
	{
		for(int j=0;j<13;j++)
		{
			if(muscleConnections[i][j]>=0)
			{
				std::stringstream tag;
				tag<<"muscle "<<i<<"-"<<j;
				std::vector<tgBasicActuator*> foundStr = this->find<tgBasicActuator>(tag.str());
				if(!foundStr.empty())
				{
//					std::cout<<"Found muscle "<<tag.str()<<std::endl;
					musclesPerNodes[i][j]=foundStr[0];
					musclesPerNodes[j][i]=musclesPerNodes[i][j];
				}
			}
		}
	}
}
