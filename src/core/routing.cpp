#include "routing.h"
#include <unordered_set>
#include "coreDesign.h"
#include "terminalImpl.h"
const int strokeWidth = 5;

#ifdef WEB_COMPILATION
#include <emscripten/val.h>
void createBlackLine(int x0, int y0, int x1, int y1) {
	emscripten::val cppfuncs = emscripten::val::global("cppfuncs");
	cppfuncs.call<void>("createWire", x0, y0, x1, y1);
}
void activeALine(int x0, int y0, int x1, int y1) {
	emscripten::val cppfuncs = emscripten::val::global("cppfuncs");
	cppfuncs.call<void>("createRedActive", x0, y0, x1, y1, 3);
}
void activeBLine(int x0, int y0, int x1, int y1) {
	emscripten::val cppfuncs = emscripten::val::global("cppfuncs");
	cppfuncs.call<void>("createBlueActive", x0, y0, x1, y1, 3);
}
void clearActives() {
	emscripten::val cppfuncs = emscripten::val::global("cppfuncs");
	cppfuncs.call<void>("removeActives");
}
#endif  // WEB_COMPILATION

void routing::route() {
	addObstacleBounding();
	for (net* internalCoalescedNet : core->internalNets) {
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

#ifdef WEB_COMPILATION
		clearActives();
#endif  // WEB_COMPILATION
		while (!tSet.empty()) {
			expandNet(tSet.pop());
		}
	}
}

void routing::addObstacleBounding() {
	addObstacle(-strokeWidth, -strokeWidth, core->size.x - core->offset.x + strokeWidth, obstacleSegment::module,
			nullptr, true);
	addObstacle(core->size.y - core->offset.y + strokeWidth, -strokeWidth, core->size.x - core->offset.x + strokeWidth,
			obstacleSegment::module, nullptr, true);
	addObstacle(-strokeWidth, -strokeWidth, core->size.y - core->offset.y + strokeWidth, obstacleSegment::module,
			nullptr, false);
	addObstacle(core->size.x - core->offset.x + strokeWidth, -strokeWidth, core->size.y - core->offset.y + strokeWidth,
			obstacleSegment::module, nullptr, false);

	for (auto&& m_pair : core->subModules) {
		intPair mPos = m_pair.second->position;
		intPair mSize = m_pair.second->size;
		addObstacle(mPos.y, mPos.x, mPos.x + mSize.x, obstacleSegment::module, nullptr, true);
		addObstacle(mPos.y + mSize.y, mPos.x, mPos.x + mSize.x, obstacleSegment::module, nullptr, true);
		addObstacle(mPos.x, mPos.y, mPos.y + mSize.y, obstacleSegment::module, nullptr, false);
		addObstacle(mPos.x + mSize.x, mPos.y, mPos.y + mSize.y, obstacleSegment::module, nullptr, false);
	}

	for (splicedTerminal* sysT : core->systemModule.moduleSplicedTerminals) {
		addObstacle(sysT->placedPosition.y, sysT->placedPosition.x, sysT->placedPosition.x, obstacleSegment::module,
				nullptr, true);
		addObstacle(sysT->placedPosition.x, sysT->placedPosition.y, sysT->placedPosition.y, obstacleSegment::module,
				nullptr, false);
	}
	sortObstacles();
}

void routing::initNet(splicedTerminal* t0, splicedTerminal* t1) {
	initActives(activesA, t0);
	initActives(activesB, t1);
	bool solutionFound = straightLine(t0, t1);
	while (!solutionFound) {
		std::unordered_set<activeSegment*> A_new;
		solutionFound |= expandActives(activesA, A_new);
		for (activeSegment* a : A_new) {
#ifdef WEB_COMPILATION
			activeALine(a->scansVertical() ? a->end1 : a->index, a->scansVertical() ? a->index : a->end1,
					a->scansVertical() ? a->end2 : a->index, a->scansVertical() ? a->index : a->end2);
#endif  // WEB_COMPILATION
			activesA.insert(a);
		}
		if (solutionFound)
			break;

		std::unordered_set<activeSegment*> B_new;
		solutionFound |= expandActives(activesB, B_new);
		for (activeSegment* b : B_new) {
#ifdef WEB_COMPILATION
			activeBLine(b->scansVertical() ? b->end1 : b->index, b->scansVertical() ? b->index : b->end1,
					b->scansVertical() ? b->end2 : b->index, b->scansVertical() ? b->index : b->end2);
#endif  // WEB_COMPILATION
			activesB.insert(b);
		}
	}

	clearActiveSet(activesA);
	clearActiveSet(activesB);
	clearActiveSet(inactives);
}

