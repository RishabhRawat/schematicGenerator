#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include "common.h"

class schematic {
private:
	coreDesign *pSchematicGenerator;

public:
	schematicParameters designParameters;
	schematic();

	virtual ~schematic();

	void parseJsonFile(std::string jsonFile);
	void doPlacement();
	void doRouting();
	std::string createDetailedJsonSchematicFromJson(std::string jsonData);
	std::string createJsonSchematicFromJson(std::string jsonData);
};

#endif //SCHEMATIC_H