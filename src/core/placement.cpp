#include "placement.h"
#include "coreDesign.h"

void placement::place(coreDesign* inputDesign, schematicParameters& parameters) {
	core = inputDesign;
	designParameters = parameters;
	partitionFormation();
	boxFormation();
	modulePlacement();
	boxPlacement();
	partitionPlacement();
	systemTerminalPlacement();
}

void placement::partitionFormation() {
	module* seed;
	hashlib::pool<module*> moduleSet;

	for (namedModulePair& m : core->subModules)
		moduleSet.insert(m.second);

	while (!moduleSet.empty()) {
		seed = selectPartitionSeed(moduleSet);
		moduleSet.erase(seed);
		allPartitions.emplace_back(createPartition(moduleSet, seed));
	}
}

module* placement::selectPartitionSeed(hashlib::pool<module*> moduleSet) const {
	// TODO: Improve this bruteforce Algorithm
	int maxConnectionsInFreeSet = -1;
	int minConnectionsOutFreeSet = INT32_MAX;
	module* seed = nullptr;
	for (module* m : moduleSet) {
		int connectionsInFreeSet = 0;
		int connectionsOutFreeSet = 0;
		// The choice is either to iterate over all modules connected to m
		// or to iterate over all elements in moduleSet and find those connected
		// to m
		for (moduleLinkPair& pair : m->connectedModuleLinkMap) {
			if (moduleSet.find(pair.first) != moduleSet.end())
				connectionsInFreeSet += pair.second.size();
			else
				connectionsOutFreeSet += pair.second.size();
		}

		if ((connectionsInFreeSet > maxConnectionsInFreeSet) ||
				((connectionsInFreeSet == maxConnectionsInFreeSet) &&
						(connectionsOutFreeSet <= minConnectionsOutFreeSet))) {
			seed = m;
			maxConnectionsInFreeSet = connectionsInFreeSet;
			minConnectionsOutFreeSet = connectionsOutFreeSet;
		}
	}
	if (!seed || maxConnectionsInFreeSet < 0 || minConnectionsOutFreeSet == INT32_MAX)
		throw "Bad moduleSet in placement::selectPartitionSeed";
	return seed;
}

partition* placement::createPartition(hashlib::pool<module*>& moduleSet, module* seed) {
	partition* newPartition = new partition();
	newPartition->addModule(seed);
	unsigned int connections = 0;
	unsigned int partitionEntries = 0;
	while (!moduleSet.empty() && (partitionEntries < designParameters.maxPartitionSize) &&
			(connections < designParameters.maxPartitionConnections)) {
		module* selectedModule = nullptr;
		int maxConnectionsInPartition = 1;
		int minConnectionsOutPartion = INT32_MAX;

		// Algo to select module
		for (module* m : moduleSet) {
			int connectionsInPartition = 0;
			int connectionsOutPartition = 0;
			for (moduleLinkPair& pair : m->connectedModuleLinkMap) {
				if (newPartition->contains(pair.first))
					connectionsInPartition += pair.second.size();
				else
					connectionsOutPartition += pair.second.size();
			}

			if ((connectionsInPartition > maxConnectionsInPartition) ||
					((connectionsInPartition == maxConnectionsInPartition) &&
							(connectionsOutPartition <= minConnectionsOutPartion))) {
				selectedModule = m;
				maxConnectionsInPartition = connectionsInPartition;
				minConnectionsOutPartion = connectionsOutPartition;
			}
		}

		if (!selectedModule)
			break;

		moduleSet.erase(selectedModule);
		newPartition->addModule(selectedModule);
		connections -= maxConnectionsInPartition;
		connections += minConnectionsOutPartion;
	}
	return newPartition;
}

