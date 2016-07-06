#include "routing.h"
#include <unordered_set>
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
	if (t->isSystemTerminal()) {
		activeSet.insert(
				new activeSegment{nullptr, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::up}(nullptr, segment(), direction::left));
		activeSet.insert(
				new activeSegment{nullptr, segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::down}(nullptr, segment(), direction::right));

		activeSet.insert(
				new activeSegment{nullptr, segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::left}(nullptr, segment(), direction::up));
		activeSet.insert(
				new activeSegment{nullptr, segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::right}(nullptr, segment(), direction::down));
	} else {
		switch (t->baseTerminal->side) {
			case terminalSide::leftSide:
				activeSet.insert(new activeSegment{nullptr,
						segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::left}(nullptr, segment(), direction::left));
				break;
			case terminalSide::rightSide:
				activeSet.insert(new activeSegment{nullptr,
						segment{t->placedPosition.x, t->placedPosition.y, t->placedPosition.y},
						direction::right}(nullptr, segment(), direction::right));
				break;
			case terminalSide::topSide:
				activeSet.insert(new activeSegment{nullptr,
						segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::up}(nullptr, segment(), direction::up));
				break;
			case terminalSide::bottomSide:
				activeSet.insert(new activeSegment{nullptr,
						segment{t->placedPosition.y, t->placedPosition.x, t->placedPosition.x},
						direction::down}(nullptr, segment(), direction::down));
				break;
			case terminalSide::noneSide:
				throw std::runtime_error("Cannot have a terminal without a side");
		}
	}
}

void routing::expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
		std::unordered_set<activeSegment*>& incrementedActSegmentSet) {
	int minCrossover = INT32_MAX, minLength = INT32_MAX;
	int j = 0;
	for (activeSegment* s : actSegmentSet)
		j = std::max(j, s->currentSegment.index);

	for (activeSegment* s : actSegmentSet) {
		if (s->currentSegment.index == j)
			expandSegment(s, incrementedActSegmentSet);
	}

	if (solutionFound)
		reconstructSolution();

	if(actSegmentSet.empty())
		throw("No solution found!!");
}

void routing::reconstructSolution() {
	intPair optimum;
	activeSegment *optA, *optB;  // NOTE: Somehow there is an assumption both have same direction (vertical/ horizontal)
	switch (optA->dir) {
		case direction::up:
		case direction::down:
			int min = optA->currentSegment.index;
			int max = optB->currentSegment.index;
			if (max < min)
				std::swap(min, max);
			vObstacleSet.insert(
					new obstacleSegment{optimum.x, min, max, obstacleSegment::obstacleType::net, currentNet});
			reconstructPath(optA, optimum.x);
			reconstructPath(optB, optimum.x);
			break;
		case direction::left:
		case direction::right:
			int min = optA->currentSegment.index;
			int max = optB->currentSegment.index;
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

void routing::expandNet(splicedTerminal* pT) {}

void routing::expandSegment(routing::activeSegment* actSegment, std::unordered_set<activeSegment*>& actSegmentSet) {
	struct simpleSegment {
		int end1, end2, crossovers;

		simpleSegment(int segmentEnd1, int segmentEnd2, int segmentCrossovers)
			: end1(segmentEnd1), end2(segmentEnd2), crossovers(segmentCrossovers) {}
	};
	std::vector<simpleSegment *> R, R_dash;
	R.emplace_back(new simpleSegment{
			actSegment->currentSegment.end1, actSegment->currentSegment.end2, actSegment->crossedNets});

	int i = actSegment->currentSegment.index;
	while (!R.empty()) {
		std::unordered_set<obstacleSegment*> obsSet;

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
					R.emplace_back(new simpleSegment(r->end1, obst->end1 - 1, r->end1));
					R_dash.emplace_back(new simpleSegment(r->end1, obst->end1 - 1, r->end1));
					r->end1 = obst->end1;
				}
				if (r->end2 > obst->end2) {
					R.emplace_back(new simpleSegment(obst->end2 + 1, r->end2, r->end1));
					R_dash.emplace_back(new simpleSegment(obst->end2 + 1, r->end2, r->end1));
					r->end1 = obst->end1;
				}
				switch (obst->type) {
					case obstacleSegment::obstacleType::module:
						solidObstacle(i, r->end1, r->end2, actSegment);
						break;
					case obstacleSegment::obstacleType::net:
						if (obst->net != currentNet)
							transparentObstacle();
						else {
							solutionFound = true;
							updateSolution(obst, i, r, actSegment);
						}
						break;
					case obstacleSegment::obstacleType::startA:
						if (activesA.find(actSegment) == activesA.end())
							break;
						solidObstacle(i, r->end1, r->end2, actSegment);
						// TODO: complete case
						break;
					case obstacleSegment::obstacleType::startB:
						if (activesB.find(actSegment) == activesB.end())
							break;
						solidObstacle(i, r->end1, r->end2, actSegment);
						break;
						// TODO: complete case
				}
			}
		}
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

void routing::solidObstacle(int i, int end1, int end2, activeSegment* actS) {
	if (actS->dir == routing::direction::up || actS->dir == routing::direction::right)
		i++;
	else if (actS->dir == routing::direction::down || actS->dir == routing::direction::left)
		i--;
	else
		throw std::runtime_error("Unexpected Direction!!");

	E.insert(new endSegment{i, end1, end2, endSegment::endType::segment});
}

void routing::transparentObstacle() {}

void routing::newActives(routing::activeSegment* actSegment, std::unordered_set<activeSegment*>& newActSegmentSet) {}

void routing::reconstructPath(activeSegment* actS, int x) {
	while (actS->bends != -1) {
		int min = x;
		int max = actS->originSegment->currentSegment.index;
		if (min > max)
			std::swap(min, max);
		switch (actS->originSegment->dir) {
			case direction::up:
			case direction::down:
				vObstacleSet.insert(new obstacleSegment{
						actS->currentSegment.index, min, max, obstacleSegment::obstacleType::net, currentNet});
				break;
			case direction::left:
			case direction::right:
				hObstacleSet.insert(new obstacleSegment{
						actS->currentSegment.index, min, max, obstacleSegment::obstacleType::net, currentNet});
				break;
		}
		actS = actS->originSegment;
		x = actS->originSegment->currentSegment.index;
	}
}

int routing::pathLength(activeSegment* actS, int x) {
	int length = 0;
	while (actS->originSegment) {
		length += std::abs(x - actS->currentSegment.index);
		actS = actS->originSegment;
		x = actS->currentSegment.index;
	}
}