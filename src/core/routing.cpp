#include "routing.h"
#include <unordered_set>
#include "coreDesign.h"
#include "terminal.h"
const int strokeWidth = 1;

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
			if ((*iter)->type == obstacleSegment::startA || (*iter)->type == obstacleSegment::startB)
				iter = hObstacleSet.erase(iter);
			else
				++iter;
		}

		for (auto&& iter = vObstacleSet.begin(); iter != vObstacleSet.end();) {
			if ((*iter)->type == obstacleSegment::startA || (*iter)->type == obstacleSegment::startB)
				iter = vObstacleSet.erase(iter);
			else
				++iter;
		}
	}
}

void routing::addObstacleBounding() {
	hObstacleSet.insert(new obstacleSegment{INT16_MIN, INT16_MIN, INT16_MAX, obstacleSegment::module, nullptr});
	hObstacleSet.insert(new obstacleSegment{INT16_MAX, INT16_MIN, INT16_MAX, obstacleSegment::module, nullptr});
	vObstacleSet.insert(new obstacleSegment{INT16_MIN, INT16_MIN, INT16_MAX, obstacleSegment::module, nullptr});
	vObstacleSet.insert(new obstacleSegment{INT16_MAX, INT16_MIN, INT16_MAX, obstacleSegment::module, nullptr});

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

bool routing::expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
		std::unordered_set<activeSegment*>& incrementedActSegmentSet) {
	int j = -1;
	bool solved = false;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->bends);

	for (activeSegment* s : actSegmentSet) {
		if (s->bends == j) {
			if (!generateEndSegments(s, *s, s->crossedNets, s->isVertical() ? hObstacleSet : vObstacleSet))
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
	activeSegment* s = soln.a;
	intPair currentPoint = soln.optimalPoint;
	while (s->prevSegment) {
		if (s->isVertical()) {
			createLine(s->index, currentPoint.y, currentPoint.x, currentPoint.y);
			currentPoint = intPair{s->index, currentPoint.y};
		} else {
			createLine(currentPoint.x, s->index, currentPoint.x, currentPoint.y);
			currentPoint = intPair{currentPoint.x, s->index};
		}
		s = s->prevSegment;
	}
	if (soln.b) {
		s = soln.a;
		currentPoint = soln.optimalPoint;
		while (s->prevSegment) {
			if (s->isVertical()) {
				createLine(s->index, currentPoint.y, currentPoint.x, currentPoint.y);
				currentPoint = intPair{s->index, currentPoint.y};
			} else {
				createLine(currentPoint.x, s->index, currentPoint.x, currentPoint.y);
				currentPoint = intPair{currentPoint.x, s->index};
			}
			s = s->prevSegment;
		}
	}
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
									obstacle->end1, actSegment->dir, actSegment->prevSegment});
						if (obstacle->end2 < a->end2)
							activesA.insert(new activeSegment{a->bends, a->crossedNets, a->index, obstacle->end2,
									a->end2, actSegment->dir, actSegment->prevSegment});
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
									obstacle->end1, actSegment->dir, actSegment->prevSegment});
						if (obstacle->end2 < b->end2)
							activesB.insert(new activeSegment{b->bends, b->crossedNets, b->index, obstacle->end2,
									b->end2, actSegment->dir, actSegment->prevSegment});
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
		std::unordered_set<activeSegment*>& nextActSSet) {
	obstacleSegment::obstacleType oType =
			(activesA.find(actS) != activesA.end()) ? obstacleSegment::startA : obstacleSegment::startB;

	orderedObstacleSet& oSet = actS->isVertical() ? vObstacleSet : hObstacleSet;

	if (e->index + e->length > ePrevHighest->index + ePrevHighest->length) {
		activeSegment* cClockwiseSegment =
				new activeSegment{actS->bends + 1, e->crossovers, 0, 0, 0, left, e->baseSegment};
		switch (actS->dir) {
			case left:
				cClockwiseSegment->index = e->end1;
				cClockwiseSegment->end1 = actS->index - e->index - e->length;
				cClockwiseSegment->end2 = actS->index - ePrevHighest->index - ePrevHighest->length - strokeWidth;
				cClockwiseSegment->dir = down;
				break;
			case up:
				cClockwiseSegment->index = e->end1;
				cClockwiseSegment->end1 = actS->index + ePrevHighest->index + ePrevHighest->length + strokeWidth;
				cClockwiseSegment->end2 = actS->index + e->index + e->length;
				cClockwiseSegment->dir = left;
				break;
			case right:
				cClockwiseSegment->index = e->end2;
				cClockwiseSegment->end1 = actS->index + ePrevHighest->index + ePrevHighest->length + strokeWidth;
				cClockwiseSegment->end2 = actS->index + e->index + e->length;
				cClockwiseSegment->dir = up;
				break;
			case down:
				cClockwiseSegment->index = e->end2;
				cClockwiseSegment->end1 = actS->index - e->index - e->length;
				cClockwiseSegment->end2 = actS->index - ePrevHighest->index - ePrevHighest->length - strokeWidth;
				cClockwiseSegment->dir = right;
				break;
		}
		nextActSSet.insert(cClockwiseSegment);
		oSet.insert(new obstacleSegment{
				cClockwiseSegment->index, cClockwiseSegment->end1, cClockwiseSegment->end2, oType, cClockwiseSegment});
	}
	if (e->index + e->length > eNextHighest->index + eNextHighest->length) {
		activeSegment* clockwiseSegment =
				new activeSegment{actS->bends + 1, e->crossovers, 0, 0, 0, right, e->baseSegment};
		switch (actS->dir) {
			case left:
				clockwiseSegment->index = e->end2;
				clockwiseSegment->end1 = actS->index - e->index - e->length;
				clockwiseSegment->end2 = actS->index - eNextHighest->index - eNextHighest->length - strokeWidth;
				clockwiseSegment->dir = up;
				break;
			case up:
				clockwiseSegment->index = e->end2;
				clockwiseSegment->end1 = actS->index + eNextHighest->index + eNextHighest->length + strokeWidth;
				clockwiseSegment->end2 = actS->index + e->index + e->length;
				clockwiseSegment->dir = right;
				break;
			case right:
				clockwiseSegment->index = e->end1;
				clockwiseSegment->end1 = actS->index + eNextHighest->index + eNextHighest->length + strokeWidth;
				clockwiseSegment->end2 = actS->index + e->index + e->length;
				clockwiseSegment->dir = down;
				break;
			case down:
				clockwiseSegment->index = e->end1;
				clockwiseSegment->end1 = actS->index - e->index - e->length;
				clockwiseSegment->end2 = actS->index - eNextHighest->index - eNextHighest->length - strokeWidth;
				clockwiseSegment->dir = left;
				break;
		}
		nextActSSet.insert(clockwiseSegment);
		oSet.insert(new obstacleSegment{
				clockwiseSegment->index, clockwiseSegment->end1, clockwiseSegment->end2, oType, clockwiseSegment});
	}
};

