#include "routing.h"
#include "coreDesign.h"
#include "terminal.h"
void routing::route() {
	addObstacleBounding();
	for (coalescedNet* internalCoalescedNet : core->internalCoalescedNets) {
		currentNet = internalCoalescedNet;
		splicedTerminalSet tSet;
		for (auto&& mT_pair : internalCoalescedNet->connectedModuleSplicedTerminalMap) {
			for (splicedTerminal* t : mT_pair.second) {
				tSet.insert(t);
			}
		}

		if (internalCoalescedNet->connectedModuleSplicedTerminalMap.size() < 2)
			throw std::runtime_error("Dangling net??");

		splicedTerminal* t0 = tSet.pop();
		splicedTerminal* t1 = tSet.pop();
		initNet(t0, t1);
		while (!tSet.empty()) {
			expandNet(tSet.pop());
		}
	}
}

void routing::addObstacleBounding() {
	hObstacleSet.insert(
			new obstacleSegment{INT32_MIN, INT32_MIN, INT32_MAX, obstacleSegment::obstacleType::module, nullptr});
	hObstacleSet.insert(
			new obstacleSegment{INT32_MAX, INT32_MIN, INT32_MAX, obstacleSegment::obstacleType::module, nullptr});
	vObstacleSet.insert(
			new obstacleSegment{INT32_MIN, INT32_MIN, INT32_MAX, obstacleSegment::obstacleType::module, nullptr});
	vObstacleSet.insert(
			new obstacleSegment{INT32_MAX, INT32_MIN, INT32_MAX, obstacleSegment::obstacleType::module, nullptr});

	for (namedModulePair& m_pair : core->subModules) {
		intPair mPos = m_pair.second->position;
		intPair mSize = m_pair.second->size;
		hObstacleSet.insert(
				new obstacleSegment{mPos.y, mPos.x, mPos.x + mSize.x, obstacleSegment::obstacleType::module, nullptr});
		hObstacleSet.insert(new obstacleSegment{
				mPos.y + mSize.y, mPos.x, mPos.x + mSize.x, obstacleSegment::obstacleType::module, nullptr});
		vObstacleSet.insert(
				new obstacleSegment{mPos.x, mPos.y, mPos.y + mSize.y, obstacleSegment::obstacleType::module, nullptr});
		vObstacleSet.insert(new obstacleSegment{
				mPos.x + mSize.x, mPos.y, mPos.y + mSize.y, obstacleSegment::obstacleType::module, nullptr});
	}

	for (splicedTerminal* sysT : core->systemModule.moduleSplicedTerminals) {
		hObstacleSet.insert(new obstacleSegment{sysT->placedPosition.y, sysT->placedPosition.x, sysT->placedPosition.x,
				obstacleSegment::obstacleType::module, nullptr});
		vObstacleSet.insert(new obstacleSegment{sysT->placedPosition.x, sysT->placedPosition.y, sysT->placedPosition.y,
				obstacleSegment::obstacleType::module, nullptr});
	}
}

void routing::initNet(splicedTerminal* t0, splicedTerminal* t1) {
	initActives(A,t0);
	initActives(B,t1);
	solutionFound = straightLine(t0,t1);
	if(!solutionFound) {
		hashlib::pool<activeSegment*> A_new;
		expandActives(A,A_new);
		if(!solutionFound) {
			if(A_new.empty()) {
				throw std::runtime_error("Need Another Initialization???");
				solutionFound = true;
			}
			else {
				for (activeSegment* a : A_new)
					A.insert(a);
				hashlib::pool<activeSegment*> B_new;
				expandActives(B,B_new);
				if(!solutionFound) {
					if(B_new.empty()) {
						throw std::runtime_error("Need Another Initialization???");
						solutionFound = true;
					}
					else {
						for (activeSegment* a : B_new)
							B.insert(a);
					}
				}
			}
		}
	}
}

