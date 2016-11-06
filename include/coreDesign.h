#ifndef COREDESIGN_H
#define COREDESIGN_H

#include "json.hpp"
#include "module.h"

class coreDesign {
	friend class schematic;
	friend class placement;
	friend class routing;

	module systemModule;  // just a place holder
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
