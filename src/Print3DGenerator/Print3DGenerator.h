/*
 * @(#) 3DPrintGenerator.h   1.0   Aug, 22, 2016
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

#include "../../build/robogen.pb.h"
#include "Print3DGenerator/OpenSCADFileWriter.h"
#include <fstream>



class Print3DGenerator
{
public:

enum PrinterType{
		SLS,
		FDM
	};
	

	Print3DGenerator(const std::string &robotFileName, const std::string &outputFolder, PrinterType printerType);
	Print3DGenerator(const std::string &robotFileName, const std::string &outputFolder, const std::string &inputFolder, PrinterType printerType);
	
	/**
	 * Runs all the functions to generate and copy stl files needed for 3D printing robot parts
	 *
	 * @return true if success	
	 */ 
	bool create3DFiles();

private:

	/**
	 * Generates the (const) map of generic stl file names associated with each part.
	 *
	 * @param printerType 	type of 3D printer. SLS or FDM. Default is SLS
	 * @return stlPartNamesMap 	const map from part type to corresponding stl part names and number of times they should be 3D printed
	 */
	static const std::map<std::string, std::map <std::string, unsigned int > > initSTLPartNamesMap(PrinterType printerType);

	/**
	 * Counts all the parts of the robot.
	 *
	 * @param robotMessage 	message describing the robot
	 * @return partCountMap 	map of all parts to the corresponding number of times in the robot
	 */
	std::map<std::string, unsigned int> countParts(const robogenMessage::Robot &robotMessage);

	/**
	 * Counts the generic (non-parametric) stl parts to be 3D printed for the robot. These need only be copied, not generated as opposed to the parametric parts.
	 *
	 * @param robotMessage 	message describing the robot
	 * @return copyCountMap 	map of stl part names to the number of times to be 3D printed
	 */
	std::map<std::string, unsigned int> countCopyFiles(const robogenMessage::Robot &robotMessage);


	/**
	 * Copies the stl files for the generic parts, according to the copyCountMap map.
	 *
	 * @param copyCountMap 	map of stl part names to the number of times to be 3D printed
	 * @return true if success	 
	 */
	bool copyGenericStlFiles(const std::map<std::string, unsigned int> copyCountMap);

	/**
	 * Generates STL files for parametric parts of a robot. Uses openscad via OpenSCADFileWriter.cpp.
	 *
	 * @param robotMessage 	message describing the robot
	 * @return fileNameList 	list of names of generated files
	 */
	std::vector<std::string> writeParametricStlFiles(const robogenMessage::Robot &robotMessage);

	
	/**
	 * Generates a human readable part list file (RobotPartList.txt) based on the counted parts.
	 *
	 * @param partCounts 	map comtaining the parts of the robot and their number
	 * @param copyCountMap 	map containing the generic (non-parametric) stl files to be copied
	 * @param parametricStlFileNames 	vector containing the names of parametric part stl files generated 
	 * @param partListFileName 	name to give the the partlist file
	 * @return true if success
	 */
	bool writePartList(const std::map<std::string, unsigned int> &partCounts,
						const std::map<std::string, unsigned int> &copyCountMap,
						const std::vector<std::string> &parametricStlFileNames,
						const std::string &partListFileName);


	const std::map<std::string, std::map <std::string, unsigned int > > STL_PART_NAMES_MAP;

	std::string robotFileName_;
	PrinterType printerType_;

	std::string STLFileFolder_;
	std::string outputFolder_;

	std::ofstream partListFile_;

	OpenSCADFileWriter oscadWriter_;
};
