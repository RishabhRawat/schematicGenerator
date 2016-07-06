#include "routing.h"
#include <unordered_set>
#include "coreDesign.h"
#include "terminal.h"
const int strokeWidth = 1;
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
	initActives(activesA, t0);
	initActives(activesB, t1);
	solutionFound = straightLine(t0, t1);
	while (!solutionFound) {
		std::unordered_set<activeSegment*> A_new;
		expandActives(activesA, A_new);
		if (solutionFound)
			break;
		for (activeSegment* a : A_new)
			activesA.insert(a);

		std::unordered_set<activeSegment*> B_new;
		expandActives(activesB, B_new);
		if (solutionFound)
			break;
		for (activeSegment* a : B_new)
			activesB.insert(a);
	}
}

void routing::initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t) {
	switch (t->baseTerminal->side) {
		case terminalSide::leftSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y + strokeWidth,
					t->placedPosition.y + strokeWidth, direction::left, nullptr});
			break;
		case terminalSide::rightSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y + strokeWidth,
					t->placedPosition.y + strokeWidth, direction::right, nullptr});
			break;
		case terminalSide::topSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x + strokeWidth,
					t->placedPosition.x + strokeWidth, direction::up, nullptr});
			break;
		case terminalSide::bottomSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x + strokeWidth,
					t->placedPosition.x + strokeWidth, direction::down, nullptr});
			break;
		case terminalSide::noneSide:
			throw std::runtime_error("Cannot have a terminal without a side");
	}
}

void routing::expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
		std::unordered_set<activeSegment*>& incrementedActSegmentSet) {
	int minCrossover = INT32_MAX, minLength = INT32_MAX;
	int j = 0;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->bends);

	// TODO: correct this!!
	for (activeSegment* s : actSegmentSet) {
		if (s->index == j)
			generateEndSegments(s);
	}

	if (solutionFound)
		reconstructSolution();

	if (actSegmentSet.empty())
		throw("No solution found!!");
}

void routing::reconstructSolution() {
	intPair optimum;
	activeSegment *optA, *optB;  // NOTE: Somehow there is an assumption both have same direction (vertical/ horizontal)
	switch (optA->dir) {
		case direction::up:
		case direction::down:
			int min = optA->index;
			int max = optB->index;
			if (max < min)
				std::swap(min, max);
			vObstacleSet.insert(
					new obstacleSegment{optimum.x, min, max, obstacleSegment::obstacleType::net, currentNet});
			reconstructPath(optA, optimum.x);
			reconstructPath(optB, optimum.x);
			break;
		case direction::left:
		case direction::right:
			int min = optA->index;
			int max = optB->index;
			if (max < min)
				std::swap(min, max);
			hObstacleSet.insert(
					new obstacleSegment{optimum.y, min, max, obstacleSegment::obstacleType::net, currentNet});
			reconstructPath(optA, optimum.y);
			reconstructPath(optB, optimum.y);
			break;
	}

	for (auto&& iter = hObstacleSet.begin(); iter != hObstacleSet.end();) {
		if ((*iter)->type == obstacleSegment::obstacleType::startA ||
				(*iter)->type == obstacleSegment::obstacleType::startB)
			iter == hObstacleSet.erase(iter);
		else
			++iter;
	}

	for (auto&& iter = vObstacleSet.begin(); iter != vObstacleSet.end();) {
		if ((*iter)->type == obstacleSegment::obstacleType::startA ||
				(*iter)->type == obstacleSegment::obstacleType::startB)
			iter == vObstacleSet.erase(iter);
		else
			++iter;
	}
}

