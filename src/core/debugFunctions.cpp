#include <iostream>
#include "box.h"
#include "partition.h"
#include "schematicGenerator.h"

void schematicGenerator::printDerivedStructures() {
	std::cout << std::endl << "Printing All modules" << std::endl;
	for (namedModulePair nm : subModules) {
		std::cout << nm.first << " : " << std::endl;
		for (splicedTerminal* t : nm.second->moduleSplicedTerminals) {
			std::cout << "\t" << t->terminalIdentifier << " : {" << t->originalPosition.x << ","
					  << t->originalPosition.y << "}" << std::endl;
		}
	}
	std::cout << std::endl << "Printing All coalescedNet connections to modules and their splicedTerminal" << std::endl;
	for (coalescedNet* n : coalescedNetSet) {
		std::cout << n->sourceNet->netIdentifier << " : " << std::endl;
		for (moduleSplicedTerminalPair mt : n->connectedModuleSplicedTerminalMap) {
			std::cout << "\t" << mt.first->moduleIdentifier << std::endl;
			for (splicedTerminal* t : mt.second)
				std::cout << "\t\t" << t->terminalIdentifier << std::endl;
		}
	}

	std::cout << std::endl << "Printing All module-coalescedNet connectivities" << std::endl;
	for (namedModulePair nm : subModules) {
		std::cout << nm.first << " : " << std::endl;
		for (splicedTerminal* t : nm.second->moduleSplicedTerminals) {
			std::cout << "\t" << t->attachedNet->netIdentifier << std::endl;
		}
	}

	std::cout << std::endl
			  << "Printing All module connections to other modules and the coalesced nets connecting them" << std::endl;
	for (namedModulePair nm : subModules) {
		std::cout << nm.first << " : " << std::endl;
		for (moduleLinkPair ml : nm.second->connectedModuleLinkMap) {
			std::cout << "\t" << ml.first->moduleIdentifier << std::endl;
			for (ulink* ul : ml.second)
				std::cout << "\t\t" << ul->linkNet->sourceNet->netIdentifier << std::endl;
		}
	}
	std::cout << "Debug Print Derived Structures Finished" << std::endl;
}

void schematicGenerator::printInitialStructures() {
	std::cout << std::endl << "Printing All modules" << std::endl;
	for (namedModulePair nm : subModules) {
		std::cout << nm.first << " : " << std::endl;
		for (namedTerminalPair t : nm.second->moduleTerminals) {
			std::cout << "\t" << t.second->terminalIdentifier << ": " << t.second->terminalWidth << std::endl;
		}
	}
	std::cout << "Debug Print Initial Structures Finished" << std::endl;
}

void schematicGenerator::printPartitions() {
	std::cout << std::endl << "Printing all partitions" << std::endl;
	int i = 0;
	for (partition* p : allPartitions) {
		std::cout << i++ << " : " << std::endl;
		for (auto&& m : p->partitionModules) {
			std::cout << "\t" << m->moduleIdentifier << std::endl;
		}
	}
	std::cout << "Debug Print Partitions Finished" << std::endl;
}