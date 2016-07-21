#include "box.h"
#include "moduleImpl.h"

void box::add(moduleImpl* m, splicedTerminal* src, splicedTerminal* sink) {
	m->setParentBox(this);
	boxModules.emplace_back(m);
	boxLink.emplace_back(src, sink);
}