moduleSet* placement::selectBoxSeeds(partition* p) {
	if (p->length() > 1)
		throw std::runtime_error("Invalid Partition Error: placment::selectBoxSeeds");

	moduleSet* roots = new hashlib::pool<module*>();
	for (auto&& m : p->partitionModules) {
		bool seed = false;
		// FIXME: what about when there are no connectedModuleLinkMap entries ??
		for (moduleLinkPair pair : m->connectedModuleLinkMap) {
			if (p->partitionModules.find(pair.first) == p->partitionModules.end() &&
					pair.first != &core->systemModule) {
				seed = true;
			} else if (pair.first == &core->systemModule) {
				for (ulink* l : pair.second) {
					for (splicedTerminal* t : *(l->linkSink)) {
						if (t->getType() == terminalType::inType || t->getType() == terminalType::inoutType) {
							seed = true;
							break;
						}
					}
					if (seed)
						break;
				}
			}
			if (seed)
				break;
		}

		int outGoingNets = 0;
		if (!seed) {
			for (splicedTerminal* t : m->moduleSplicedTerminals) {
				if (t->getType() == terminalType::outType)
					outGoingNets++;
				if (outGoingNets > 1)
					break;
			}
		}

		if (seed || outGoingNets == 1)
			roots->insert(m);
	}
	return roots;
}

box* placement::selectPath(box* path, moduleSet remainingModules) {
	for (module* m : remainingModules) {
		remainingModules.erase(m);
	}

	bool searchSuccess = true;
	while (searchSuccess && path->length() <= designParameters.maxPathLength) {
		searchSuccess = false;
		module* lastModule = path->boxModules.back();
		for (auto&& m : remainingModules) {
			moduleLinkMap::iterator mapIterator = lastModule->connectedModuleLinkMap.find(m);
			if (mapIterator != lastModule->connectedModuleLinkMap.end()) {
				for (ulink* l : mapIterator->second) {
					if (l->linkSource->getType() == terminalType::inType ||
							l->linkSource->getType() == terminalType::inoutType) {
						for (splicedTerminal* t : *(l->linkSink)) {
							if (t->getType() == terminalType::outType || t->getType() == terminalType::inoutType) {
								remainingModules.erase(m);
								path->add(m, l->linkSource, t);  // Note The root is added earlier
								searchSuccess = true;
								break;
							}
						}
					}
					if (searchSuccess)
						break;
				}
			}
			if (searchSuccess)
				break;
		}
	}
	return path;
}

void placement::boxFormation() {
	for (partition* p : allPartitions) {
		moduleSet* seeds = selectBoxSeeds(p);
		if (seeds->empty())
			throw "No seeds";
		while (!p->partitionModules.empty()) {  // FIXME: What if stuck in this loop
			box* longestPath = nullptr;
			for (module* seed : *seeds) {
				box* path = new box(seed);
				path = selectPath(path, p->partitionModules);  // Call by Value

				if (longestPath == nullptr || longestPath->length() < path->length()) {
					delete longestPath;
					longestPath = path;
				} else
					delete path;
			}

			for (auto&& m : longestPath->boxModules) {
				p->partitionModules.erase(m);
				m->setParentBox(longestPath);
			}

			seeds->erase(longestPath->boxModules.front());
			p->add(longestPath);
		}
		delete seeds;
	}
}

void placement::modulePlacement() {
	for (partition* p : allPartitions) {
		for (box* b : p->partitionBoxes) {
			intPair rightTop;
			initModulePlacement(b, b->offset, rightTop);
			for (unsigned int i = 1; i < b->length(); ++i) {
				placeModule(b, i, b->offset, rightTop);
			}
			b->size = rightTop - b->offset;
		}
	}
}

void placement::initModulePlacement(box* b, intPair& leftBottom, intPair& rightTop) {
	module* root = b->boxModules.front();
	if (b->length() > 1) {
		splicedTerminal* out = b->boxLink.front().first;
		switch (out->baseTerminal->side) {
			case terminalSide::rightSide:
				root->rotateModule(clockwiseRotation::d_0);
				break;
			case terminalSide::topSide:
				root->rotateModule(clockwiseRotation::d_90);
				break;
			case terminalSide::leftSide:
				root->rotateModule(clockwiseRotation::d_180);
				break;
			case terminalSide::bottomSide:
				root->rotateModule(clockwiseRotation::d_270);
				break;
			case terminalSide::noneSide:
				throw std::runtime_error(
						"Invalid side of terminal, Have you initialized "
						"terminal positions?");
		}
	}

	unsigned int lT, rT, bT, tT;
	lT = rT = bT = tT = 0;

	for (splicedTerminal* t : root->moduleSplicedTerminals) {
		switch (t->baseTerminal->side) {
			case terminalSide::leftSide:
				lT++;
				break;
			case terminalSide::bottomSide:
				bT++;
				break;
			case terminalSide::rightSide:
				rT++;
				break;
			case terminalSide::topSide:
				tT++;
				break;
			case terminalSide::noneSide:
				throw std::runtime_error("Invalid terminal side assignments");
		}
	}

	root->position = {calculatePadding(lT), calculatePadding(bT)};
	leftBottom = {0, 0};
	rightTop = root->position + root->size + intPair{calculatePadding(rT), calculatePadding(tT)};
}

