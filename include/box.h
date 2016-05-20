#ifndef BOX_H
#define BOX_H
#include "common.h"

class box {
	friend class placement;
private:
	partition *parentPartition;
	hashlib::pool<module*> boxModules;
public:
	void add(module *m);
	void remove(module *m);
	void setParentPartition(partition *p){parentPartition=p;}
	size_t size() { return boxModules.size();}
	bool empty() { return boxModules.empty();}
};

#endif // BOX_H
