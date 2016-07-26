#ifndef COREDESIGN_H
#define COREDESIGN_H

#include <json/json.hpp>
#include "moduleImpl.h"

class coreDesign {
	friend class schematic;
	friend class placement;
	friend class routing;

	moduleImpl systemModule;  // just a place holder
	namedModuleCollection subModules;

	schematicParameters designParameters;

	intPair size, offset;
	netSet internalNets;

	coreDesign() : systemModule("topModule") {}
	~coreDesign();

	void initializeStructures();

	void doPlacement(schematicParameters parameters);
	std::string doRouting();

	std::string createDebugJsonSchematicFromJson();
	std::string createJsonSchematicFromJson();

	std::string exportRoutingJson();
};

#endif  // PLACEMENT_H
