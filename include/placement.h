#ifndef PLACEMENT_H
#define PLACEMENT_H
#include <module.h>
#include <net.h>
//#include <unordered_set>

class placement
{


private:
//    std::unordered_set<modules> subModules;
//    std::unordered_set<nets> internalNets;
	module systemModule;
	std::vector<module> subModules;
	std::vector<net> internalNets;
	std::vector<terminal> systemTerminals;

	// Do not ask me why... just gut feel...
	// Instead give me a reason saying this is wrong
	hashlib::pool<partition*> allPartitions;

	//Important Layout Parameters
	unsigned int maxPartitionSize;
	unsigned int maxPartitionConnections;
	unsigned int maxPathLength;

	void initializeStructures();

	void partitionFormation();
	module* selectSeed(hashlib::pool<module*> moduleSet);
	int connectionsToExistingPartitions(module *m);
	partition *createPartition(hashlib::pool<module *> &moduleSet, module *seed);

	void boxFormation();
	moduleCollection selectRoots(partition *p);
	box * selectPath( box *rootBox, box *remainingModules);


	void modulePlacement();
	void boxPlacement();
	void partitionPlacement();
	void terminalPlacement();



public:
	placement();
	~placement();

	void doPlacement();
	void addModule();
};

#endif // PLACEMENT_H
