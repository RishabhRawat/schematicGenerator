#include "placement.h"
#include "coreDesign.h"
#include <strings.h>
#include <iostream>
#include <iterator>
#include <sstream>

coreDesign::~coreDesign() {

	for (auto&& item : internalNets) {
		delete item.second;
	}

	for (auto&& item : internalCoalescedNets) {
		delete item;
	}

	for (auto&& item : subModules) {
		delete item.second;
	}
}

void coreDesign::doPlacement() {
	initializeStructures();
	placement placementObject;
	placementObject.place(this,designParameters);
}

void coreDesign::initializeStructures() {
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
			if (t_pair.second->type == terminalType::outType)
				t_pair.second->side = terminalSide::rightSide;
			else
				t_pair.second->side = terminalSide::leftSide;

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
					internalCoalescedNets.insert(newCNet);
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
					// FIXME: Possible bug in implementation
					moduleLinkMap::iterator mapIterator = m.second->connectedModuleLinkMap.find(mST_pair.first);
					if (mapIterator == m.second->connectedModuleLinkMap.end()) {
						linkVector newCollection{new ulink(t->getCoalescedNet(), t, &mST_pair.second)};
						m.second->connectedModuleLinkMap.insert(
								std::pair<module*, linkVector>(mST_pair.first, newCollection));
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
			if (t->getType() == terminalType::outType) {
				output++;
			} else
				input++;
		}
		int maxout = output + 1;
		int maxin = input + 1;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->getType() == terminalType::outType)
				t->originalPosition = {m.second->size.x, (output-- * m.second->size.y) / maxout};
			else
				t->originalPosition = {0, (input-- * m.second->size.y) / maxin};
			t->placedPosition = t->originalPosition;
		}
	}
}

terminal& coreDesign::addSystemTerminal(
		const std::string& terminalName, const terminalType type, const int width) {
	return *systemModule.moduleTerminals
					.insert({terminalName, new terminal(terminalName, type, width, &systemModule, true)})
					.first->second;
}

void coreDesign::routing() {
	addObstacleBounding();
	for (coalescedNet* internalCoalescedNet : internalCoalescedNets) {
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

void coreDesign::addObstacleBounding() {}

void coreDesign::initNet(splicedTerminal* t0, splicedTerminal* t1) {}

void coreDesign::expandNet(splicedTerminal* pT) {}

terminal& coreDesign::getSystemTerminal(const std::string& terminalIdentifier) {
	return systemModule.getTerminal(terminalIdentifier);
}

module& coreDesign::addModule(const std::string& moduleName) {
	return *(subModules.insert(std::make_pair(moduleName, new module(moduleName))).first->second);
}

module& coreDesign::getModule(const std::string& moduleName) {
	return *subModules.find(moduleName)->second;
}

net& coreDesign::addNet(const std::string& netName, const int netWidth) {
	return *(internalNets.insert({netName, new net(netName, netWidth)}).first->second);
}

net& coreDesign::getNet(const std::string& netName) {
	return *(internalNets.find(netName)->second);
}