void placement::placeModule(box* b, unsigned int index, intPair& leftBottom, intPair& rightTop) {
	module* m_prev = b->boxModules[index - 1];
	module* m = b->boxModules[index];
	splicedTerminal* source = b->boxLink[index - 1].first;
	splicedTerminal* sink = b->boxLink[index - 1].second;

	switch (sink->baseTerminal->side) {
		case terminalSide::rightSide:
			m->rotateModule(clockwiseRotation::d_180);
			break;
		case terminalSide::topSide:
			m->rotateModule(clockwiseRotation::d_270);
			break;
		case terminalSide::leftSide:
			m->rotateModule(clockwiseRotation::d_0);
			break;
		case terminalSide::bottomSide:
			m->rotateModule(clockwiseRotation::d_90);
			break;
		case terminalSide::noneSide:
			throw std::runtime_error(
					"Invalid side of terminal, Have you initialized terminal "
					"positions?");
	}

	switch (source->baseTerminal->side) {
		case terminalSide::rightSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y;
			break;
		case terminalSide::topSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y +
							designParameters.wireModuleDistance;
			break;
		case terminalSide::leftSide:
			if (source->placedPosition.y > m_prev->size.y / 2)
				m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y +
								designParameters.wireModuleDistance;
			else
				m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y -
								designParameters.wireModuleDistance;
			break;
		case terminalSide::bottomSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y -
							designParameters.wireModuleDistance;
			break;
		case terminalSide::noneSide:
			throw std::runtime_error(
					"Invalid side of terminal, Have you initialized terminal "
					"positions?");
	}

	unsigned int lT, rT, bT, tT;
	lT = rT = bT = tT = 0;

	for (splicedTerminal* t : m->moduleSplicedTerminals) {
		switch (t->baseTerminal->side) {
			case terminalSide::leftSide:
				lT++;
				break;
			case terminalSide::bottomSide:
				bT++;
				break;
			case terminalSide::rightSide:
				rT++;
				break;
			case terminalSide::topSide:
				tT++;
				break;
			case terminalSide::noneSide:
				throw std::runtime_error("Invalid terminal side assignments");
		}
	}

	m->position.x = rightTop.x + calculatePadding(lT);

	intPair temp = m->position + m->size + intPair{calculatePadding(rT), calculatePadding(tT)};
	rightTop.x = temp.x;
	if (temp.y > rightTop.y)
		rightTop.y = temp.y;

	int newBottom = m->position.y - calculatePadding(bT);
	if (newBottom > leftBottom.y)
		leftBottom.y = newBottom;
}

int placement::calculatePadding(unsigned int n) {
	return (10 + 2 * n);
}

void placement::boxPlacement() {
	for (partition* p : allPartitions) {
		hashlib::dict<box*, positionalStructure<box>> layoutData;
		box* largestBox = *std::max_element(p->partitionBoxes.begin(), p->partitionBoxes.end(),
				[](const box* x, const box* y) { return x->length() < y->length(); });

		largestBox->position = {0, 0};
		layoutData.insert(std::make_pair(largestBox, positionalStructure<box>(largestBox)));
		intPair leftBottom = {0, 0};
		intPair rightTop = largestBox->size;

		hashlib::pool<box *> remainingBoxes, placedBoxes;
		placedBoxes.insert(largestBox);

		for_each(p->partitionBoxes.begin(), p->partitionBoxes.end(), [&](box* b) {
			if (b != largestBox)
				remainingBoxes.insert(b);
		});
		while (!remainingBoxes.empty()) {
			box* b = selectNextBox(remainingBoxes, placedBoxes);
			remainingBoxes.erase(b);
			// NOTE: can these two be merged??
			intPair optimumPosition = calculateOptimumBoxPosition(b, placedBoxes);
			b->position = calculateActualPosition(b->size, optimumPosition, layoutData);

			placedBoxes.insert(b);
			auto&& pS = layoutData.insert(std::make_pair(b, positionalStructure<box>(b)));
			for (auto&& p : layoutData) {
				p.second.add(b);
				pS.first->second.add(p.first);
			}
			leftBottom = {std::min(leftBottom.x, b->position.x), std::min(leftBottom.y, b->position.y)};
			rightTop = {
					std::max(rightTop.x, b->position.x + b->size.x), std::max(rightTop.y, b->position.y + b->size.y)};
		}
		p->size = rightTop - leftBottom;
		p->offset = leftBottom;
	}
}

