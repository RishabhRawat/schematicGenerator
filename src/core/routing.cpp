#include "routing.h"
#include <unordered_set>
#include "coreDesign.h"
#include "terminal.h"
const int strokeWidth = 5;

#ifdef WEB_COMPILATION
#include <emscripten/val.h>
void line(int x0, int y0, int x1, int y1) {
	emscripten::val cppfuncs = emscripten::val::global("cppfuncs");
	cppfuncs.call<void>("createWire", x0, y0, x1, y1);
}
#endif  // WEB_COMPILATION

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

		if (internalCoalescedNet->connectedModuleSplicedTerminalMap.size() < 2) {
			continue;
			// TODO: A better way to handle??
			throw std::runtime_error("Dangling net??");
		}

		splicedTerminal* t0 = tSet.pop();
		splicedTerminal* t1 = tSet.pop();
		initNet(t0, t1);
		while (!tSet.empty()) {
			//			expandNet(tSet.pop());
		}

		// Clearing up Actives
		for (auto&& a : activesA) {
			delete a;
		}
		activesA.clear();
		for (auto&& b : activesB) {
			delete b;
		}
		activesB.clear();

		// Clearing up the obstacles!!
		for (auto&& iter = hObstacleSet.begin(); iter != hObstacleSet.end();) {
			if ((*iter)->type == obstacleSegment::startA || (*iter)->type == obstacleSegment::startB) {
				obstacleSegment* element = *iter;
				iter = hObstacleSet.erase(iter);
				delete element;
			} else
				++iter;
		}

		for (auto&& iter = vObstacleSet.begin(); iter != vObstacleSet.end();) {
			if ((*iter)->type == obstacleSegment::startA || (*iter)->type == obstacleSegment::startB) {
				obstacleSegment* element = *iter;
				iter = vObstacleSet.erase(iter);
				delete element;
			} else
				++iter;
		}
	}
}

void routing::addObstacleBounding() {
	hObstacleSet.insert(new obstacleSegment{
			-strokeWidth, -strokeWidth, core->size.x + strokeWidth, obstacleSegment::module, nullptr});
	hObstacleSet.insert(new obstacleSegment{
			core->size.y + strokeWidth, -strokeWidth, core->size.x + strokeWidth, obstacleSegment::module, nullptr});
	vObstacleSet.insert(new obstacleSegment{
			-strokeWidth, -strokeWidth, core->size.y + strokeWidth, obstacleSegment::module, nullptr});
	vObstacleSet.insert(new obstacleSegment{
			core->size.x + strokeWidth, -strokeWidth, core->size.y + strokeWidth, obstacleSegment::module, nullptr});

	for (auto&& m_pair : core->subModules) {
		intPair mPos = m_pair.second->position;
		intPair mSize = m_pair.second->size;
		hObstacleSet.insert(new obstacleSegment{mPos.y, mPos.x, mPos.x + mSize.x, obstacleSegment::module, nullptr});
		hObstacleSet.insert(
				new obstacleSegment{mPos.y + mSize.y, mPos.x, mPos.x + mSize.x, obstacleSegment::module, nullptr});
		vObstacleSet.insert(new obstacleSegment{mPos.x, mPos.y, mPos.y + mSize.y, obstacleSegment::module, nullptr});
		vObstacleSet.insert(
				new obstacleSegment{mPos.x + mSize.x, mPos.y, mPos.y + mSize.y, obstacleSegment::module, nullptr});
	}

	for (splicedTerminal* sysT : core->systemModule.moduleSplicedTerminals) {
		hObstacleSet.insert(new obstacleSegment{sysT->placedPosition.y, sysT->placedPosition.x, sysT->placedPosition.x,
				obstacleSegment::module, nullptr});
		vObstacleSet.insert(new obstacleSegment{sysT->placedPosition.x, sysT->placedPosition.y, sysT->placedPosition.y,
				obstacleSegment::module, nullptr});
	}
}

void routing::initNet(splicedTerminal* t0, splicedTerminal* t1) {
	initActives(activesA, t0);
	initActives(activesB, t1);
	bool solutionFound = straightLine(t0, t1);
	while (!solutionFound) {
		std::unordered_set<activeSegment*> A_new;
		solutionFound |= expandActives(activesA, A_new);
		for (activeSegment* a : A_new)
			activesA.insert(a);

		std::unordered_set<activeSegment*> B_new;
		solutionFound |= expandActives(activesB, B_new);
		for (activeSegment* a : B_new)
			activesB.insert(a);
	}
}

