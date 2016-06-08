#ifndef BOX_H
#define BOX_H
#include "common.h"
#include "module.h"

class box {
	friend class schematicGenerator;

private:
	partition* parentPartition;
	std::vector<module*> boxModules;  // NOTE: check if using vector is fine here
	std::vector<std::pair<splicedTerminal*, splicedTerminal*>> boxLink;
	intPair moduleOffset, boxPosition, boxSize;

public:
	box(module* m) {
		m->setParentBox(this);
		boxModules.emplace_back(m);
	}
	void add(module* m, splicedTerminal* src, splicedTerminal* sink);
	void setParentPartition(partition* p) {
		parentPartition = p;
	}
	unsigned int length() const {
		return static_cast<unsigned int>(boxModules.size());
	}
	bool empty() {
		return boxModules.empty();
	}
};

#endif  // BOX_H
