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

	template <typename T>
	struct positionalStructure {
		T* item;
		std::vector<const T*> side[4];  // clockwise from top
		void add(const T* const newItem) {
			if (item == newItem)
				return;
			if (item->position.x > newItem->position.x)
				side[3].push_back(newItem);
			if (item->position.y > newItem->position.y)
				side[2].push_back(newItem);
			if (item->position.x + item->size.x < newItem->position.x + newItem->size.x)
				side[1].push_back(newItem);
			if (item->position.x + item->size.y > newItem->position.x + newItem->size.y)
				side[0].push_back(newItem);
		}
	};

	module systemModule;  // just a place holder
	namedModuleCollection subModules;
	namedNetCollection internalNets;

	intPair size, offset;
	hashlib::pool<coalescedNet*> coalescedNetSet;

	partitionCollection allPartitions;

	// Important Layout Parameters, just guesses for now
	unsigned int maxPartitionSize = 20;
	unsigned int maxPartitionConnections = 20;
	unsigned int maxPathLength = 10;

	void initializeStructures();

	void partitionFormation();

	module* selectPartitionSeed(hashlib::pool<module*> moduleSet) const;

	//	int connectionsToExistingPartitions(module *m);
	partition* createPartition(hashlib::pool<module*>& moduleSet, module* seed);

	void boxFormation();

	moduleSet* selectBoxSeeds(partition* p);

	box* selectPath(box* rootBox, moduleSet remainingModules);

	void modulePlacement();

	void initModulePlacement(box* b, intPair& leftBottom, intPair& rightTop);

	void placeModule(box* b, unsigned int index, intPair& pair, intPair& intPair);

	int calculatePadding(unsigned int n);

	void boxPlacement();  // SHARING BOUNDARIES IS PERFECTLY LEGAL

	box* selectNextBox(const hashlib::pool<box*, hashlib::hash_ops<box*>>& remainingBoxes,
			const hashlib::pool<box*, hashlib::hash_ops<box*>>& placedBoxes);

	intPair calculateOptimumBoxPosition(const box* b, hashlib::pool<box*>& placedBoxes);

	template <typename T>
	intPair calculateActualPosition(
			const intPair size, const intPair optimumPosition, hashlib::dict<T*, positionalStructure<T>>& layoutData);

	void partitionPlacement();  // SHARING BOUNDARIES IS PERFECTLY LEGAL

	partition* selectNextParition(
			hashlib::pool<partition*> remainingPartition, hashlib::pool<partition*> placedPartition);

	intPair calculateOptimumPartitionPosition(partition* p, hashlib::pool<partition*>& placedPartition);

	void systemTerminalPlacement();

	void printInitialStructures();

	void printDerivedStructures();

	void printPartitions();

public:
	// API FUNCTIONS

	schematicGenerator() : systemModule("topModule") {}

	~schematicGenerator();

	void doPlacement();

	void parseJson(std::string jsonFile);

	terminal& addSystemTerminal(
			const std::string& terminalIdentifier, const schematic::terminalType type, const int width);

	terminal& getSystemTerminal(const std::string& terminalIdentifier);

	module& addModule(const std::string& moduleName);

	module& getModule(const std::string& moduleName);

	net& addNet(const std::string& netName, const int netWidth);

	net& getNet(const std::string& netName);
};

#endif  // PLACEMENT_H
