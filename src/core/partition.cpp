#include "partition.h"
#include "box.h"

void partition::add(box* b) {
	b->setParentPartition(this);
	partitionBoxes.emplace_back(b);
}

void partition::addModule(module* m) {
	partitionModules.insert(m);
}

partition::~partition() {
	for (box* b : partitionBoxes)
		delete b;
}

bool partition::contains(module* m) {
	return partitionModules.find(m) != partitionModules.end();
}