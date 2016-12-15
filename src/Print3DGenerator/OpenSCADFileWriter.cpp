#include "Print3DGenerator/OpenSCADFileWriter.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <map>


OpenSCADFileWriter::OpenSCADFileWriter(const std::string &openSCADTemplateFolder, const std::string &outputFolder) : openSCADTemplateFolder_(openSCADTemplateFolder), outputFolder_(outputFolder) {}

const std::map<std::string, std::vector<std::string> > OpenSCADFileWriter::initFuncNamesMap()
{
	std::map<std::string, std::vector<std::string> > funcNamesMap;

	funcNamesMap[PART_TYPE_ACTIVE_WHEEL].push_back("activeWheel");
	funcNamesMap[PART_TYPE_PASSIVE_WHEEL].push_back("passiveWheel");
	funcNamesMap[PART_TYPE_ACTIVE_WHEG].push_back("activeWheg");
	funcNamesMap[PART_TYPE_PARAM_JOINT].push_back("parametricJointPartA");
	funcNamesMap[PART_TYPE_PARAM_JOINT].push_back("parametricJointPartB");

	return funcNamesMap;
}

std::vector<std::string> OpenSCADFileWriter::createStlFiles(const std::string &partType, const std::string &partName, const std::vector<float> &parameters)
{
	/*

	////ParametricJoint 							– ParametricJoint_partA & ParametricJoint_partB
	
	CoreComponent, CoreComponentNoIMU & FixedBrick 	

	ActiveHinge 									

	PassiveHinge 										

	LightSensor 								

	IrSensor 										 

	////ActiveWheel									– ActiveWheel
	
	////ActiveWheg									– ActiveWheg

	////PassiveWheel								– PassiveWheel

	*/ 

	// List of names of created files
	std::vector<std::string> fileNames;

	// check if we can make a system call
	if(!system(NULL))
	{
		std::cout << "[OpenSCADFileWriter]: Could not make system call" << std::endl;
		return fileNames;
	}

	//create temporary oscad file name
	std::stringstream tempFileName;
	tempFileName << openSCADTemplateFolder_  << "/temp.scad";

	int success = true;

	//Create iterator to find current part openscad function name to be called
	const std::map<std::string, std::vector<std::string> >::const_iterator func_names_it = FUNC_NAMES_MAP.find(partType);
	// If part Type found in map
	if(func_names_it != FUNC_NAMES_MAP.end())
	{
		const std::vector<std::string> func_names = func_names_it->second;

		//Create iterator for function names to be called for part
		std::vector<std::string>::const_iterator it_func;
		int counter = 1;
		for(it_func = func_names.begin(); it_func != func_names.end(); it_func++)
		{
			//create .scad file
			std::ofstream tempFile(tempFileName.str());
			tempFile << "include <include_list.scad>" << std::endl;
			tempFile << (*it_func) << "(";

			//Create iterator for parameters needed for part
			std::vector<float>::const_iterator it_param = parameters.begin();
			while(true)
			{
				tempFile << *(it_param++);
				if(it_param != parameters.end())
				{
					tempFile << ", ";
				}else
				{
					tempFile << ");" << std::endl;
					break;
				}
			}

			//create stl file
			std::stringstream outputFileName;
			outputFileName << partType << "_" << partName << counter << ".stl";

			std::stringstream command;
			command << "openscad -o " << outputFolder_ << "/" << outputFileName.str() <<  " " << tempFileName.str();

			std::cout << command.str() << ":" << std::endl;
			int result = system(command.str().c_str());
			if(result == 0) // command executed successfully
			{
				fileNames.push_back(outputFileName.str());		
			}
			else
			{
				success = false;
			}
			counter++;
		}
	}

	return fileNames;
}

const std::map<std::string, std::vector<std::string> > OpenSCADFileWriter::FUNC_NAMES_MAP = OpenSCADFileWriter::initFuncNamesMap();
