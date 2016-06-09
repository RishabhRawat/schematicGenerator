#include "schematicGenerator.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include "box.h"
#include "partition.h"

schematicGenerator::~schematicGenerator() {
	for (partition* p : allPartitions) {
		delete p;
	}
}

void schematicGenerator::doPlacement() {
	printInitialStructures();
	initializeStructures();
	printDerivedStructures();
	partitionFormation();
	printPartitions();
	boxFormation();
	modulePlacement();
	boxPlacement();
	partitionPlacement();
	terminalPlacement();
}

void schematicGenerator::initializeStructures() {
	/*
	 * First Preprocessing Technique is no coelescing, and using standard nets
	 * themselves as
	 * coelesced nets.
	 * The terminals will be sliced and if a terminal is fanout to different
	 * nets,
	 * then there will be two copies of that terminal
	 */

	subModules.insert({systemModule.moduleIdentifier, &systemModule});
	// Iterates over all bitTerminals in the whole system
	for (namedModulePair& m_pair : subModules) {
		for (namedTerminalPair& t_pair : m_pair.second->moduleTerminals) {
			// TODO: Fix terminal placement
			if (t_pair.second->type == schematic::outType)
				t_pair.second->side = schematic::rightSide;
			else
				t_pair.second->side = schematic::leftSide;

			hashlib::dict<net*, std::vector<int>> attachedNetIndexes;
			for (int i = 0; i < t_pair.second->terminalWidth; ++i) {
				for (bitNet* bN : t_pair.second->internalBitTerminals[i]->connectedBitNets) {
					auto pair = attachedNetIndexes.find(bN->baseNet);
					if (pair == attachedNetIndexes.end())
						attachedNetIndexes.insert({bN->baseNet, {i}});
					else
						pair->second.push_back(i);
				}
			}
			// Adding spliced Terminals
			// TODO: Implement a more efficient string builder?
			for (auto&& nI_pair : attachedNetIndexes) {
				std::ostringstream imploded;
				imploded << t_pair.second->terminalIdentifier << "_";
				std::copy(nI_pair.second.begin(), nI_pair.second.end(), std::ostream_iterator<int>(imploded, "_"));
				// Insertion into module
				splicedTerminal* newST =
						m_pair.second->addSplicedTerminal(t_pair.second, imploded.str(), nI_pair.first);
				// Insertion into coalesced List
				if (nI_pair.first->coalesced == nullptr) {
					coalescedNet* newCNet = new coalescedNet(nI_pair.first);
					newCNet->connectedModuleSplicedTerminalMap.insert({m_pair.second, {newST}});
					nI_pair.first->coalesced = newCNet;
					coalescedNetSet.insert(newCNet);
				} else {
					auto cMST_iter = nI_pair.first->coalesced->connectedModuleSplicedTerminalMap.find(m_pair.second);
					if (cMST_iter == nI_pair.first->coalesced->connectedModuleSplicedTerminalMap.end())
						nI_pair.first->coalesced->connectedModuleSplicedTerminalMap.insert({m_pair.second, {newST}});
					else
						cMST_iter->second.push_back(newST);
				}
			}
		}
	}

	// Setting up module connectivity
	for (namedModulePair& m : subModules) {
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			for (moduleSplicedTerminalPair& mST_pair : t->getCoalescedNet()->connectedModuleSplicedTerminalMap) {
				if (mST_pair.first != m.second) {
					moduleLinkMap::iterator mapIterator = m.second->connectedModuleLinkMap.find(mST_pair.first);
					if (mapIterator == m.second->connectedModuleLinkMap.end()) {
						linkCollection* newCollection = new linkCollection();
						newCollection->push_back(new ulink(t->getCoalescedNet(), t, &mST_pair.second));
						m.second->connectedModuleLinkMap.insert({mST_pair.first, *newCollection});
					} else {
						mapIterator->second.push_back(new ulink(t->getCoalescedNet(), t, &mST_pair.second));
					}
				}
			}
		}
	}
	subModules.erase(systemModule.moduleIdentifier);

	// Terminal Positioning
	for (namedModulePair m : subModules) {
		int input = 0;  // count inout on left side :/
		int output = 0;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->getType() == schematic::outType) {
				output++;
			} else
				input++;
		}
		int maxout = output + 1;
		int maxin = input + 1;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->getType() == schematic::outType)
				t->originalPosition = {m.second->size.x, (output-- * m.second->size.y) / maxout};
			else
				t->originalPosition = {0, (input-- * m.second->size.y) / maxin};
		}
	}
}

