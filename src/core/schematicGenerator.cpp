#include "schematicGenerator.h"
#include "partition.h"
#include "box.h"
#include <iostream>
#include <sstream>
#include <iterator>

schematicGenerator::~schematicGenerator() {
	for(partition *p: allPartitions) {
		for(box *b: p->partitionBoxes){
			delete b;
		}
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
	 * First Preprocessing Technique is no coelescing, and using standard nets themselves as
	 * coelesced nets.
	 * The terminals will be sliced and if a terminal is fanout to different nets,
	 * then there will be two copies of that terminal
	 */

	subModules.insert({systemModule.moduleIdentifier,&systemModule});
	//Iterates over all bitTerminals in the whole system
	for(namedModulePair &m_pair:subModules) {
		for (namedTerminalPair &t_pair:m_pair.second->moduleTerminals) {
			hashlib::dict<net *, std::vector<int>> attachedNetIndexes;
			for (int i = 0; i < t_pair.second->terminalWidth; ++i) {
				for (bitNet *bN: t_pair.second->internalBitTerminals[i]->connectedBitNets) {
					auto pair = attachedNetIndexes.find(bN->baseNet);
					if (pair == attachedNetIndexes.end())
						attachedNetIndexes.insert({bN->baseNet, {i}});
					else
						pair->second.push_back(i);
				}
			}
			//Adding spliced Terminals
			//TODO: Implement a more efficient string builder?
			for(auto &&nI_pair:attachedNetIndexes) {
				std::ostringstream imploded;
				imploded<<t_pair.second->terminalIdentifier<<"_";
				std::copy(nI_pair.second.begin(), nI_pair.second.end(), std::ostream_iterator<int>(imploded, "_"));
				//Insertion into module
				splicedTerminal *newST = m_pair.second->addSplicedTerminal(t_pair.second, imploded.str(), nI_pair.first);
				//Insertion into coalesced List
				if(nI_pair.first->coalesced == nullptr) {
					coalescedNet * newCNet = new coalescedNet(nI_pair.first);
					newCNet->connectedModuleSplicedTerminalMap.insert({m_pair.second,{newST}});
					nI_pair.first->coalesced = newCNet;
					coalescedNetSet.insert(newCNet);
				}
				else {
					auto cMST_iter = nI_pair.first->coalesced->connectedModuleSplicedTerminalMap
							.find(m_pair.second);
					if(cMST_iter == nI_pair.first->coalesced->connectedModuleSplicedTerminalMap.end())
						nI_pair.first->coalesced->connectedModuleSplicedTerminalMap.insert({m_pair.second,{newST}});
					else
						cMST_iter->second.push_back(newST);

				}
			}
		}
	}


	//Setting up module connectivity
	for(namedModulePair& m: subModules) {
		for(splicedTerminal *t: m.second->moduleSplicedTerminals) {
			for(moduleSplicedTerminalPair& mST_pair: t->getCoalescedNet()->connectedModuleSplicedTerminalMap) {
				if(mST_pair.first != m.second){
					moduleLinkMap::iterator mapIterator = m.second->connectedModuleLinkMap.find(mST_pair.first);
					if(mapIterator == m.second->connectedModuleLinkMap.end()) {
						linkCollection *newCollection = new linkCollection();
						newCollection->push_back(new ulink(t->getCoalescedNet(),t,&mST_pair.second));
						m.second->connectedModuleLinkMap.insert({mST_pair.first,*newCollection});
					}
					else {
						mapIterator->second.push_back(new ulink(t->getCoalescedNet(),t,&mST_pair.second));
					}
				}
			}
		}
	}
	subModules.erase(systemModule.moduleIdentifier);
}

void schematicGenerator::partitionFormation() {
	module * seed;
	hashlib::pool<module*> moduleSet;

	for(namedModulePair& m: subModules)
		moduleSet.insert(m.second);

	while(!moduleSet.empty()){
		seed = selectSeed(moduleSet);
		moduleSet.erase(seed);
		allPartitions.emplace_back(createPartition(moduleSet,seed));
	}
}

module* schematicGenerator::selectSeed(hashlib::pool<module*> moduleSet) const{
	//TODO: Improve this bruteforce Algorithm
	int maxConnectionsInFreeSet = -1;
	int minConnectionsOutFreeSet = INT32_MAX;
	module * seed = nullptr;
	for(module* m: moduleSet) {
		int connectionsInFreeSet = 0;
		int connectionsOutFreeSet = 0;
		// The choice is either to iterate over all modules connected to m
		// or to iterate over all elements in moduleSet and find those connected to m
		for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
			if(moduleSet.find(pair.first) != moduleSet.end())
				connectionsInFreeSet += pair.second.size();
			else
				connectionsOutFreeSet += pair.second.size();
		}

		if((connectionsInFreeSet > maxConnectionsInFreeSet) ||
		   ((connectionsInFreeSet == maxConnectionsInFreeSet) &&
		    (connectionsOutFreeSet <= minConnectionsOutFreeSet))){
			seed = m;
			maxConnectionsInFreeSet = connectionsInFreeSet;
			minConnectionsOutFreeSet = connectionsOutFreeSet;
		}
	}
	if(!seed || maxConnectionsInFreeSet < 0 ||
	   minConnectionsOutFreeSet == INT32_MAX)
		throw "Bad moduleSet in schematicGenerator::selectSeed";
	return seed;
}