box* placement::selectNextBox(const hashlib::pool<box*>& remainingBoxes, const hashlib::pool<box*>& placedBoxes) {
	// TODO: Verify that this works
	box* nextBox = nullptr;
	unsigned int maxCount = 0;

	for (box* b : remainingBoxes) {
		unsigned int count = 0;
		for (module* m : b->boxModules) {
			for (moduleLinkPair& connectedPair : m->connectedModuleLinkMap) {
				if (placedBoxes.find(connectedPair.first->parentBox) != placedBoxes.end())
					count++;
			}
		}
		if (count > maxCount) {
			maxCount = count;
			nextBox = b;
		}
	}

	if (maxCount == 0)
		throw std::runtime_error("no good next box found??");

	return nextBox;
}

intPair placement::calculateOptimumBoxPosition(const box* b, hashlib::pool<box*>& placedBoxes) {
	// The number of terminals of modules in box b which have connections which
	// connect to the other boxes
	// ie. terminals crossing the boundary
	intPair boxGrav = {0, 0};
	intPair restGrav = {0, 0};
	int boxCount = 0;
	int restCount = 0;

	for (module* boxModule : b->boxModules) {
		for (moduleLinkPair& m_pair : boxModule->connectedModuleLinkMap) {
			if (m_pair.first->parentBox != b && m_pair.first != &core->systemModule &&
					placedBoxes.find(m_pair.first->parentBox) != placedBoxes.end()) {
				boxCount += m_pair.second.size();
				for (ulink* ul : m_pair.second) {
					boxGrav += boxModule->position + b->offset + ul->linkSource->placedPosition;
					restCount += ul->linkSink->size();
					for (splicedTerminal* otherT : *ul->linkSink) {
						restGrav += m_pair.first->position + m_pair.first->parentBox->offset +
									m_pair.first->parentBox->position + otherT->placedPosition;
					}
				}
			}
		}
	}
	// Minimize ||(x + boxGrav.x - restGrav.x ), (y + boxGrav.y - restGrav.y )||
	return (restGrav / restCount - boxGrav / boxCount);  // For x0,y0 (lower bottom)
}

