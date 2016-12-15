/*
 * @(#) 3DPrintGenerator.cpp   1.0   Aug, 22, 2016
 *
 *
 * The ROBOGEN Framework
 * Copyright © 2012-2016 Andrea Maesani, Joshua Auerbach
 *
 * Laboratory of Intelligent Systems, EPFL
 *
 * This file is part of the ROBOGEN Framework.
 *
 * The ROBOGEN Framework is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License (GPL)
 * as published by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @(#) $Id$
 */

 #include <iostream>
 #include "Print3DGenerator/Print3DGenerator.h"
 #include "evolution/representation/RobotRepresentation.h"
 #include "PartList.h"

using namespace robogen;
using namespace std;

Print3DGenerator::Print3DGenerator(const std::string &robotFileName, const std::string &outputFolder, PrinterType printerType) : 
	Print3DGenerator(robotFileName, outputFolder, "..", printerType)
{ 

}

Print3DGenerator::Print3DGenerator(const std::string &robotFileName, const std::string &outputFolder, const std::string &inputFolder, PrinterType printerType) : 
	robotFileName_(robotFileName),
	printerType_(printerType),
	STLFileFolder_(inputFolder + "/printing-3D/STL_Files_For_Printing"),
	outputFolder_(outputFolder),
	oscadWriter_(inputFolder + "/printing-3D/Openscad_Templates",outputFolder),
	STL_PART_NAMES_MAP(initSTLPartNamesMap(printerType))
{ 

}

const std::map<std::string, std::map <std::string, unsigned int > > Print3DGenerator::initSTLPartNamesMap(PrinterType printerType)
{
	std::map<std::string, std::map<std::string, unsigned int > > stlPartNamesMap;

	std::map <std::string, unsigned int > partMap;

	//PART_TYPE_CORE_COMPONENT, PART_TYPE_CORE_COMPONENT_NO_IMU, PART_TYPE_FIXED_BRICK
	if(printerType == SLS){
		partMap["Core"] = 1;
	} else if(printerType == FDM)
	{
		partMap["Core_FDM"] = 1;
	}
	stlPartNamesMap[PART_TYPE_CORE_COMPONENT] = partMap;
	stlPartNamesMap[PART_TYPE_CORE_COMPONENT_NO_IMU] = partMap;
	stlPartNamesMap[PART_TYPE_FIXED_BRICK] = partMap;

	// PART_TYPE_ACTIVE_HINGE
	partMap.clear();
	partMap["ActiveHinge_Frame"] = 1;
	partMap["ActiveHinge_ServoHolder"] = 1;
	stlPartNamesMap[PART_TYPE_ACTIVE_HINGE] = partMap;

	// PART_TYPE_PASSIVE_HINGE
	partMap.clear();
	partMap["PassiveHinge"] = 1;
	stlPartNamesMap[PART_TYPE_PASSIVE_HINGE] = partMap;

	// PART_TYPE_IR_SENSOR
	partMap.clear();
	if(printerType == SLS){
		partMap["IrSensor"] = 1;
	} else if(printerType == FDM)
	{
		partMap["IrSensor_FDM_PartA"] = 1;
		partMap["IrSensor_FDM_PartB"] = 1;
	}
	stlPartNamesMap[PART_TYPE_IR_SENSOR] = partMap;

	// PART_TYPE_LIGHT_SENSOR
	partMap.clear();
	partMap["LightSensor"] = 1;
	stlPartNamesMap[PART_TYPE_LIGHT_SENSOR] = partMap;

	// PART_TYPE_ACTIVE_WHEEL
	partMap.clear();
	partMap["ActiveWheelMotorHolder"] = 1;
	stlPartNamesMap[PART_TYPE_ACTIVE_WHEEL] = partMap;

	// PART_TYPE_PASSIVE_WHEEL
	partMap.clear();
	partMap["PassiveWheelPlate"] = 1;
	partMap["PassiveWheelRing"] = 1;
	stlPartNamesMap[PART_TYPE_PASSIVE_WHEEL] = partMap;

	// PART_TYPE_ACTIVE_WHEG
	partMap.clear();
	partMap["ActiveWheelMotorHolder"] = 1;
	stlPartNamesMap[PART_TYPE_ACTIVE_WHEG] = partMap; //this part is the same for active wheels and whegs


	return stlPartNamesMap;
}

bool Print3DGenerator::create3DFiles()
{
	/* create robot message */
	robogenMessage::Robot robotMessage;
	RobotRepresentation::createRobotMessageFromFile(robotMessage,robotFileName_);

	//Count required parts
	std::map<std::string, unsigned int> partCounts = countParts(robotMessage); // all parts
	std::map<std::string, unsigned int> copyCountMap = countCopyFiles(robotMessage); // sum of generic files to be copied
	
	//Generate files
	std::vector<std::string> parametricStlFileNames = writeParametricStlFiles(robotMessage);
	copyGenericStlFiles(copyCountMap);

	writePartList(partCounts, copyCountMap, parametricStlFileNames, std::string("RobotPartList.txt"));
}

std::map<std::string, unsigned int> Print3DGenerator::countParts(const robogenMessage::Robot &robotMessage)
{
	/* Create map part type -> count (number of times the part is used in the robot)*/
	const robogenMessage::Body &body = robotMessage.body();
	std::map<std::string, unsigned int> partCountMap;

	for (int i = 0; i < body.part_size(); i++) 
	{
		const robogenMessage::BodyPart &bodyPart = body.part(i);
		partCountMap[bodyPart.type()]++;
	}
	return partCountMap;
}

