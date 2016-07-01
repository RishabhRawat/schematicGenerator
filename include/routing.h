#ifndef ROUTING_H
#define ROUTING_H

#include "common.h"

class routing {
	const coreDesign* core;

	void addObstacleBounding();
	void initNet(splicedTerminal* t0, splicedTerminal* t1);
	void expandNet(splicedTerminal* pTerminal);
	void route();

public:
	routing(coreDesign* core) : core(core) {}
};

#endif  // ROUTING_H