partition * schematicGenerator::createPartition(hashlib::pool<module*>& moduleSet, module * seed) {
	box * rootBox = new box();
	rootBox->add(seed);
	partition * newPartition = new partition();
	newPartition->add(rootBox);
	unsigned int connections = 0;
	unsigned int partitionEntries = 0;
	while(!moduleSet.empty() && (partitionEntries < maxPartitionSize)
	      && (connections < maxPartitionConnections)) {

		module * selectedModule = nullptr;
		int maxConnectionsInPartition = -1;
		int minConnectionsOutPartion = INT32_MAX;

		//Algo to select module
		for(module* m: moduleSet) {
			int connectionsInPartition = 0;
			int connectionsOutPartition = 0;
			for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
				if(pair.first->parentBox == rootBox)
					connectionsInPartition += pair.second.size();
				else
					connectionsOutPartition += pair.second.size();
			}

			if((connectionsInPartition > maxConnectionsInPartition) ||
			   ((connectionsInPartition == maxConnectionsInPartition) &&
			    (connectionsOutPartition <= minConnectionsOutPartion))) {
				selectedModule = m;
				maxConnectionsInPartition = connectionsInPartition;
				minConnectionsOutPartion = connectionsOutPartition;
			}
		}

		if(!selectedModule || maxConnectionsInPartition < 0 ||
		   minConnectionsOutPartion == INT32_MAX)
			throw "Bad selectedModule in schematicGenerator::createPartition";


		moduleSet.erase(selectedModule);
		rootBox->add(selectedModule);
		connections -= maxConnectionsInPartition;
		connections +=minConnectionsOutPartion;
	}
	return newPartition;
}

moduleCollection schematicGenerator::selectRoots(partition *p) {
	if(p->partitionBoxes.size()>1) throw std::runtime_error("Invalid Partition Error: placment::selectRoots");

	box * b = p->partitionBoxes.back();
	moduleCollection roots;
	for(module *m:b->boxModules) {
		bool seed = false;
		//FIXME: what about when there are no connectedModuleLinkMap entries ??
		//TODO: Fix bug relating to not adding systemroots as terminals
		for(moduleLinkPair pair: m->connectedModuleLinkMap) {
			if(b->boxModules.find(pair.first) == b->boxModules.end() && pair.first != &systemModule){
				seed = true;
			}
			else if(pair.first == &systemModule) {
				for(ulink *l: pair.second){
					for(splicedTerminal *t: *(l->linkSink)) {
						if (t->getType() == schematic::inType || t->getType() == schematic::inoutType){
							seed = true;
							break;
						}
					}
					if(seed)
						break;
				}
			}
			if(seed)
				break;
		}

		int outGoingNets = 0;
		for(namedTerminalPair &t:m->moduleTerminals) {
			if(t.second->type == schematic::outType)
				outGoingNets++;
			if(outGoingNets > 1)
				break;
		}
		if(seed)
			roots.push_back(m);
	}
	return roots;
}

box * schematicGenerator::selectPath( box *path, box *remainingModules) {
	bool searchSuccess = true;

	while(searchSuccess && path->size()<=maxPathLength) {
		searchSuccess = false;
		//FIXME: module * lastModule = path->boxModules.back();
		module * lastModule = path->boxModules.back();
		for(module *m: remainingModules->boxModules) {
			moduleLinkMap::iterator mapIterator = lastModule->connectedModuleLinkMap.find(m);
			if(mapIterator != lastModule->connectedModuleLinkMap.end()) {
				for(ulink *l:mapIterator->second){
					if(l->linkSource->getType() == schematic::inType || l->linkSource->getType() == schematic::inoutType ){
						for(splicedTerminal *t: *(l->linkSink)){
							if(t->getType() == schematic::outType || t->getType() == schematic::inoutType){
								remainingModules->remove(m);
								path->add(m);
								searchSuccess = true;
								break;
							}
						}
					}
					if(searchSuccess)
						break;
				}
			}
			if(searchSuccess)
				break;
		}
	}
	return path;
}

void schematicGenerator::boxFormation() {

	for(partition *p: allPartitions) {
		moduleCollection roots = selectRoots(p);
		if(roots.empty())
			throw "too less roots";
		box * elements = p->partitionBoxes.pop();
		while(!elements->empty()) {
			box * longestPath = nullptr;
			for(module * root:roots) {
				box * path = new box();
				elements->remove(root);
				path->add(root);
				path = selectPath(path,elements); //This will cause all the elements in the path box

				for (module* m:path->boxModules)
					m->setParentBox(elements);

				if(longestPath == nullptr || longestPath->size() < path->size()) {
					delete longestPath;
					longestPath = path;
				}
				else
					delete path;

			}
			//To take care of parentBox inconsistencies
			for(module *m:longestPath->boxModules)
				m->setParentBox(longestPath);
		}
	}

}

terminal & schematicGenerator::addSystemTerminal(
		const std::string &terminalName, const schematic::terminalType type, const int width) {
	//FIXME: I need to check whether the returned reference is correct
	return *systemModule.moduleTerminals.insert(
			{terminalName,new terminal(terminalName, type, width, &systemModule, true)}).first->second;
}

terminal &schematicGenerator::getSystemTerminal(const std::string &terminalIdentifier) {
	return systemModule.getTerminal(terminalIdentifier);
}

module & schematicGenerator::addModule(const std::string &moduleName) {
	return *(subModules.insert(std::make_pair(moduleName,new module(moduleName))).first->second);
}


module & schematicGenerator::getModule(const std::string &moduleName) {
	return *subModules.find(moduleName)->second;
}


net & schematicGenerator::addNet(const std::string &netName, const int netWidth) {
	return *(internalNets.insert({netName,new net(netName,netWidth)}).first->second);
}

net & schematicGenerator::getNet(const std::string &netName) {
	return *(internalNets.find(netName)->second);
}








