/*
 * @(#) FileViewer.cpp   1.0   Mar 5, 2013
 *
 * Andrea Maesani (andrea.maesani@epfl.ch)
 * Titus Cieslewski (dev@titus-c.ch)
 * Joshua Auerbach (joshua.auerbach@epfl.ch)
 *
 * The ROBOGEN Framework
 * Copyright © 2012-2016 Andrea Maesani, Titus Cieslweski, Joshua Auerbach
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

using namespace robogen;


void printUsage(char* argv[])
{
	std::cout << std::endl << "USAGE: " << std::endl << "      "
			<< std::string(argv[0]) << " <ROBOT_FILE, STRING> "
			<< "-o or --output  <OUTPUT_FILE_PATH, STRING> [<OPTIONS>]"
			<< std::endl << "WHERE: " << std::endl
			<< "<OUTPUT_FILE_PATH, STRING>" << std::endl
			<< "Where the files are output." << std::endl << std::endl
			<< "[OPTIONS]" << std::endl
			<< "-i or --input  <INPUT_FILE_PATH. STRING> " << std::endl
			<< "Where the folder printing3D is located. Default is ../" << std::endl << std::endl
			<< "--printerType  <PRINTERTYPE, STRING>" << std::endl
			<< "The type of 3D printer on which parts will be printed. Default is FDM" << std::endl << std::endl << std::endl;
}

int main(int argc, char *argv[]) {

	if(argc < 2)
	{
		std::cerr << "error: No robotFile provided" << std::endl;
		printUsage(argv);
		return EXIT_FAILURE;
	}

	// print usage
	if (std::string(argv[1]) == "--help")
	{
		printUsage(argv);
		return EXIT_FAILURE;
	}

	std::string outputDir(".");
	std::string inputDir("..");
	Print3DGenerator::PrinterType printerType(Print3DGenerator::PrinterType::SLS);

	// interate over options
	for(int iArg = 2; iArg < argc; iArg++)
	{
		if((strcmp(argv[iArg], "--output") == 0) || (strcmp(argv[iArg], "-o") == 0))
		{
			if(argc < iArg + 2 || strncmp(argv[iArg+1], "-",1) == 0)
			{
				std::cerr << "In order to write output files, must provide "
										<< "directory."
										<< std::endl;
				printUsage(argv);
				return EXIT_FAILURE;
			}
			outputDir = argv[++iArg];
		}

		if((strcmp(argv[iArg], "--input") == 0) || (strcmp(argv[iArg], "-i") == 0))
		{
			if(argc < iArg + 2 || strncmp(argv[iArg+1], "-",1)  == 0)
			{
				std::cerr << "Please provide input path. "
										<< std::endl;
				printUsage(argv);
				return EXIT_FAILURE;
			}
			inputDir = argv[++iArg];
		}

		// Get PrinterType
		else if(strcmp(argv[iArg], "--printerType") == 0)
		{
			if(argc < iArg + 2 || strncmp(argv[iArg+1], "-",1) == 0)
			{
				std::cerr << "error: printerType not specified"	<< std::endl;
				printUsage(argv);
				return EXIT_FAILURE;
			}
			iArg++;
			if((strcmp(argv[iArg], "FDM") == 0) || (strcmp(argv[iArg], "fdm") == 0))
			{
				printerType = Print3DGenerator::PrinterType::FDM;
			}
			else if((strcmp(argv[iArg], "SLS") == 0) || (strcmp(argv[iArg], "sls") == 0))
			{
				printerType = Print3DGenerator::PrinterType::SLS;	
			}
			else
			{
				std::cerr << "error: printerType " << argv[iArg] << "not supported" << std::endl;
			}
		}
	}


	std::string robotFileString(argv[1]);

	// ---------------------------------------
	// Print3DGenerator
	// ---------------------------------------
	Print3DGenerator print3Dgenerator(robotFileString, outputDir, inputDir, printerType);
	/*std::stringstream command;
	command << "mkdir " << outputDir;
	system(command.c_str());*/
	print3Dgenerator.create3DFiles();

}