void routing::initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t) {
	switch (t->placedSide) {
		case leftSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y,
					t->isSystemTerminal() ? right : left, nullptr});
			break;
		case rightSide:
			activeSet.insert(new activeSegment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y,
					t->isSystemTerminal() ? left : right, nullptr});
			break;
		case topSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x,
					t->isSystemTerminal() ? down : up, nullptr});
			break;
		case bottomSide:
			activeSet.insert(new activeSegment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x,
					t->isSystemTerminal() ? up : down, nullptr});
			break;
		case noneSide:
			throw std::runtime_error("Cannot have a terminalImpl without a side");
	}
}

bool routing::expandActives(
		std::unordered_set<activeSegment*>& actSegmentSet, std::unordered_set<activeSegment*>& newActiveSegments) {
	int j = -1;
	bool solved = false;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->bends);

	for (activeSegment* s : actSegmentSet) {
		if (s->bends == j) {
			if (!generateEndSegments(s, *s, s->crossedNets, s->scansVertical() ? hObstacleSet : vObstacleSet)) {
				newActives(s, newActiveSegments);
				sortObstacles();
			} else
				solved = true;
			for (endSegment* ends : E) {
				delete ends;
			}
			E.clear();
		}
	}

	if (!solved && newActiveSegments.empty())
		throw std::runtime_error("No solution found!!");

	if (solved) {
		clearActiveObstacles();
		reconstructSolution();
		sortObstacles();
	}
	return solved;
}

void createLine(int x0, int y0, int x1, int y1) {
#ifdef WEB_COMPILATION
//	createBlackLine(x0, y0, x1, y1);
#endif  // WEB_COMPILATION
		//	std::cout << x0 << " " << y0 << " " << x1 << " " << y1 << std::endl;
}

void routing::reconstructSolution() {
	line* newLine = new line();
	activeSegment* s = soln.a;
	intPair currentPoint = soln.optimalPoint;
	while (s) {
		if (s->scansVertical()) {
			int smaller = s->index, larger = currentPoint.y;
			if (larger < smaller)
				std::swap(smaller, larger);
			createLine(currentPoint.x, smaller, currentPoint.x, larger);
			net::addLineSegment(newLine, currentPoint, intPair{currentPoint.x, s->index});
			addObstacle(currentPoint.x, smaller, larger, obstacleSegment::net, currentNet, false);
			currentPoint = intPair{currentPoint.x, s->index};
		} else {
			int smaller = s->index, larger = currentPoint.x;
			if (larger < smaller)
				std::swap(smaller, larger);
			createLine(smaller, currentPoint.y, larger, currentPoint.y);
			net::addLineSegment(newLine, currentPoint, intPair{s->index, currentPoint.y});
			addObstacle(currentPoint.y, smaller, larger, obstacleSegment::net, currentNet, true);
			currentPoint = intPair{s->index, currentPoint.y};
		}
		s = s->prevSegment;
	}
	s = soln.b;
	currentPoint = soln.optimalPoint;
	while (s) {
		if (s->scansVertical()) {
			int smaller = s->index, larger = currentPoint.y;
			if (larger < smaller)
				std::swap(smaller, larger);
			createLine(currentPoint.x, smaller, currentPoint.x, larger);
			net::addLineSegment(newLine, currentPoint, intPair{currentPoint.x, s->index});
			addObstacle(currentPoint.x, smaller, larger, obstacleSegment::net, currentNet, false);
			currentPoint = intPair{currentPoint.x, s->index};
		} else {
			int smaller = s->index, larger = currentPoint.x;
			if (larger < smaller)
				std::swap(smaller, larger);
			createLine(smaller, currentPoint.y, larger, currentPoint.y);
			net::addLineSegment(newLine, currentPoint, intPair{s->index, currentPoint.y});
			addObstacle(currentPoint.y, smaller, larger, obstacleSegment::net, currentNet, true);
			currentPoint = intPair{s->index, currentPoint.y};
		}
		s = s->prevSegment;
	}
	currentNet->renderedLine.push_back(newLine);
	soln.clear();
}