void routing::initActives(hashlib::pool<activeSegment*>& activeSet, const splicedTerminal* t) {
	if (t->isSystemTerminal()) {
		activeSet.insert(new activeSegment{segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
				direction::none, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x}, direction::up});
		activeSet.insert(new activeSegment{segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
				direction::none, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x}, direction::down});

		activeSet.insert(new activeSegment{segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
				direction::none, segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y}, direction::left});
		activeSet.insert(
				new activeSegment{segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y}, direction::none,
						segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y}, direction::right});
	} else {
		switch (t->baseTerminal->side) {
			case terminalSide::leftSide:
				activeSet.insert(new activeSegment{segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::none, segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::left});
				break;
			case terminalSide::rightSide:
				activeSet.insert(new activeSegment{segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::none, segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::right});
				break;
			case terminalSide::topSide:
				activeSet.insert(new activeSegment{segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::none, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::up});
				break;
			case terminalSide::bottomSide:
				activeSet.insert(new activeSegment{segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::none, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::down});
				break;
			case terminalSide::noneSide:
				throw std::runtime_error("Cannot have a terminal without a side");
		}
	}
}

bool routing::expandActives(
		hashlib::pool<activeSegment*>& actSegmentSet, hashlib::pool<activeSegment*>& incrementedActSegmentSet) {
	int minCrossover = INT32_MAX, minLength = INT32_MAX;
	int j = 0;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->currentSegment.index);

	bool solved = false;

	for (activeSegment* s : actSegmentSet) {
		if (s->currentSegment.index == j)
			solved |= expandSegment(s, incrementedActSegmentSet);
	}

	if (solved)
		reconstructSolution();
	return solved;
}

void routing::reconstructSolution() {
	intPair optimum;
	activeSegment *optA, *optB;
	coalescedNet* currentNet;
}

void routing::expandNet(splicedTerminal* pT) {}

bool routing::expandSegment(routing::activeSegment* actSegment, hashlib::pool<activeSegment*>& actSegmentSet) {
	hashlib::pool<endSegment*> endSet;
	bool solved = false;
	hashlib::pool<activeSegment *> aSideSet, bSideSet;
	struct simpleSegment {
		int end1, end2, crossovers;
		simpleSegment(int end1, int end2, int crossovers) : end1(end1), end2(end2), crossovers(crossovers) {}
	};
	std::vector<simpleSegment *> R, R_dash;
	R.emplace_back(new simpleSegment{
			actSegment->currentSegment.end1, actSegment->currentSegment.end2, actSegment->crossedNets});

	int i = actSegment->currentSegment.index;
	while (!R.empty()) {
		hashlib::pool<obstacleSegment*> obsSet;

		int tempDistance = INT32_MAX;
		obstacleSegment* closestObstacle = nullptr;
		for (obstacleSegment* hObstacle : hObstacleSet) {
			int temp = i - hObstacle->index;
			if (temp > 0 && tempDistance > temp) {
				tempDistance = temp;
				closestObstacle = hObstacle;
			}
		}
		obsSet.insert(closestObstacle);

		R_dash = R;
		while (!R_dash.empty()) {
			simpleSegment* r = R_dash.back();
			R_dash.pop_back();

			// TODO:overlap condition
			while (true) {
				obstacleSegment* obst = nullptr;
				R.erase(std::find(R.begin(), R.end(), r));

				if (r->end1 < obst->end1) {
					// TODO: Can be optimized!! remove -1 thing and optimize it
					R.emplace_back(new simpleSegment(r->end1, obst->end1 - 1, r->end1));
					R_dash.emplace_back(new simpleSegment(r->end1, obst->end1 - 1, r->end1));
					r->end1 = obst->end1;
				}
				if (r->end2 > obst->end2) {
					// TODO: Can be optimized!! remove -1 thing and optimize it
					R.emplace_back(new simpleSegment(obst->end2 + 1, r->end2, r->end1));
					R_dash.emplace_back(new simpleSegment(obst->end2 + 1, r->end2, r->end1));
					r->end1 = obst->end1;
				}
				if (obst->type == obstacleSegment::obstacleType::module)
					moduleObstacle(i, r, actSegment->currentDirection);
				else if (obst->type == obstacleSegment::obstacleType::net && obst->net != currentNet)
					netObstacle(R,i,r,obst);
				else if (obst->type == obstacleSegment::obstacleType::startA && )
			}
		}
	}

	return solved;
}
