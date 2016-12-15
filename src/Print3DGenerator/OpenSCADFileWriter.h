#include <vector>
#include <string>
#include <map>
#include "PartList.h"


class OpenSCADFileWriter
{
public:
	/**
	 * Generates the map of openscad commands to use to generate cad files of parametric parts.
	 *
	 * @param openSCADTemplateFolder 	tpath of the location of openscad template files
	 * @return outputFolder 	path of folder where the files are output 	
	 */
	OpenSCADFileWriter(const std::string &openSCADTemplateFolder, const std::string &outputFolder);

	/**
	 * Genrates the openscad code and compiles it to stl files of the parametric parts of the robot.
	 *
	 * @param partType type of parametric part
	 * @param partName 	name of parametric part
	 * @param parameters  	parameters of parametric part
	 * @return stlPartNamesMap 	const map from part type to corresponding stl part names and number of times they should be 3D printed
	 */
	std::vector<std::string> createStlFiles(const std::string &partType, const std::string &partName, const std::vector<float> &parameters);

private:

	static bool isCodeLine(const std::string &line);

	static const std::map<std::string, std::vector<std::string> > initFuncNamesMap();

	static const std::map<std::string, std::vector<std::string> > FUNC_NAMES_MAP;

	std::string openSCADTemplateFolder_;
	std::string outputFolder_;

};
