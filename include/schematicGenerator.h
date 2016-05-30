#ifndef PLACEMENT_H
#define PLACEMENT_H
#include "module.h"
#include "net.h"

class schematicGenerator {

private:


	module systemModule;    //just a place holder
	namedModuleCollection subModules;
	namedNetCollection internalNets;

	hashlib::pool<coalescedNet*> coalescedNetSet;

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

	schematicGenerator() : systemModule("topModule") { }

	~schematicGenerator();

	void doPlacement();

	void parseJson(std::string jsonFile);

	terminal &addSystemTerminal(const std::string &terminalIdentifier, const schematic::terminalType type, const int width);

	terminal & getSystemTerminal(const std::string &terminalIdentifier);

	module & addModule(const std::string &moduleName);

	module & getModule(const std::string &moduleName) ;

	net &addNet(const std::string &netName, const int netWidth);

	net & getNet(const std::string &netName);


};

#endif // PLACEMENT_H