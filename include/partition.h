#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
	friend class schematicGenerator;

private:
	std::vector<box*> partitionBoxes;
	moduleSet partitionModules;
	intPair offset, position, size;

	// used after box formation
	void addModule(module* m);
	bool contains(module* m);

public:
	void add(box* b);
	unsigned int length() {
		return static_cast<unsigned int>(partitionBoxes.size());
	}
	~partition();
	intPair getVertex(const int index) {
		switch (index) {
		case 0:
			return position;
		case 1:
			return position + size.component(1);
		case 2:
			return position + size;
		case 3:
			return position + size.component(0);
		default:
			throw std::invalid_argument("Invalid index");
		}
	};
};

#endif  // PARTITION_H
