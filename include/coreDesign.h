#ifndef COREDESIGN_H
#define COREDESIGN_H

#include <json/json.hpp>
#include "module.h"
#include "net.h"

class coreDesign {
	friend class placement;
	friend class routing;
private:
	module systemModule;  // just a place holder
	schematicParameters& designParameters;

	namedModuleCollection subModules;
	namedNetCollection internalNets;

	intPair size, offset;
	hashlib::pool<coalescedNet*> internalCoalescedNets;

	void initializeStructures();

	void printInitialStructures();

	void printDerivedStructures();

public:
	// API FUNCTIONS

	coreDesign(schematicParameters& designParameters) : systemModule("topModule"), designParameters(designParameters) {}
	~coreDesign();

	void doPlacement();
	void doRouting();

	void parseJson(nlohmann::json parsedJson);
	void parseJsonFile(std::string jsonFile);
	void parseJsonString(std::string jsonText);
	std::string createDebugJsonSchematicFromJson(std::string jsonData);
	std::string createJsonSchematicFromJson(std::string jsonData);

	terminal& addSystemTerminal(const std::string& terminalIdentifier, const terminalType type, const int width);
	terminal& getSystemTerminal(const std::string& terminalIdentifier);
	module& addModule(const std::string& moduleName);
	module& getModule(const std::string& moduleName);
	net& addNet(const std::string& netName, const int netWidth);

	net& getNet(const std::string& netName);
};

#endif  // PLACEMENT_H