void routing::newActives(activeSegment* actS, std::unordered_set<activeSegment*>& nextActSSet) {
	// Ends are already populated by this time.
	// This will be the top element (other wise we have an errror)
	auto iter = E.insert(new endSegment{0, actS->end1 - strokeWidth, actS->end1 - strokeWidth, actS->crossedNets, actS})
						.first;
	E.insert(new endSegment{0, actS->end2 + strokeWidth, actS->end2 + strokeWidth, actS->crossedNets, actS}).first;

	endSegment* ePrev = *iter;
	endSegment* e = *++iter;

	auto iterNext = iter;
	endSegment* eNext = *++iterNext;

	while (iterNext != E.end()) {
		for (; iterNext != E.end() && (*iterNext)->end1 == eNext->end1; ++iterNext)
			eNext = *iterNext;

		while (e->end1 != eNext->end1) {
			addActiveFunction(ePrev, e, eNext, actS, nextActSSet);
			e = *++iter;
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
		closestIndex = std::min(s.end2 - strokeWidth - actSegment->prevSegment->index,
				s.end1 + strokeWidth - actSegment->prevSegment->index);

	intPair joinPoint = actSegment->isVertical() ? intPair{s.index, closestIndex} : intPair{closestIndex, s.index};

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
	// NOTE: THis is cleanup but there should not be any actives left
	for (auto&& a : activesA) {
		delete a;
	}
	for (auto&& b : activesB) {
		delete b;
	}
}