bool routing::generateEndSegments(
		activeSegment* actSegment, segment s, int crossovers, orderedObstacleSet& obstacleSet) {
	bool solved = false;
	orderedObstacleSet obstacles;
	for (auto&& o : obstacleSet) {
		if (s.end1 < o->end1 || o->end2 < s.end1)
			obstacles.insert(o);
	}
	obstacleSegment* obstacle =
			actSegment->isUpRight()
					? *obstacleSet.lower_bound(new obstacleSegment{s.index})
					: *std::reverse_iterator<orderedObstacleSet>(obstacleSet.upper_bound(new obstacleSegment{s.index}));
	if (s.end1 < obstacle->end1) {
		solved |= generateEndSegments(actSegment, segment{s.index, s.end1, obstacle->end1}, crossovers, obstacleSet);
	}
	if (obstacle->end2 < s.end2) {
		solved |= generateEndSegments(actSegment, segment{s.index, obstacle->end2, s.end2}, crossovers, obstacleSet);
	}
	switch (obstacle->type) {
		case obstacleSegment::obstacleType::module:
			E.insert(new endSegment{std::abs(s.index - actSegment->index) + strokeWidth,
					std::abs(obstacle->index - s.index) - strokeWidth, obstacle->end1, obstacle->end2, actSegment});
			break;
		case obstacleSegment::obstacleType::net:
			if (currentNet != obstacle->net) {
				E.insert(new endSegment{std::abs(s.index - actSegment->index) + strokeWidth,
						std::abs(obstacle->index - s.index) - strokeWidth, obstacle->end1, obstacle->end2, actSegment});
				solved |= generateEndSegments(actSegment,
						segment{actSegment->isUpRight() ? obstacle->index - strokeWidth : obstacle->index + strokeWidth,
								obstacle->end2, s.end2},
						crossovers + 1, obstacleSet);
			} else {
				solved = true;
				updateSolution();
			}
			break;
		case obstacleSegment::obstacleType::startA:
			// TODO:complete case
			break;
		case obstacleSegment::obstacleType::startB:
			// TODO:complete case
			break;
	}
}
bool routing::straightLine(splicedTerminal* t0, splicedTerminal* t1) {
	bool horizontal;
	splicedTerminal* lowerT;
	splicedTerminal* higherT;

	auto tSide = [](splicedTerminal* t) {
		switch (t->baseTerminal->side) {
			case terminalSide::leftSide:
				return t->isSystemTerminal() ? terminalSide::rightSide : terminalSide::leftSide;
			case terminalSide::topSide:
				return t->isSystemTerminal() ? terminalSide::bottomSide : terminalSide::topSide;
			case terminalSide::rightSide:
				return t->isSystemTerminal() ? terminalSide::leftSide : terminalSide::rightSide;
			case terminalSide::bottomSide:
				return t->isSystemTerminal() ? terminalSide::topSide : terminalSide::bottomSide;
			case terminalSide::noneSide:
				throw std::runtime_error("Uninitialized Terminals!!");
		}
	};

	if (tSide(t0) == terminalSide::leftSide && tSide(t1) == terminalSide::rightSide &&
			t0->placedPosition.x < t1->placedPosition.x) {
		horizontal = true;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == terminalSide::leftSide && tSide(t0) == terminalSide::rightSide &&
			   t1->placedPosition.x < t0->placedPosition.x) {
		horizontal = true;
		lowerT = t1;
		higherT = t0;
	} else if (tSide(t0) == terminalSide::bottomSide && tSide(t1) == terminalSide::topSide &&
			   t0->placedPosition.y < t1->placedPosition.y) {
		horizontal = false;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == terminalSide::bottomSide && tSide(t0) == terminalSide::topSide &&
			   t1->placedPosition.y < t0->placedPosition.y) {
		horizontal = false;
		lowerT = t1;
		higherT = t0;
	} else
		return false;

	if (horizontal) {
		auto startIter = vObstacleSet.lower_bound(
				new obstacleSegment{lowerT->placedPosition.x, 0, 0, obstacleSegment::obstacleType::module, nullptr});
		auto endIter = vObstacleSet.upper_bound(
				new obstacleSegment{higherT->placedPosition.x, 0, 0, obstacleSegment::obstacleType::module, nullptr});
		while (startIter != endIter) {
			if (((*startIter)->end1 < lowerT->placedPosition.y && lowerT->placedPosition.y < (*startIter)->end1) &&
					((*startIter)->type == obstacleSegment::obstacleType::module ||
							((*startIter)->type == obstacleSegment::obstacleType::net) &&
									((*startIter)->end1 == lowerT->placedPosition.y ||
											(*startIter)->end2 == lowerT->placedPosition.y)))
				return false;
			++startIter;
		}
		hObstacleSet.insert(new obstacleSegment(lowerT->placedPosition.y, lowerT->placedPosition.x,
				higherT->placedPosition.x, obstacleSegment::obstacleType::net, currentNet));
		return true;
	} else {
		auto startIter = hObstacleSet.lower_bound(
				new obstacleSegment{lowerT->placedPosition.y, 0, 0, obstacleSegment::obstacleType::module, nullptr});
		auto endIter = hObstacleSet.upper_bound(
				new obstacleSegment{higherT->placedPosition.y, 0, 0, obstacleSegment::obstacleType::module, nullptr});
		while (startIter != endIter) {
			if (((*startIter)->end1 < lowerT->placedPosition.y && lowerT->placedPosition.y < (*startIter)->end1) &&
					((*startIter)->type == obstacleSegment::obstacleType::module ||
							((*startIter)->type == obstacleSegment::obstacleType::net) &&
									((*startIter)->end1 == lowerT->placedPosition.y ||
											(*startIter)->end2 == lowerT->placedPosition.y)))
				return false;
			++startIter;
		}
		vObstacleSet.insert(new obstacleSegment(lowerT->placedPosition.x, lowerT->placedPosition.y,
				higherT->placedPosition.y, obstacleSegment::obstacleType::net, currentNet));
		return true;
	}
}

