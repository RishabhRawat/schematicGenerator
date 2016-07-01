#ifndef COREDESIGN_H
#define COREDESIGN_H

#include <json/json.hpp>
#include "module.h"
#include "net.h"

class coreDesign {
	friend class placement;
private:

	module systemModule;  // just a place holder
	schematicParameters& designParameters;

	namedModuleCollection subModules;
	namedNetCollection internalNets;

	intPair size, offset;
	hashlib::pool<coalescedNet*> internalCoalescedNets;


	void initializeStructures();

	void routing();

	void printInitialStructures();

	void printDerivedStructures();

public:
	// API FUNCTIONS

	coreDesign(schematicParameters& designParameters)
		: systemModule("topModule"), designParameters(designParameters) {}

	~coreDesign();

	void doPlacement();

	void parseJsonFile(std::string jsonFile);

	terminal& addSystemTerminal(
			const std::string& terminalIdentifier, const terminalType type, const int width);

	terminal& getSystemTerminal(const std::string& terminalIdentifier);

	module& addModule(const std::string& moduleName);

	module& getModule(const std::string& moduleName);

	net& addNet(const std::string& netName, const int netWidth);

	net& getNet(const std::string& netName);

	void parseJson(nlohmann::json parsedJson);

	void parseJsonString(std::string jsonText);

	exportStructure createSchematicFromJson(std::string jsonData);

	std::string createJsonSchematicFromJson(std::string jsonData);

	void addObstacleBounding();

	void initNet(splicedTerminal* t0, splicedTerminal* t1);

	void expandNet(splicedTerminal* pTerminal);
};

#endif  // PLACEMENT_H