void routing::clearActiveObstacles() {
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

routing::obstacleSegment* routing::addObstacle(
		int index, int end1, int end2, obstacleSegment::obstacleType oType, void* sourcePtr, bool horizontal) {
	obstacleSegment* obstacle = new obstacleSegment(index, end1, end2, oType, sourcePtr);
	if (horizontal) {
		hObstacleSet.push_back(obstacle);
		hSortNeeded = true;
	} else {
		vObstacleSet.push_back(obstacle);
		vSortNeeded = true;
	}
	return obstacle;
}

void routing::sortObstacles() {
	if (hSortNeeded) {
		std::sort(hObstacleSet.begin(), hObstacleSet.end(), obstacleSegmentAscendingComparator());
		hSortNeeded = false;
	}
	if (vSortNeeded) {
		std::sort(vObstacleSet.begin(), vObstacleSet.end(), obstacleSegmentAscendingComparator());
		vSortNeeded = false;
	}
}

routing::obstacleSegment* routing::findObstacle(
		segment s, bool direction, orderedObstacleSet& reducedObstacleSet, orderedObstacleSet& obstacleSet) {
	for (auto&& o : obstacleSet) {
		if (!(o->end1 > s.end2 || o->end2 < s.end1))
			reducedObstacleSet.push_back(o);
	}
	obstacleSegment obstacleScanner{s.index};

	return direction
				   ? *std::upper_bound(reducedObstacleSet.begin(), reducedObstacleSet.end(), &obstacleScanner,
							 obstacleSegmentAscendingComparator())
				   : *orderedObstacleSet::reverse_iterator(std::lower_bound(reducedObstacleSet.begin(),
							 reducedObstacleSet.end(), &obstacleScanner, obstacleSegmentAscendingComparator()));
}
bool routing::generateEndSegments(
		activeSegment* actSegment, segment s, int crossovers, orderedObstacleSet& obstacleSet) {
	bool solved = false;

	orderedObstacleSet obstacles;
	obstacleSegment* obstacle = findObstacle(s, actSegment->isUpRight(), obstacles, obstacleSet);

	segment cutSegment = s;

	if (s.end1 < obstacle->end1) {
		solved |= generateEndSegments(
				actSegment, segment{s.index, s.end1, obstacle->end1 - strokeWidth}, crossovers, obstacles);
		cutSegment.end1 = obstacle->end1;
	}
	if (obstacle->end2 < s.end2) {
		solved |= generateEndSegments(
				actSegment, segment{s.index, obstacle->end2 + strokeWidth, s.end2}, crossovers, obstacles);
		cutSegment.end2 = obstacle->end2;
	}

	endSegment* newEndSegment = new endSegment{std::abs(cutSegment.index - actSegment->index) + strokeWidth,
			std::abs(obstacle->index - cutSegment.index) - 2 * strokeWidth, cutSegment.end1, cutSegment.end2,
			actSegment->crossedNets, actSegment};

	switch (obstacle->type) {
		case obstacleSegment::module:
			if (newEndSegment->length > 0)
				E.insert(newEndSegment);
			else
				delete newEndSegment;
			break;
		case obstacleSegment::net:
			if (static_cast<net*>(obstacle->sourcePtr) != currentNet) {
				if (newEndSegment->length > 0)
					E.insert(newEndSegment);
				else
					delete newEndSegment;
				solved |= generateEndSegments(actSegment,
						segment{actSegment->isUpRight() ? obstacle->index + strokeWidth : obstacle->index - strokeWidth,
								cutSegment.end1, cutSegment.end2},
						crossovers + 1, obstacleSet);
			} else {
				delete newEndSegment;
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::startA:
			if (newEndSegment->length > 0)
				E.insert(newEndSegment);
			else
				delete newEndSegment;

			if (activesA.find(actSegment) == activesA.end()) {
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::startB:
			if (newEndSegment->length > 0)
				E.insert(newEndSegment);
			else
				delete newEndSegment;

			if (activesB.find(actSegment) == activesB.end()) {
				solved = true;
				updateSolution(cutSegment, obstacle, actSegment);
			}
			break;
		case obstacleSegment::compare:
			throw std::runtime_error("A compare type is to be never inserted!!");
	}
	sortObstacles();
	return solved;
}

bool routing::straightLine(splicedTerminal* t0, splicedTerminal* t1) {
	bool horizontal;
	splicedTerminal* lowerT;
	splicedTerminal* higherT;

	auto tSide = [](splicedTerminal* t) {
		switch (t->placedSide) {
			case leftSide:
				return t->isSystemTerminal() ? rightSide : leftSide;
			case topSide:
				return t->isSystemTerminal() ? bottomSide : topSide;
			case rightSide:
				return t->isSystemTerminal() ? leftSide : rightSide;
			case bottomSide:
				return t->isSystemTerminal() ? topSide : bottomSide;
			case noneSide:
				break;
		}
		throw std::runtime_error("Uninitialized Terminals!!");
	};

	if (tSide(t0) == rightSide && tSide(t1) == leftSide && t0->placedPosition.x < t1->placedPosition.x &&
			t0->placedPosition.y == t1->placedPosition.y) {
		horizontal = true;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == rightSide && tSide(t0) == leftSide && t1->placedPosition.x < t0->placedPosition.x &&
			   t0->placedPosition.y == t1->placedPosition.y) {
		horizontal = true;
		lowerT = t1;
		higherT = t0;
	} else if (tSide(t0) == topSide && tSide(t1) == bottomSide && t0->placedPosition.y < t1->placedPosition.y &&
			   t0->placedPosition.x == t1->placedPosition.x) {
		horizontal = false;
		lowerT = t0;
		higherT = t1;
	} else if (tSide(t1) == topSide && tSide(t0) == bottomSide && t1->placedPosition.y < t0->placedPosition.y &&
			   t0->placedPosition.x == t1->placedPosition.x) {
		horizontal = false;
		lowerT = t1;
		higherT = t0;
	} else
		return false;

	orderedObstacleSet& perpendicularObstacles = horizontal ? vObstacleSet : hObstacleSet;

	obstacleSegment lowerScanEnd{horizontal ? lowerT->placedPosition.x : lowerT->placedPosition.y};
	obstacleSegment higherScanEnd{horizontal ? higherT->placedPosition.x : higherT->placedPosition.y};

	auto stIt = std::upper_bound(perpendicularObstacles.begin(), perpendicularObstacles.end(), &lowerScanEnd,
			obstacleSegmentAscendingComparator());
	auto endIt = std::lower_bound(perpendicularObstacles.begin(), perpendicularObstacles.end(), &higherScanEnd,
			obstacleSegmentAscendingComparator());

	// Checking for obstacles
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
		addObstacle(lowerT->placedPosition.y, lowerT->placedPosition.x, higherT->placedPosition.x, obstacleSegment::net,
				currentNet, horizontal);
	} else {
		addObstacle(lowerT->placedPosition.x, lowerT->placedPosition.y, higherT->placedPosition.y, obstacleSegment::net,
				currentNet, horizontal);
	}
	createLine(t0->placedPosition.x, t0->placedPosition.y, t1->placedPosition.x, t1->placedPosition.y);
	line* newLine = new line{
			intPair{t0->placedPosition.x, t0->placedPosition.y}, intPair{t1->placedPosition.x, t1->placedPosition.y}};
	currentNet->renderedLine.push_back(newLine);

	clearActiveObstacles();
	sortObstacles();
	return true;
}

// Adds atmost two actives for each endSegment
void routing::addActiveFunction(endSegment* ePrevHighest, endSegment* e, endSegment* eNextHighest, activeSegment* actS,
		std::unordered_set<activeSegment*>& newActiveSegments) {
	obstacleSegment::obstacleType oType =
			(activesA.find(actS) != activesA.end()) ? obstacleSegment::startA : obstacleSegment::startB;

	if (e->index + e->length > ePrevHighest->index + ePrevHighest->length) {
		activeSegment* leftSegment =
				new activeSegment{actS->bends + 1, e->crossovers, e->end1, 0, 0, left, e->baseSegment};
		leftSegment->scanDirection = actS->scansVertical() ? left : down;
		if (actS->isUpRight()) {
			leftSegment->end1 = std::max(e->nearIndex(), ePrevHighest->farIndex() + strokeWidth);
			leftSegment->end2 = e->farIndex();
		} else {
			leftSegment->end1 = e->farIndex();
			leftSegment->end2 = std::min(e->nearIndex(), ePrevHighest->farIndex() - strokeWidth);
		}
		newActiveSegments.insert(leftSegment);
		addObstacle(
				leftSegment->index, leftSegment->end1, leftSegment->end2, oType, leftSegment, !actS->scansVertical());
	}
	if (e->index + e->length > eNextHighest->index + eNextHighest->length) {
		activeSegment* rightSegment =
				new activeSegment{actS->bends + 1, e->crossovers, e->end2, 0, 0, right, e->baseSegment};
		rightSegment->scanDirection = actS->scansVertical() ? right : up;
		if (actS->isUpRight()) {
			rightSegment->end1 = std::max(e->nearIndex(), eNextHighest->farIndex() + strokeWidth);
			rightSegment->end2 = e->farIndex();
		} else {
			rightSegment->end1 = e->farIndex();
			rightSegment->end2 = std::min(e->nearIndex(), eNextHighest->farIndex() - strokeWidth);
		}
		newActiveSegments.insert(rightSegment);
		addObstacle(rightSegment->index, rightSegment->end1, rightSegment->end2, oType, rightSegment,
				!actS->scansVertical());
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
	if (actS) {
		while (actS->prevSegment) {
			// measures length between actS and actS->prev
			length += std::abs(x - actS->prevSegment->index);
			x = actS->index;
			actS = actS->prevSegment;
		}
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
	if (s.end2 - s.end1 > 2 * strokeWidth) {
		// NOTE: need to work on how to decide the closest index
		closestIndex = std::abs(s.end2 - 3 * strokeWidth - actSegment->prevSegment->index) >
									   std::abs(s.end1 + 3 * strokeWidth - actSegment->prevSegment->index)
							   ? s.end1 + 3 * strokeWidth
							   : s.end2 - 3 * strokeWidth;

		//		closestIndex = (s.end1 + s.end2) / 2;
	} else {
		// NOTE: Need to add another option to increase cost!!
		totalBends += 10000;
		closestIndex = (s.end1 + s.end2) / 2;
	}

	intPair joinPoint = actSegment->scansVertical() ? intPair{closestIndex, s.index} : intPair{s.index, closestIndex};

	length = pathLength(actSegment, closestIndex) + std::abs(actSegment->index - obstacle->index);

	if (obstacle->type == obstacleSegment::startA || obstacle->type == obstacleSegment::startB) {
		otherActSegment = static_cast<activeSegment*>(obstacle->sourcePtr);
		// NOTE: DO WE NOT NEED TO CHECK THE SEGMENT INSTEAD??
		length += pathLength(otherActSegment, closestIndex);
		totalBends += otherActSegment->bends;
		totalCrossovers += otherActSegment->crossedNets;
	} else if (obstacle->type == obstacleSegment::net) {
		length += std::abs(obstacle->index - s.index);
		otherActSegment = *activesB.insert(new activeSegment{obstacle->index, closestIndex, closestIndex,
												   actSegment->scanDirection, nullptr})
								   .first;
	}

	if (soln.cost > calculateCost(totalBends, totalCrossovers, length)) {
		soln.a = actSegment;
		soln.b = otherActSegment;
		soln.cost = calculateCost(totalBends, totalCrossovers, length);
		soln.optimalPoint = joinPoint;
	}
}

void routing::expandNet(splicedTerminal* t) {
	initActives(activesA, t);
	bool solutionFound = false;
	while (!solutionFound) {
		std::unordered_set<activeSegment*> A_new;
		solutionFound |= expandActives(activesA, A_new);
		for (activeSegment* a : A_new) {
#ifdef WEB_COMPILATION
			activeALine(a->scansVertical() ? a->end1 : a->index, a->scansVertical() ? a->index : a->end1,
					a->scansVertical() ? a->end2 : a->index, a->scansVertical() ? a->index : a->end2);
#endif  // WEB_COMPILATION
			activesA.insert(a);
		}
	}
	clearActiveSet(activesA);
	clearActiveSet(activesB);
	clearActiveSet(inactives);
#ifdef WEB_COMPILATION
	clearActives();
#endif  // WEB_COMPILATION
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

void routing::clearActiveSet(std::unordered_set<activeSegment*>& set) {
	for (activeSegment* a : set) {
		delete a;
	}
	set.clear();
}
