#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
	friend class schematicGenerator;
private:
	hashlib::pool<box*> partitionBoxes;
public:
	void add(box* b);
	size_t size() {return partitionBoxes.size();};
};

#endif // PARTITION_H
