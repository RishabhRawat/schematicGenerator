#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
	friend class schematicGenerator;

private:
	std::vector<box*> partitionBoxes;
	moduleSet partitionModules;

	// used after box formation
	void addModule(module* m);
	bool contains(module* m);

public:
	void add(box* b);
	unsigned int length() {
		return static_cast<unsigned int>(partitionBoxes.size());
	}
	~partition();
};

#endif  // PARTITION_H