void routing::initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t) {
	switch (t->baseTerminal->side) {
		case terminalSide::leftSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y,
					t->isSystemTerminal() ? right : left, nullptr});
			break;
		case terminalSide::rightSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y,
					t->isSystemTerminal() ? left : right, nullptr});
			break;
		case terminalSide::topSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x,
					t->isSystemTerminal() ? down : up, nullptr});
			break;
		case terminalSide::bottomSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x,
					t->isSystemTerminal() ? up : down, nullptr});
			break;
		case terminalSide::noneSide:
			throw std::runtime_error("Cannot have a terminal without a side");
	}
}

bool routing::expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
		std::unordered_set<activeSegment*>& incrementedActSegmentSet) {
	int j = -1;
	bool solved = false;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->bends);

	for (activeSegment* s : actSegmentSet) {
		if (s->bends == j) {
			if (!generateEndSegments(s, *s, s->crossedNets, s->scansVertical() ? hObstacleSet : vObstacleSet))
				newActives(s, incrementedActSegmentSet);
			else
				solved = true;
			for (auto&& ends : E) {
				delete ends;
			}
			E.clear();
		}
	}

	if (!solved && incrementedActSegmentSet.empty())
		throw std::runtime_error("No solution found!!");

	if (solved) {
		reconstructSolution();
		return true;
	} else {
		return false;
	}
}

void createLine(int x0, int y0, int x1, int y1) {
#ifdef WEB_COMPILATION
	line(x0, y0, x1, y1);
#endif  // WEB_COMPILATION
	std::cout << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
}

void routing::reconstructSolution() {
	std::cout << "Current Solution" << std::endl;
	activeSegment* s = soln.a;
	intPair currentPoint = soln.optimalPoint;
	while (s) {
		if (s->scansVertical()) {
			createLine(currentPoint.x, s->index, currentPoint.x, currentPoint.y);
			currentPoint = intPair{currentPoint.x, s->index};
		} else {
			createLine(s->index, currentPoint.y, currentPoint.x, currentPoint.y);
			currentPoint = intPair{s->index, currentPoint.y};
		}
		s = s->prevSegment;
	}
	s = soln.b;
	currentPoint = soln.optimalPoint;
	while (s) {
		if (s->scansVertical()) {
			createLine(currentPoint.x, s->index, currentPoint.x, currentPoint.y);
			currentPoint = intPair{currentPoint.x, s->index};
		} else {
			createLine(s->index, currentPoint.y, currentPoint.x, currentPoint.y);
			currentPoint = intPair{s->index, currentPoint.y};
		}
		s = s->prevSegment;
	}
	soln.clear();
}

