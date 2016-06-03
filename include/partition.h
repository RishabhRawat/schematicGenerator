#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
	friend class schematicGenerator;
private:
	hashlib::pool<box*> partitionBoxes;
	moduleSet partitionModules; //NOTE: Should it be heap allocated, as it is not used after box formation
	void addModule(module *m);
	bool contains(module *m);
public:
	void add(box* b);
	unsigned int size() {return static_cast<unsigned int>(partitionBoxes.size());}
	~partition();


};

#endif // PARTITION_H
