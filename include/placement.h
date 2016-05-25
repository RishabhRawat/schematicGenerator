#ifndef PLACEMENT_H
#define PLACEMENT_H
#include "module.h"
#include "net.h"
//#include <unordered_set>

class placement
{

private:

	module systemModule;    //just a place holder
	namedModuleCollection subModules;
	namedNetCollection internalNets;
	std::string systemIdentifier;

	partitionCollection allPartitions;

	//Important Layout Parameters
	unsigned int maxPartitionSize;
	unsigned int maxPartitionConnections;
	unsigned int maxPathLength;

	void initializeStructures();

	void partitionFormation();
	module *selectSeed(hashlib::pool<module *> moduleSet) const;
//	int connectionsToExistingPartitions(module *m);
	partition *createPartition(hashlib::pool<module *> &moduleSet, module *seed);

	void boxFormation();
	moduleCollection selectRoots(partition *p);
	box * selectPath( box *rootBox, box *remainingModules);


	void modulePlacement() {};
	void boxPlacement() {};
	void partitionPlacement() {};
	void terminalPlacement() {};



public:

	//API FUNCTIONS

	placement() : systemModule("topModule") { }

	~placement();

	void doPlacement();

	void parseJson(std::string jsonFile);

	const std::string &Identifier() const {
		return systemIdentifier;
	}

	terminal & addSystemTerminal(const std::string &terminalIdentifier, const schematic::terminalType type);

	terminal & getSystemTerminal(const std::string &terminalIdentifier);

	module & addModule(const std::string &moduleName);

	module & getModule(const std::string &moduleName) ;

	net & addNet(const std::string &netName);

	net & getNet(const std::string &netName);
};

#endif // PLACEMENT_H