bool routing::generateEndSegments(
		activeSegment* actSegment, segment s, int crossovers, orderedObstacleSet& obstacleSet) {
	bool solved = false;
	orderedObstacleSet obstacles;
	for (auto&& o : obstacleSet) {
		if (!(o->end1 > s.end2 || o->end2 < s.end1))
			obstacles.insert(o);
	}
	obstacleSegment obstacleScanner{s.index};
	obstacleSegment* obstacle =
			actSegment->isUpRight() ? *obstacles.upper_bound(&obstacleScanner)
									: *orderedObstacleSet::reverse_iterator(obstacles.lower_bound(&obstacleScanner));

	segment cutSegment = s;

	if (s.end1 < obstacle->end1) {
		solved |= generateEndSegments(
				actSegment, segment{s.index, s.end1, obstacle->end1 - strokeWidth}, crossovers, obstacleSet);
		cutSegment.end1 = obstacle->end1;
	}
	if (obstacle->end2 < s.end2) {
		solved |= generateEndSegments(
				actSegment, segment{s.index, obstacle->end2 + strokeWidth, s.end2}, crossovers, obstacleSet);
		cutSegment.end2 = obstacle->end2;
	}

	endSegment* newEndSegment = new endSegment{std::abs(cutSegment.index - actSegment->index) + strokeWidth,
			std::abs(obstacle->index - cutSegment.index) - 2 * strokeWidth, cutSegment.end1, cutSegment.end2,
			actSegment->crossedNets, actSegment};

	switch (obstacle->type) {
		case obstacleSegment::module:
			E.insert(newEndSegment);
			break;
		case obstacleSegment::net:
			if (static_cast<coalescedNet*>(obstacle->sourcePtr) != currentNet) {
				E.insert(newEndSegment);
				solved |= generateEndSegments(actSegment,
						segment{actSegment->isUpRight() ? obstacle->index + strokeWidth : obstacle->index - strokeWidth,
								cutSegment.end1, cutSegment.end2},
						crossovers + 1, obstacleSet);
			} else {
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::startA:
			E.insert(newEndSegment);

			if (activesA.find(actSegment) != activesA.end()) {
				for (auto&& a : activesA) {
					if (a->index == obstacle->index && a->end1 <= obstacle->end1 && obstacle->end2 <= a->end2) {
						activesA.erase(a);
						if (a->end1 < obstacle->end1)
							activesA.insert(new activeSegment{a->bends, a->crossedNets, a->index, a->end1,
									obstacle->end1, actSegment->scanDirection, actSegment->prevSegment});
						if (obstacle->end2 < a->end2)
							activesA.insert(new activeSegment{a->bends, a->crossedNets, a->index, obstacle->end2,
									a->end2, actSegment->scanDirection, actSegment->prevSegment});
						break;
					}
				}
			} else {
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::startB:
			E.insert(newEndSegment);

			if (activesB.find(actSegment) != activesB.end()) {
				for (auto&& b : activesB) {
					if (b->index == obstacle->index && b->end1 <= obstacle->end1 && obstacle->end2 <= b->end2) {
						if (b->end1 < obstacle->end1)
							activesB.insert(new activeSegment{b->bends, b->crossedNets, b->index, b->end1,
									obstacle->end1, actSegment->scanDirection, actSegment->prevSegment});
						if (obstacle->end2 < b->end2)
							activesB.insert(new activeSegment{b->bends, b->crossedNets, b->index, obstacle->end2,
									b->end2, actSegment->scanDirection, actSegment->prevSegment});
						activesB.erase(b);
						break;
					}
				}
			} else {
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::compare:
			throw std::runtime_error("A compare type is to be never inserted!!");
	}
	return solved;
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
				break;
		}
		throw std::runtime_error("Uninitialized Terminals!!");
	};

	if (tSide(t0) == terminalSide::rightSide && tSide(t1) == terminalSide::leftSide &&
			t0->placedPosition.x < t1->placedPosition.x) {
		horizontal = true;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == terminalSide::rightSide && tSide(t0) == terminalSide::leftSide &&
			   t1->placedPosition.x < t0->placedPosition.x) {
		horizontal = true;
		lowerT = t1;
		higherT = t0;
	} else if (tSide(t0) == terminalSide::topSide && tSide(t1) == terminalSide::bottomSide &&
			   t0->placedPosition.y < t1->placedPosition.y) {
		horizontal = false;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == terminalSide::topSide && tSide(t0) == terminalSide::bottomSide &&
			   t1->placedPosition.y < t0->placedPosition.y) {
		horizontal = false;
		lowerT = t1;
		higherT = t0;
	} else
		return false;

	orderedObstacleSet& perpendicularObstacles = horizontal ? vObstacleSet : hObstacleSet;
	orderedObstacleSet& parallelObstacles = horizontal ? hObstacleSet : vObstacleSet;

	obstacleSegment lowerScanEnd{horizontal ? lowerT->placedPosition.x : lowerT->placedPosition.y};
	obstacleSegment higherScanEnd{horizontal ? higherT->placedPosition.x : higherT->placedPosition.y};

	auto stIt = perpendicularObstacles.upper_bound(&lowerScanEnd);
	auto endIt = perpendicularObstacles.lower_bound(&higherScanEnd);
	while (stIt != endIt) {
		if (((*stIt)->end1 < lowerT->placedPosition.y && lowerT->placedPosition.y < (*stIt)->end2) &&
				((*stIt)->type == obstacleSegment::module ||
						(((*stIt)->type == obstacleSegment::net) &&
								((*stIt)->end1 == lowerT->placedPosition.y ||
										(*stIt)->end2 == lowerT->placedPosition.y))))
			return false;
		++stIt;
	}
	if (horizontal) {
		parallelObstacles.insert(new obstacleSegment{lowerT->placedPosition.y, lowerT->placedPosition.x,
				higherT->placedPosition.x, obstacleSegment::net, currentNet});
	} else {
		parallelObstacles.insert(new obstacleSegment{lowerT->placedPosition.x, lowerT->placedPosition.y,
				higherT->placedPosition.y, obstacleSegment::net, currentNet});
	}
	createLine(t0->placedPosition.x, t0->placedPosition.y, t1->placedPosition.x, t1->placedPosition.y);
	return true;
}

// Adds atmost two actives for each endSegment
void routing::addActiveFunction(endSegment* ePrevHighest, endSegment* e, endSegment* eNextHighest, activeSegment* actS,
		std::unordered_set<activeSegment*>& newActiveSegments) {
	obstacleSegment::obstacleType oType =
			(activesA.find(actS) != activesA.end()) ? obstacleSegment::startA : obstacleSegment::startB;

	orderedObstacleSet& oSet = actS->scansVertical() ? vObstacleSet : hObstacleSet;

	if (e->index + e->length > ePrevHighest->index + ePrevHighest->length) {
		activeSegment* leftSegment =
				new activeSegment{actS->bends + 1, e->crossovers, e->end1, 0, 0, left, e->baseSegment};
		leftSegment->scanDirection = actS->scansVertical() ? left : down;
		if (actS->isUpRight()) {
			leftSegment->end1 = ePrevHighest->farIndex() + strokeWidth;
			leftSegment->end2 = e->farIndex();
		} else {
			leftSegment->end1 = e->farIndex();
			leftSegment->end2 = ePrevHighest->farIndex() - strokeWidth;
		}
		newActiveSegments.insert(leftSegment);
		oSet.insert(new obstacleSegment{leftSegment->index, leftSegment->end1, leftSegment->end2, oType, leftSegment});
	}
	if (e->index + e->length > eNextHighest->index + eNextHighest->length) {
		activeSegment* rightSegment =
				new activeSegment{actS->bends + 1, e->crossovers, e->end2, 0, 0, right, e->baseSegment};
		rightSegment->scanDirection = actS->scansVertical() ? right : up;
		if (actS->isUpRight()) {
			rightSegment->end1 = eNextHighest->farIndex() + strokeWidth;
			rightSegment->end2 = e->farIndex();
		} else {
			rightSegment->end1 = e->farIndex();
			rightSegment->end2 = eNextHighest->farIndex() - strokeWidth;
		}
		newActiveSegments.insert(rightSegment);
		oSet.insert(
				new obstacleSegment{rightSegment->index, rightSegment->end1, rightSegment->end2, oType, rightSegment});
	}
};

void routing::newActives(activeSegment* actS, std::unordered_set<activeSegment*>& newActiveSegments) {
	// Ends are already populated by this time.
	// This will be the top element (other wise we have an errror)
	auto iter = E.insert(new endSegment{actS->end1 - strokeWidth, actS->end1 - strokeWidth, actS}).first;
	E.insert(new endSegment{actS->end2 + strokeWidth, actS->end2 + strokeWidth, actS});

	endSegment* ePrev = *iter;
	endSegment* e = *++iter;

	auto iterNext = iter;
	endSegment* eNext = *++iterNext;

	while (iterNext != E.end()) {
		for (; iterNext != E.end() && (*iterNext)->end1 == eNext->end1; ++iterNext)
			eNext = *iterNext;

		while (e->end1 != eNext->end1) {
			addActiveFunction(ePrev, e, eNext, actS, newActiveSegments);
			++iter;
			if (e->end1 != (*iter)->end1)
				ePrev = e;
			e = *iter;
		}
		eNext = *iterNext;
	}
}

unsigned int routing::pathLength(activeSegment* actS, int x) {
	unsigned int length = 0;
	while (actS->prevSegment) {
		// measures length between actS and actS->prev
		length += std::abs(x - actS->prevSegment->index);
		x = actS->index;
		actS = actS->prevSegment;
	}
	return length;
}

int calculateCost(int bends, int crossovers, int length) {
	return 20 * bends + 10 * crossovers + length;
}

void routing::updateSolution(segment s, obstacleSegment* obstacle, activeSegment* actSegment) {
	int closestIndex;
	activeSegment* otherActSegment = nullptr;
	unsigned int length;
	int totalBends = actSegment->bends, totalCrossovers = actSegment->crossedNets;
	if (s.end2 - s.end1 < 2 * strokeWidth)  // No space for a new net
		return;
	else
		closestIndex = std::abs(s.end2 - strokeWidth - actSegment->prevSegment->index) >
									   std::abs(s.end1 + strokeWidth - actSegment->prevSegment->index)
							   ? s.end1 + strokeWidth
							   : s.end2 - strokeWidth;

	intPair joinPoint = actSegment->scansVertical() ? intPair{closestIndex, s.index} : intPair{s.index, closestIndex};

	if (obstacle->type == obstacleSegment::net) {
		length = pathLength(actSegment, closestIndex) + std::abs(s.index - actSegment->index);
	} else {
		otherActSegment = static_cast<activeSegment*>(obstacle->sourcePtr);
		length = pathLength(actSegment, closestIndex) + pathLength(otherActSegment, closestIndex) +
				 std::abs(actSegment->index - obstacle->index);
		totalBends += otherActSegment->bends;
		// NOTE: DO WE NOT NEED TO CHECK THE SEGMENT INSTEAD??
		totalCrossovers += otherActSegment->crossedNets;
	}
	if (soln.cost > calculateCost(totalBends, totalCrossovers, length)) {
		soln.a = actSegment;
		soln.b = otherActSegment;
		soln.cost = calculateCost(totalBends, totalCrossovers, length);
		soln.optimalPoint = joinPoint;
	}
}

routing::~routing() {
	for (auto&& o : vObstacleSet) {
		delete o;
	}
	for (auto&& o : hObstacleSet) {
		delete o;
	}
	if (!activesA.empty())
		throw std::runtime_error("Incomplete memory deallocation in activesA");
	if (!activesB.empty())
		throw std::runtime_error("Incomplete memory deallocation in activesB");
}