void routing::newActives(activeSegment* actS, std::unordered_set<activeSegment*>& nextActSSet) {
	// Adds atmost two actives for each endSegment
	auto addActiveFunction = [&nextActSSet](endSegment* ePrevHighest, endSegment* e, endSegment* eNextHighest) {
		if (e->index + e->distance > ePrevHighest->index + ePrevHighest->distance) {
			if (e->isUpRight())
				nextActSSet.insert(
						new activeSegment{actS->bends, e->crossovers, e->end1, e->baseSegment->index + e->index,
								e->baseSegment->index + e->index + e->distance, e->baseSegment->dir--, e->baseSegment});
			else
				nextActSSet.insert(new activeSegment{actS->bends, e->crossovers, e->end1,
						e->baseSegment->index - e->index - e->distance, e->baseSegment->index - e->index,
						e->baseSegment->dir--, e->baseSegment});
		}
		if (e->index + e->distance > eNextHighest->index + eNextHighest->distance) {
			if (e->isUpRight())
				nextActSSet.insert(
						new activeSegment{actS->bends, e->crossovers, e->end2, e->baseSegment->index + e->index +,
								e->baseSegment->index - e->index - e->distance, e->baseSegment->dir++, e->baseSegment});
			else
				nextActSSet.insert(new activeSegment{actS->bends, e->crossovers, e->end2,
						e->baseSegment->index - e->index - e->distance, e->baseSegment->index - e->index,
						e->baseSegment->dir++, e->baseSegment});
		}
	};

	// Ends are already populated by this time.
	// This will be the top element (other wise we have an errror)
	auto iter = E.insert(new endSegment{0, actS->end1 - strokeWidth, actS->end1 - strokeWidth, actS->crossedNets, actS})
						.first;
	auto end = E.insert(new endSegment{0, actS->end2 + strokeWidth, actS->end2 + strokeWidth, actS->crossedNets, actS})
					   .first;

	endSegment* ePrev = *iter;
	endSegment* e = *++iter;

	auto iterNext = iter;
	endSegment* eNext = *++iterNext;

	while (iter != end) {
		for (; iterNext != E.end() && (*iterNext)->end1 == eNext->end1; ++iterNext)
			eNext = *iterNext;

		while (e->end1 != eNext->end1) {
			addActiveFunction(ePrev, e, eNext);
			e = *++iter;
		}
		eNext = *++iterNext;
	}
}

void routing::reconstructPath(activeSegment* actS, int x) {
	while (actS->bends != -1) {
		int min = x;
		int max = actS->prevSegment->index;
		if (min > max)
			std::swap(min, max);
		switch (actS->prevSegment->dir) {
			case up:
			case down:
				vObstacleSet.insert(
						new obstacleSegment{actS->index, min, max, obstacleSegment::obstacleType::net, currentNet});
				break;
			case left:
			case right:
				hObstacleSet.insert(
						new obstacleSegment{actS->index, min, max, obstacleSegment::obstacleType::net, currentNet});
				break;
		}
		actS = actS->prevSegment;
		x = actS->prevSegment->index;
	}
}

int routing::pathLength(activeSegment* actS, int x) {
	int length = 0;
	while (actS->prevSegment) {
		length += std::abs(x - actS->index);
		actS = actS->prevSegment;
		x = actS->index;
	}
}
