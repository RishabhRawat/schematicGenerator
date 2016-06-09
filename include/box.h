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
	intPair offset, position, size;

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

#endif  // BOX_H