void schematicGenerator::partitionFormation() {
	module* seed;
	hashlib::pool<module*> moduleSet;

	for (namedModulePair& m : subModules)
		moduleSet.insert(m.second);

	while (!moduleSet.empty()) {
		seed = selectPartitionSeed(moduleSet);
		moduleSet.erase(seed);
		allPartitions.emplace_back(createPartition(moduleSet, seed));
	}
}

module* schematicGenerator::selectPartitionSeed(hashlib::pool<module*> moduleSet) const {
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
		throw "Bad moduleSet in schematicGenerator::selectPartitionSeed";
	return seed;
}

partition* schematicGenerator::createPartition(hashlib::pool<module*>& moduleSet, module* seed) {
	partition* newPartition = new partition();
	newPartition->addModule(seed);
	unsigned int connections = 0;
	unsigned int partitionEntries = 0;
	while (!moduleSet.empty() && (partitionEntries < maxPartitionSize) && (connections < maxPartitionConnections)) {
		module* selectedModule = nullptr;
		int maxConnectionsInPartition = -1;
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

		if (!selectedModule || maxConnectionsInPartition < 0 || minConnectionsOutPartion == INT32_MAX)
			throw "Bad selectedModule in schematicGenerator::createPartition";

		moduleSet.erase(selectedModule);
		newPartition->addModule(selectedModule);
		connections -= maxConnectionsInPartition;
		connections += minConnectionsOutPartion;
	}
	return newPartition;
}

