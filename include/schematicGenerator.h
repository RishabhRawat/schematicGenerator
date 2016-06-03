#ifndef PLACEMENT_H
#define PLACEMENT_H
#include "module.h"
#include "net.h"

class schematicGenerator {

private:

	struct schematicParameters {
		/*
		 * This is the distance used in placing modules reserved for string wires
		 */
		unsigned int wireModuleDistance = 5;

	} designParameters;


	module systemModule;    //just a place holder
	namedModuleCollection subModules;
	namedNetCollection internalNets;

	hashlib::pool<coalescedNet*> coalescedNetSet;

	partitionCollection allPartitions;

	//Important Layout Parameters, just guesses for now
	unsigned int maxPartitionSize = 20;
	unsigned int maxPartitionConnections = 20;
	unsigned int maxPathLength = 10;

	void initializeStructures();

	void partitionFormation();
	module *selectPartitionSeed(hashlib::pool<module *> moduleSet) const;
//	int connectionsToExistingPartitions(module *m);
	partition *createPartition(hashlib::pool<module *> &moduleSet, module *seed);

	void boxFormation();
	moduleSet * selectBoxSeeds(partition *p);
	box * selectPath(box *rootBox, moduleSet remainingModules);


	void modulePlacement();
	void initModulePlacement(box *b, intPair &leftBottom, intPair &rightTop);
	void placeModule(box *b, unsigned int index, intPair &pair, intPair &intPair);
	unsigned int calculatePadding(unsigned int n);
	void boxPlacement() {};
	void partitionPlacement() {};
	void terminalPlacement() {};

	void printInitialStructures();
	void printDerivedStructures();
	void printPartitions();


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