template <typename T>
intPair placement::calculateActualPosition(
		const intPair size, const intPair optimumPosition, hashlib::dict<T*, positionalStructure<T>>& layoutData) {
	intPair bestPosition = {INT32_MAX, INT32_MAX};
	unsigned int bestDistance = UINT32_MAX;

	enum direction { top = 0, right = 1, bottom = 2, left = 3 };

	auto obstructionPointer = [&](
			const intPair point, const positionalStructure<T>& posStruct, const direction d) -> const T* {
		// Check all rectangles in that direction for overlaps
		//		for (const T* const obst : posStruct.side[d]) {
		for (auto&& obst_pair : layoutData) {
			const T* const obst = obst_pair.first;
			// If one rectangle is on left side of other
			if (!(obst->position.x > point.x + size.x || point.x > obst->position.x + obst->size.x ||
						// If one rectangle is above other
						obst->position.y > point.y + size.y || point.y > obst->position.y + obst->size.y))
				return obst;
		}
		return nullptr;
	};

	auto scanEdge = [&](
			const intPair p0, const intPair p1, const positionalStructure<T>& posStruct, const direction orientation) {
		// index == 0 implies horizontal movement, while index == 1 implies vertical movement
		const unsigned int index = static_cast<unsigned int>(orientation % 2);
		if (p0[index] > optimumPosition[index]) {
			intPair point = p0;
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, posStruct, orientation);
				if (ptr)
					point[index] = ptr->size[index] + ptr->position[index] + 1;
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[index] <= p1[index]);
		} else if (p1[index] > optimumPosition[index]) {
			intPair point = optimumPosition.component(index) + p0.component(1 - index);
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					break;
				const T* const ptr = obstructionPointer(point, posStruct, orientation);
				if (ptr)
					point[index] = ptr->size[index] + ptr->position[index] + 1;
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					break;
				}
			} while (point[index] <= p1[index]);
			point = optimumPosition.component(index) + p0.component(1 - index);
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, posStruct, orientation);
				if (ptr)
					point[index] = ptr->position[index] - size[index] - 1;
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[index] >= p0[index]);
		} else {
			intPair point = p1;
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, posStruct, orientation);
				if (ptr)
					point[index] = ptr->position[index] - size[index] - 1;
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[index] >= p0[index]);
		}
	};

	bool obstructed = false;
	for (std::pair<T*, positionalStructure<T>>& p : layoutData) {
		if (!(p.first->position.x > optimumPosition.x + size.x ||
					optimumPosition.x > p.first->position.x + p.first->size.x ||
					// If one rectangle is above other
					p.first->position.y > optimumPosition.y + size.y ||
					optimumPosition.y > p.first->position.y + p.first->size.y)) {
			obstructed = true;
			break;
		}
	};

	if (!obstructed)
		return optimumPosition;

	// NOTE: For svg width?
	for (std::pair<T*, positionalStructure<T>> p : layoutData) {
		if (p.first->position.x < optimumPosition.x + size.x)  // left
			scanEdge(p.first->getVertex(0) - size - intPair{1, 0},
					p.first->getVertex(1) - size.component(0) - intPair{1, 0}, p.second, left);
		if (p.first->position.y + p.first->size.y > optimumPosition.y)  // top
			scanEdge(p.first->getVertex(1) - size.component(0) + intPair{0, 1}, p.first->getVertex(2) + intPair{0, 1},
					p.second, top);
		if (p.first->position.x + p.first->size.x > optimumPosition.x)  // right
			scanEdge(p.first->getVertex(3) - size.component(1) + intPair{1, 0}, p.first->getVertex(2) + intPair{1, 0},
					p.second, right);
		if (p.first->position.y < optimumPosition.y + size.y)  // bottom
			scanEdge(p.first->getVertex(0) - size - intPair{0, 1},
					p.first->getVertex(3) - size.component(1) - intPair{0, 1}, p.second, bottom);
	};

	if (bestPosition.x == INT32_MAX && bestPosition.y == INT32_MAX)
		throw std::runtime_error("Some error in algorithm");
	return bestPosition;
}
void placement::partitionPlacement() {
	unsigned int maxModules = 0;
	partition* largestPartition = nullptr;
	hashlib::dict<partition*, positionalStructure<partition>> layoutData;
	for (partition* p : allPartitions) {
		unsigned int mCount = 0;
		for (box* b : p->partitionBoxes) {
			mCount += b->length();
		}

		if (mCount > maxModules) {
			maxModules = mCount;
			largestPartition = p;
		}
	}
	if (largestPartition == nullptr)
		throw std::runtime_error("Check partitions, all partitions are empty!!!");

	largestPartition->position = {0, 0};
	layoutData.insert(std::make_pair(largestPartition, positionalStructure<partition>(largestPartition)));
	intPair leftBottom = {0, 0};
	intPair rightTop = largestPartition->size;

	hashlib::pool<partition *> remainingPartitions, placedPartitions;

	for_each(allPartitions.begin(), allPartitions.end(), [&](partition* p) {
		if (p != largestPartition)
			remainingPartitions.insert(p);
	});
	placedPartitions.insert(largestPartition);

	while (!remainingPartitions.empty()) {
		partition* p = selectNextParition(remainingPartitions, placedPartitions);
		remainingPartitions.erase(p);
		intPair optimumPosition = calculateOptimumPartitionPosition(p, placedPartitions);
		p->position = calculateActualPosition(p->size, optimumPosition, layoutData);

		placedPartitions.insert(p);
		auto&& pS = layoutData.insert(std::make_pair(p, positionalStructure<partition>(p)));
		for (auto&& pair : layoutData) {
			pair.second.add(p);
			pS.first->second.add(pair.first);
		}
		leftBottom = {std::min(leftBottom.x, p->position.x), std::min(leftBottom.y, p->position.y)};
		rightTop = {std::max(rightTop.x, p->position.x + p->size.x), std::max(rightTop.y, p->position.y + p->size.y)};
	}
	core->size = rightTop - leftBottom;
	core->offset = leftBottom;
}
partition* placement::selectNextParition(
		hashlib::pool<partition*> remainingPartition, hashlib::pool<partition*> placedPartition) {
	unsigned int max = 0;
	partition* maxConnectedPartition = nullptr;
	for (partition* p : remainingPartition) {
		unsigned int conn = 0;
		for (box* b : p->partitionBoxes) {
			for (module* m : b->boxModules) {
				for (moduleLinkPair& otherM : m->connectedModuleLinkMap) {
					if (otherM.first == &core->systemModule)
						continue;
					partition* pP = otherM.first->parentBox->parentPartition;
					if (pP != p && placedPartition.find(pP) != placedPartition.end())
						conn += otherM.second.size();
				}
			}
		}
		if (max < conn) {
			max = conn;
			maxConnectedPartition = p;
		}
	}
	if (maxConnectedPartition == nullptr)
		throw std::runtime_error("Something wrong, multiple isolated partitions??");

	return maxConnectedPartition;
}