moduleSet* schematicGenerator::selectBoxSeeds(partition* p) {
	if (p->length() > 1)
		throw std::runtime_error("Invalid Partition Error: placment::selectBoxSeeds");

	moduleSet* roots = new hashlib::pool<module*>();
	for (auto&& m : p->partitionModules) {
		bool seed = false;
		// FIXME: what about when there are no connectedModuleLinkMap entries ??
		for (moduleLinkPair pair : m->connectedModuleLinkMap) {
			if (p->partitionModules.find(pair.first) == p->partitionModules.end() && pair.first != &systemModule) {
				seed = true;
			} else if (pair.first == &systemModule) {
				for (ulink* l : pair.second) {
					for (splicedTerminal* t : *(l->linkSink)) {
						if (t->getType() == schematic::inType || t->getType() == schematic::inoutType) {
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
				if (t->getType() == schematic::outType)
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

box* schematicGenerator::selectPath(box* path, moduleSet remainingModules) {
	bool searchSuccess = true;

	while (searchSuccess && path->length() <= maxPathLength) {
		searchSuccess = false;
		module* lastModule = path->boxModules.back();
		for (auto&& m : remainingModules) {
			moduleLinkMap::iterator mapIterator = lastModule->connectedModuleLinkMap.find(m);
			if (mapIterator != lastModule->connectedModuleLinkMap.end()) {
				for (ulink* l : mapIterator->second) {
					if (l->linkSource->getType() == schematic::inType ||
							l->linkSource->getType() == schematic::inoutType) {
						for (splicedTerminal* t : *(l->linkSink)) {
							if (t->getType() == schematic::outType || t->getType() == schematic::inoutType) {
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

void schematicGenerator::boxFormation() {
	for (partition* p : allPartitions) {
		moduleSet* seeds = selectBoxSeeds(p);
		if (seeds->empty())
			throw "No seeds";
		while (!p->partitionModules.empty()) {  // FIXME: What if stuck in this loop
			box* longestPath = nullptr;
			for (module* seed : *seeds) {
				p->partitionModules.erase(seed);
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
	}
}

terminal& schematicGenerator::addSystemTerminal(
		const std::string& terminalName, const schematic::terminalType type, const int width) {
	return *systemModule.moduleTerminals
					.insert({terminalName, new terminal(terminalName, type, width, &systemModule, true)})
					.first->second;
}

void schematicGenerator::modulePlacement() {
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

void schematicGenerator::initModulePlacement(box* b, intPair& leftBottom, intPair& rightTop) {
	module* root = b->boxModules.front();
	splicedTerminal* out = b->boxLink.front().first;
	if (b->length() > 1) {
		switch (out->baseTerminal->side) {
			case schematic::rightSide:
				root->rotateModule(schematic::d_0);
				break;
			case schematic::topSide:
				root->rotateModule(schematic::d_90);
				break;
			case schematic::leftSide:
				root->rotateModule(schematic::d_180);
				break;
			case schematic::bottomSide:
				root->rotateModule(schematic::d_270);
				break;
			case schematic::noneSide:
				throw std::runtime_error(
						"Invalid side of terminal, Have you initialized "
						"terminal positions?");
		}
	}

	unsigned int lT, rT, bT, tT;
	lT = rT = bT = tT = 0;

	for (splicedTerminal* t : root->moduleSplicedTerminals) {
		switch (t->baseTerminal->side) {
			case schematic::leftSide:
				lT++;
				break;
			case schematic::bottomSide:
				bT++;
				break;
			case schematic::rightSide:
				rT++;
				break;
			case schematic::topSide:
				tT++;
				break;
			case schematic::noneSide:
				throw std::runtime_error("Invalid terminal side assignments");
		}
	}

	root->position = {static_cast<int>(calculatePadding(lT)), static_cast<int>(calculatePadding(bT))};
	leftBottom = {0, 0};
	rightTop = root->position + root->size + intPair{calculatePadding(rT), calculatePadding(tT)};
}

void schematicGenerator::placeModule(box* b, unsigned int index, intPair& leftBottom, intPair& rightTop) {
	module* m_prev = b->boxModules[index - 1];
	module* m = b->boxModules[index];
	splicedTerminal* source = b->boxLink[index - 1].first;
	splicedTerminal* sink = b->boxLink[index - 1].second;

	switch (sink->baseTerminal->side) {
		case schematic::rightSide:
			m->rotateModule(schematic::d_180);
			break;
		case schematic::topSide:
			m->rotateModule(schematic::d_270);
			break;
		case schematic::leftSide:
			m->rotateModule(schematic::d_0);
			break;
		case schematic::bottomSide:
			m->rotateModule(schematic::d_90);
			break;
		case schematic::noneSide:
			throw std::runtime_error(
					"Invalid side of terminal, Have you initialized terminal "
					"positions?");
	}

	switch (source->baseTerminal->side) {
		case schematic::rightSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y;
			break;
		case schematic::topSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y +
							designParameters.wireModuleDistance;
			break;
		case schematic::leftSide:
			if (source->placedPosition.y > m_prev->size.y / 2)
				m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y +
								designParameters.wireModuleDistance;
			else
				m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y -
								designParameters.wireModuleDistance;
			break;
		case schematic::bottomSide:
			m->position.y = m_prev->position.y + source->placedPosition.y - sink->placedPosition.y -
							designParameters.wireModuleDistance;
			break;
		case schematic::noneSide:
			throw std::runtime_error(
					"Invalid side of terminal, Have you initialized terminal "
					"positions?");
	}

	unsigned int lT, rT, bT, tT;
	lT = rT = bT = tT = 0;

	for (splicedTerminal* t : m->moduleSplicedTerminals) {
		switch (t->baseTerminal->side) {
			case schematic::leftSide:
				lT++;
				break;
			case schematic::bottomSide:
				bT++;
				break;
			case schematic::rightSide:
				rT++;
				break;
			case schematic::topSide:
				tT++;
				break;
			case schematic::noneSide:
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

int schematicGenerator::calculatePadding(unsigned int n) {
	return (10 + 2 * n);
}

void schematicGenerator::boxPlacement() {
	for (partition* p : allPartitions) {
		hashlib::dict<box*, positionalStructure<box>> layoutData;
		box* largestBox = *std::max_element(p->partitionBoxes.begin(), p->partitionBoxes.end(),
				[](const box* x, const box* y) { return x->length() < y->length(); });

		largestBox->position = {0, 0};
		layoutData.insert(std::make_pair(largestBox, positionalStructure<box>()));
		intPair leftBottom;
		intPair rightTop = largestBox->size;

		hashlib::pool<box *> remainingBoxes, placedBoxes;

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

			auto&& pS = layoutData.insert(std::make_pair(b, positionalStructure<box>()));
			for (auto&& p : layoutData) {
				p.second.add(b);
				pS.first->second.add(p.first);
			}
			leftBottom = {std::min(leftBottom.x, b->position.x), std::min(leftBottom.y, b->position.y)};
			rightTop = {std::min(leftBottom.x, b->position.x + b->size.x),
					std::min(leftBottom.y, b->position.y + b->size.y)};
		}
		p->size = rightTop - leftBottom;
		p->offset = leftBottom;
	}
}

box* schematicGenerator::selectNextBox(
		const hashlib::pool<box*>& remainingBoxes, const hashlib::pool<box*>& placedBoxes) {
	// TODO: Verify that this works
	return *std::max_element(remainingBoxes.begin(), remainingBoxes.end(), [&](box* lhs, box* rhs) {
		int sumLHS = std::accumulate(lhs->boxModules.begin(), lhs->boxModules.end(), 0, [&](int sum, module* m) {
			return sum + std::count_if(m->connectedModuleLinkMap.begin(), m->connectedModuleLinkMap.end(),
								 [&](moduleLinkPair& pair) {
									 return placedBoxes.find(pair.first->parentBox) != placedBoxes.end();
								 });
		});
		int sumRHS = std::accumulate(rhs->boxModules.begin(), rhs->boxModules.end(), 0, [&](int sum, module* m) {
			return sum + std::count_if(m->connectedModuleLinkMap.begin(), m->connectedModuleLinkMap.end(),
								 [&](moduleLinkPair& pair) {
									 return placedBoxes.find(pair.first->parentBox) != placedBoxes.end();
								 });
		});
		return sumLHS < sumRHS;
	});
}

intPair schematicGenerator::calculateOptimumBoxPosition(const box* b, hashlib::pool<box*>& placedBoxes) {
	// The number of terminals of modules in box b which have connections which
	// connect to the other boxes
	// ie. terminals crossing the boundary
	intPair boxGrav = {0, 0};
	intPair restGrav = {0, 0};
	int boxCount = 0;
	int restCount = 0;

	for (module* boxModule : b->boxModules) {
		for (moduleLinkPair& m_pair : boxModule->connectedModuleLinkMap) {
			if (m_pair.first->parentBox != b && m_pair.first != &systemModule &&
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
intPair schematicGenerator::calculateActualPosition(
		const intPair size, const intPair optimumPosition, hashlib::dict<T*, positionalStructure<T>>& layoutData) {
	// TODO: This algorithm needs a lot of testing!!!!!!!!!!!!!!!!!!!!! yeah that many exclaimations :/
	intPair bestPosition = {INT32_MAX, INT32_MAX};
	unsigned int bestDistance = UINT32_MAX;

	enum direction { top = 0, right = 1, bottom = 2, left = 3 };

	auto obstructionPointer = [&](const intPair point, T* const attachedRect, const direction d) -> const T* {
		// Check all rectangles in that direction for overlaps
		for (const T* const obst : layoutData.find(attachedRect)->second.side[d]) {
			// If one rectangle is on left side of other
			if (obst->position.x > point.x + size.x || point.x > obst->position.x + obst->size.x)
				continue;

			// If one rectangle is above other
			if (obst->position.y > point.y + size.y || point.y > obst->position.y + obst->size.y)
				continue;

			return obst;
		}
		return nullptr;
	};

	auto scanEdge = [&](const intPair p0, const intPair p1, T* const rectangle, const int orientation) {
		if (p0[orientation] > optimumPosition[orientation]) {
			intPair point = p0;
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, rectangle, top);
				if (ptr)
					point[orientation] = ptr->size[orientation] + ptr->position[orientation];
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[orientation] <= p1[orientation]);
		} else if (p1[orientation] > optimumPosition[orientation]) {
			intPair point = {optimumPosition[orientation], p0[1 - orientation]};
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					break;
				const T* const ptr = obstructionPointer(point, rectangle, top);
				if (ptr)
					point[orientation] = ptr->size[orientation] + ptr->position[orientation];
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					break;
				}
			} while (point[orientation] <= p1[orientation]);
			point = {optimumPosition[orientation], p0[1 - orientation]};
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, rectangle, top);
				if (ptr)
					point[orientation] = ptr->position[orientation];
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[orientation] <= p1[orientation]);
		} else {
			intPair point = p1;
			do {
				if (intPair::L2norm_sq(point, optimumPosition) > bestDistance)
					return;
				const T* const ptr = obstructionPointer(point, rectangle, top);
				if (ptr)
					point[orientation] = ptr->position[orientation];
				else {
					bestDistance = intPair::L2norm_sq(point, optimumPosition);
					bestPosition = point;
					return;
				}
			} while (point[orientation] >= p0[orientation]);
		}
	};

	for (std::pair<T*, positionalStructure<T>> p : layoutData) {
		if (p.first->position.x < optimumPosition.x)  // left
			scanEdge(p.first->getVertex(0), p.first->getVertex(1), p.first, 1);
		if (p.first->position.y + p.first->size.y > optimumPosition.y)  // top
			scanEdge(p.first->getVertex(1), p.first->getVertex(2), p.first, 0);
		if (p.first->position.x + p.first->size.x > optimumPosition.x)  // right
			scanEdge(p.first->getVertex(2), p.first->getVertex(3), p.first, 1);
		if (p.first->position.y < optimumPosition.y)  // bottom
			scanEdge(p.first->getVertex(3), p.first->getVertex(0), p.first, 1);
	};

	if (bestDistance == INT32_MAX)
		throw std::runtime_error("Some error in algorithm");
	return bestPosition;
}
void schematicGenerator::partitionPlacement() {
	unsigned int maxModules = 0;
	partition* largestPartition = nullptr;
	hashlib::dict<partition*, positionalStructure<partition>> layoutData;
	for (partition* p : allPartitions) {
		unsigned int m = std::accumulate(p->partitionBoxes.begin(), p->partitionBoxes.end(),
				static_cast<unsigned int>(0), [](unsigned int sum, box* b) { return sum + b->length(); });
		if (m > maxModules) {
			maxModules = m;
			largestPartition = p;
		}
	}
	if (largestPartition == nullptr)
		throw std::runtime_error("Check partitions, all partitions are empty!!!");

	largestPartition->position = {0, 0};
	intPair leftBottom = {0, 0};
	intPair rightTop = largestPartition->size;

	hashlib::pool<partition *> remainingPartitions, placedPartitions;

	for_each(allPartitions.begin(), allPartitions.end(), [&](partition* p) {
		if (p != largestPartition)
			remainingPartitions.insert(p);
	});
	while (!remainingPartitions.empty()) {
		partition* p = selectNextParition(remainingPartitions, placedPartitions);
		remainingPartitions.erase(p);
		intPair optimumPosition = calculateOptimumPartitionPosition(p,remainingPartitions);
		p->position = calculateActualPosition(p->size, optimumPosition, layoutData);

		auto&& pS = layoutData.insert(std::make_pair(p, positionalStructure<partition>()));
		for (auto&& pair : layoutData) {
			pair.second.add(p);
			pS.first->second.add(pair.first);
		}
		leftBottom = {std::min(leftBottom.x, p->position.x), std::min(leftBottom.y, p->position.y)};
		rightTop = {
				std::min(leftBottom.x, p->position.x + p->size.x), std::min(leftBottom.y, p->position.y + p->size.y)};
	}
	size = rightTop - leftBottom;
	offset = leftBottom;
}
partition* schematicGenerator::selectNextParition(
		hashlib::pool<partition*> remainingPartition, hashlib::pool<partition*> placedPartition) {
	unsigned int max = 0;
	partition* maxConnectedPartition = nullptr;
	for (partition* p : remainingPartition) {
		unsigned int conn = 0;
		for (box* b : p->partitionBoxes) {
			for (module* m : b->boxModules) {
				for (moduleLinkPair& otherM : m->connectedModuleLinkMap) {
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

intPair schematicGenerator::calculateOptimumPartitionPosition(
		partition* p, hashlib::pool<partition*>& placedPartition) {
	intPair partitionGrav = {0, 0};
	intPair restGrav = {0, 0};
	int partitionCount = 0;
	int restCount = 0;
	for (box* b : p->partitionBoxes) {
		for (module* boxModule : b->boxModules) {
			for (moduleLinkPair& m_pair : boxModule->connectedModuleLinkMap) {
				if (m_pair.first->parentBox->parentPartition != p && m_pair.first != &systemModule &&
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

terminal& schematicGenerator::getSystemTerminal(const std::string& terminalIdentifier) {
	return systemModule.getTerminal(terminalIdentifier);
}

module& schematicGenerator::addModule(const std::string& moduleName) {
	return *(subModules.insert(std::make_pair(moduleName, new module(moduleName))).first->second);
}

module& schematicGenerator::getModule(const std::string& moduleName) {
	return *subModules.find(moduleName)->second;
}

net& schematicGenerator::addNet(const std::string& netName, const int netWidth) {
	return *(internalNets.insert({netName, new net(netName, netWidth)}).first->second);
}

net& schematicGenerator::getNet(const std::string& netName) {
	return *(internalNets.find(netName)->second);
}
