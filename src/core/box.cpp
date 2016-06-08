#include "box.h"
#include "module.h"

void box::add(module* m, splicedTerminal* src, splicedTerminal* sink) {
	m->setParentBox(this);
	boxModules.emplace_back(m);
	boxLink.emplace_back(src, sink);
}