std::map<std::string, unsigned int> Print3DGenerator::countCopyFiles(const robogenMessage::Robot &robotMessage)
{
	/* Create map part type -> count (number of times the part is used in the robot)*/
	const robogenMessage::Body &body = robotMessage.body();
	std::map<std::string, unsigned int> copyCountMap;
	
	for (int i = 0; i < body.part_size(); i++) 
	{
		const robogenMessage::BodyPart &bodyPart = body.part(i);

		// find element of STL_PART_NAMES_MAP with key bodyPart.type()
		std::map <std::string, std::map <std::string, unsigned int > >::const_iterator it_find = STL_PART_NAMES_MAP.find(bodyPart.type());
		if(it_find == STL_PART_NAMES_MAP.end())
		{
			continue;
		}
		const std::map <std::string, unsigned int >& fileCountMap = it_find->second;

		std::map<std::string, unsigned int>::const_iterator it;
		for(it = fileCountMap.begin(); it != fileCountMap.end();it++)
		{
			copyCountMap[it->first] += it->second;
		}
	}
	return copyCountMap;
}

bool Print3DGenerator::writePartList(const std::map<std::string, unsigned int> &partCounts,
										const std::map<std::string, unsigned int> &copyCountMap,
										const std::vector<std::string> &parametricStlFileNames,
										const std::string &partListFileName)
{	
	//Write the number of parts of each type to file
	std::stringstream outputFilePath;
	outputFilePath << outputFolder_ << "/" << partListFileName;
	std::ofstream outputFile(outputFilePath.str());

	/* open output file*/ 
	if(!outputFile)
	{
		std::cerr << "OpenSCADFileWriter: could not create PartList file: " << outputFilePath.str() << std::endl;
		return false;
	}

	/* write part list to file */
	outputFile << "List of parts contained in robot:" << std::endl;
	std::map<std::string, unsigned int>::const_iterator it;
	for(it = partCounts.begin(); it != partCounts.end();it++)
	{
		outputFile << it->first << ": " << it->second <<std::endl;
	}

	outputFile << "----------------------------" << std::endl;
	outputFile << "List of stl files to 3D print:" << std::endl;
	for(it = copyCountMap.begin(); it != copyCountMap.end();it++)
	{
		outputFile << it->first << ".stl" << ": " << it->second <<std::endl;
	}

	outputFile << "--Specific parts:" << std::endl;
	std::vector<std::string>::const_iterator it_parametric;
	for(it_parametric = parametricStlFileNames.begin(); it_parametric != parametricStlFileNames.end();it_parametric++)
	{
		outputFile << *it_parametric <<std::endl;
	}
	
	outputFile.close();

	return true;
}

std::vector<std::string> Print3DGenerator::writeParametricStlFiles(const robogenMessage::Robot &robotMessage)

	/*

	////ParametricJoint 							
	
	CoreComponent, CoreComponentNoIMU & FixedBrick 	– FixedBrick

	ActiveHinge 									– ActiveHingeFrame & Active HingeServoHolder

	PassiveHinge 									– PassiveHinge (x2)

	LightSensor 									– LightSensor  

	IrSensor 										– IrSensor

	////ActiveWheel									– ActiveWheelMotorHolder
	
	////ActiveWheg									– ActiveWheelMotorHolder

	////PassiveWheel								– PassiveWheelPlate & Ring

	*/ 
{
	// iterate over all body parts
	const robogenMessage::Body &body = robotMessage.body();
	std::vector<std::string> parametricStlFileNames;

	for (int i = 0; i < body.part_size(); i++) 
	{
		const robogenMessage::BodyPart &bodyPart = body.part(i);

		/* check if is parametrized part */
		if(bodyPart.evolvableparam_size() > 0)
		{
			//std::cout << "Found parametrized part " << bodyPart.id() << " of type " << bodyPart.type() << " with " << bodyPart.evolvableparam_size() << " parameters" << std::endl;
			std::vector<float> parameters;
			/* iterate over all parameters of bodyPart and add value to parameters */
			for (int j = 0; j < bodyPart.evolvableparam_size(); j++)
			{
				parameters.push_back(bodyPart.evolvableparam(j).paramvalue());
			}

			/* write oscad file */ 
			std::vector<std::string> fileNames = oscadWriter_.createStlFiles(bodyPart.type(), bodyPart.id(), parameters);
			parametricStlFileNames.insert(parametricStlFileNames.end(), fileNames.begin(), fileNames.end()); // append filenames to list
		}
	}
	return parametricStlFileNames;
}

bool Print3DGenerator::copyGenericStlFiles(const std::map<std::string, unsigned int> copyCountMap)
{
	// check if we can make a system call
	if(!system(NULL))
	{
		std::cout << "[Print3DGenerator]: Could not make system call" << std::endl;
		return false;
	}

	bool success = true;

	//Create iterator to find current part file name
	std::map<std::string, unsigned int> ::const_iterator it;
	for(it = copyCountMap.begin(); it != copyCountMap.end(); it++)
	{
		std::string fileName = it->first;
		unsigned int count = it->second;

		//create input file name
		std::stringstream inputFileName;
		inputFileName << STLFileFolder_ << "/" << fileName << ".stl";
		//create file name
		std::stringstream outputFileName;
		outputFileName << outputFolder_ << "/" << fileName << "_print_" << count <<"_times.stl";

		//copy stl file
		std::stringstream command;
		command << "cp " << inputFileName.str() << " " << outputFileName.str();

		std::cout << command.str() << ":" << std::endl;
		int result = system(command.str().c_str());
		success &= (result == 0);
	}
	return success;
}