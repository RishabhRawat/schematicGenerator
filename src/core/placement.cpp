#include "placement.h"
#include "partition.h"
#include "box.h"
#include <iostream>

inline int countNets(netCollection nC) {
	int connections = 0;
	//If you want give weigtaget to net width
	//    for(net* n:nC) {
	//        connections+=n->getNetWidth();
	//    }

	//If you just want to count nets
	connections = nC.size();
	return connections;
}
//Same with this function
inline int countNets(linkCollection lC) {
	int connections = 0;
	connections = lC.size();
	return connections;
}


placement::~placement() {
	for(partition *p: allPartitions) {
		for(box *b: p->partitionBoxes){
			delete b;
		}
		delete p;
	}
}

void placement::doPlacement() {
	initializeStructures();
	partitionFormation();
	boxFormation();
	modulePlacement();
	boxPlacement();
	partitionPlacement();
	terminalPlacement();
}

void placement::initializeStructures() {
	//Setting up net connectivity
	for(namedNetPair& n: internalNets) {
		for(terminal * t: n.second.getConnectedTerminals()) {

			moduleTerminalMap::iterator mapIterator = n.second.connectedModuleTerminalMap.find(t->getParent());
			if(mapIterator == n.second.connectedModuleTerminalMap.end()) {
				terminalCollection newCollection;
				newCollection.push_back(t);
				//NOTE: I presume that in insert we have a copy, so even when the lifetime
				// of newCollection ends we wont have a problem
				// need a better solution to this maybe
				n.second.connectedModuleTerminalMap.insert({t->getParent(),newCollection});
			}
			else {
				mapIterator->second.push_back(t);
			}
		}
	}

	//Setting up module connectivity
	for(namedModulePair& m: subModules) {
		for(namedTerminalPair &t: m.second.moduleTerminals) {
			for(moduleTerminalPair& pair: (t.second.getNet())->connectedModuleTerminalMap) {
				if(pair.first != &m.second){
					moduleLinkMap::iterator mapIterator = m.second.connectedModuleLinkMap.find(pair.first);
					if(mapIterator == m.second.connectedModuleLinkMap.end()) {
						linkCollection *newCollection = new linkCollection();
						newCollection->push_back(new ulink(t.second.getNet(),&t.second,&pair.second));
						m.second.connectedModuleLinkMap.insert({pair.first,*newCollection});
					}
					else {
						mapIterator->second.push_back(new ulink(t.second.getNet(),&t.second,&pair.second));
					}
				}
			}
		}
	}
}

void placement::partitionFormation() {
	module * seed;
	hashlib::pool<module*> moduleSet;

	for(namedModulePair& m: subModules)
		moduleSet.insert(&(m.second));

	while(!moduleSet.empty()){
		seed = selectSeed(moduleSet);
		moduleSet.erase(seed);
		allPartitions.emplace_back(createPartition(moduleSet,seed));
	}
}

module* placement::selectSeed(hashlib::pool<module*> moduleSet) const{
	int maxConnectionsInFreeSet = -1;
	int minConnectionsOutFreeSet = INT32_MAX;
	module * seed;
	for(module* m: moduleSet) {
		int connectionsInFreeSet = 0;
		int connectionsOutFreeSet = 0;
		// The choice is either to iterate over all modules connected to m
		// or to iterate over all elements in moduleSet and find those connected to m
		for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
			if(moduleSet.find(pair.first) != moduleSet.end())
				connectionsInFreeSet += countNets(pair.second);
			else
				connectionsOutFreeSet += countNets(pair.second);
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
		throw "Bad moduleSet in placement::selectSeed";
	return seed;
}

partition * placement::createPartition(hashlib::pool<module*>& moduleSet, module * seed) {
	box * rootBox = new box();
	rootBox->add(seed);
	partition * newPartition = new partition();
	newPartition->add(rootBox);
	unsigned int connections = 0;
	unsigned int partitionEntries = 0;
	while(!moduleSet.empty() && (partitionEntries < maxPartitionSize)
		  && (connections < maxPartitionConnections)) {

		module * selectedModule;
		int maxConnectionsInPartition = -1;
		int minConnectionsOutPartion = INT32_MAX;

		//Algo to select module
		for(module* m: moduleSet) {
			int connectionsInPartition = 0;
			int connectionsOutPartition = 0;
			for(moduleLinkPair& pair: m->connectedModuleLinkMap) {
				if(pair.first->parentBox == rootBox)
					connectionsInPartition += countNets(pair.second);
				else
					connectionsOutPartition += countNets(pair.second);
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
			throw "Bad selectedModule in placement::createPartition";


		moduleSet.erase(selectedModule);
		rootBox->add(selectedModule);
		connections -= maxConnectionsInPartition;
		connections +=minConnectionsOutPartion;
	}
	return newPartition;
}

moduleCollection placement::selectRoots(partition *p) {
	if(p->partitionBoxes.size()>1) throw "Invalid Partition Error: placment::selectRoots";

	box * b = p->partitionBoxes.back();
	moduleCollection roots;
	for(module *m:b->boxModules) {
		bool seed = false;
		for(moduleLinkPair pair: m->connectedModuleLinkMap) {
			if(b->boxModules.find(pair.first) == b->boxModules.end() && pair.first != &systemModule){
				seed = true;
			}
			else if(pair.first == &systemModule) {
				for(ulink *l: pair.second){
					for(terminal* t: *(l->linkSink)) {
						if (t->type == schematic::in || t->type == schematic::inout){
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
			if(t.second.type == schematic::out)
				outGoingNets++;
			if(outGoingNets > 1)
				break;
		}
		if(seed)
			roots.push_back(m);
	}
	return roots;
}

box * placement::selectPath( box *path, box *remainingModules) {
	bool searchSuccess = true;

	while(searchSuccess && path->size()<=maxPathLength) {
		searchSuccess = false;
		//FIXME: module * lastModule = path->boxModules.back();
		module * lastModule = path->boxModules.back();
		for(module *m: remainingModules->boxModules) {
			moduleLinkMap::iterator mapIterator = lastModule->connectedModuleLinkMap.find(m);
			if(mapIterator != lastModule->connectedModuleLinkMap.end()) {
				for(ulink *l:mapIterator->second){
					if(l->linkSource->type == schematic::in || l->linkSource->type == schematic::inout ){
						for(terminal * t: *(l->linkSink)){
							if(t->type == schematic::out || t->type == schematic::inout){
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

void placement::boxFormation() {

	for(partition *p: allPartitions) {
		moduleCollection roots = selectRoots(p);
		box * elements = p->partitionBoxes.pop();
		while(!elements->empty()) {
			box * longestPath = nullptr;
			for(module * root:roots) {
				box * path = new box();
				elements->remove(root);
				//FIXME:This will be in two boxes at the same time?
				//ParentBox is not required in selectPath
				//so its value should be consistent at the end
				path->add(root);
				path = selectPath(path,elements);
				elements->add(root);
				if(longestPath->size() < path->size()) {
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

terminal & placement::addSystemTerminal(const std::string &terminalIdentifier, const schematic::terminalType type) {
	//FIXME: I need to check whether the returned reference is correct
	return systemModule.moduleTerminals.insert(std::make_pair(terminalIdentifier,terminal
			(terminalIdentifier,type,&systemModule,true))).first->second;
}