intPair placement::calculateOptimumPartitionPosition(partition* p, hashlib::pool<partition*>& placedPartition) {
	intPair partitionGrav = {0, 0};
	intPair restGrav = {0, 0};
	int partitionCount = 0;
	int restCount = 0;
	for (box* b : p->partitionBoxes) {
		for (module* boxModule : b->boxModules) {
			for (moduleLinkPair& m_pair : boxModule->connectedModuleLinkMap) {
				if (m_pair.first != &core->systemModule && m_pair.first->parentBox->parentPartition != p &&
						placedPartition.find(m_pair.first->parentBox->parentPartition) != placedPartition.end()) {
					partitionCount += m_pair.second.size();
					for (ulink* ul : m_pair.second) {
						partitionGrav += boxModule->position + b->offset + b->position + p->offset +
										 ul->linkSource->placedPosition;
						restCount += ul->linkSink->size();
						for (splicedTerminal* otherT : *ul->linkSink) {
							restGrav += m_pair.first->position + m_pair.first->parentBox->offset +
										m_pair.first->parentBox->position +
										m_pair.first->parentBox->parentPartition->offset +
										m_pair.first->parentBox->parentPartition->position + otherT->placedPosition;
						}
					}
				}
			}
		}
	}
	// Minimize ||(x + boxGrav.x - restGrav.x ), (y + boxGrav.y - restGrav.y )||
	return (restGrav / restCount - partitionGrav / partitionCount);  // For x0,y0 (lower bottom)
}

void placement::systemTerminalPlacement() {
	auto calculateTerminalGravity = [&](std::vector<splicedTerminal*>* sinkT) {
		intPair grav{0, 0};
		int count = 0;
		for (splicedTerminal* sT : *sinkT) {
			grav += sT->placedPosition + sT->getParent()->position + sT->getParent()->parentBox->offset +
					sT->getParent()->parentBox->position + sT->getParent()->parentBox->parentPartition->offset +
					sT->getParent()->parentBox->parentPartition->position + core->offset;
			count++;
		}
		return grav / count;
	};

	for (moduleLinkPair& pair : core->systemModule.connectedModuleLinkMap) {
		for (ulink* ul : pair.second) {
			// NOTE: ASSUMING ONLY ONE LINK PER TERMINAL
			intPair gravity = calculateTerminalGravity(ul->linkSink);
			// Currently only terminal directions are left and right, this can be easily changed here
			switch (ul->linkSource->getType()) {
				case terminalType::inType:
					ul->linkSource->placedPosition = {core->offset.x, gravity.y};
					break;
				case terminalType::outType:
					ul->linkSource->placedPosition = {core->offset.x + core->size.x, gravity.y};
					break;
				case terminalType::inoutType:
					if (gravity.x > core->offset.x + core->size.x / 2)
						ul->linkSource->placedPosition = {core->offset.x + core->size.x, gravity.y};
					else
						ul->linkSource->placedPosition = {core->offset.x, gravity.y};
					break;
			}
		}
	}
}

void placement::flattenSchematic() {
	for (partition* p : allPartitions) {
		for (box* b : p->partitionBoxes) {
			for (module* m : b->boxModules) {
				m->position = (m->position - b->offset) + (b->position - p->offset) + (p->position - core->offset);
			}
		}
	}
}

placement::~placement() {
	for (partition* p : allPartitions) {
		delete p;
	}
}
