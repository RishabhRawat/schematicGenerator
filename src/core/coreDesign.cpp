/*
 * schematiK - An automagic schematic generation tool
 *
 * Copyright (c) 2016, Rishabh Rawat <rishabhrawat1994@gmail.com>
 * Permission to use, copy, modify, and/or distribute this software for any purpose with or without
 * fee is hereby granted, provided that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 */

#include "coreDesign.h"
#include <strings.h>
#include <iostream>
#include <iterator>
#include <sstream>
#include "placement.h"
#include "routing.h"

coreDesign::~coreDesign() {
	for (auto&& item : internalNets) {
		delete item;
	}

	for (auto&& item : subModules) {
		delete item.second;
	}
}

void coreDesign::doPlacement(schematicParameters parameters) {
	designParameters = parameters;
	initializeStructures();
	placement placementObject;
	placementObject.place(this, designParameters);
	placementObject.flattenSchematic();
}

void coreDesign::initializeStructures() {
	subModules.insert({systemModule.moduleIdentifier, &systemModule});
	// Iterates over all bitTerminal in the whole system
	for (namedModulePair& m_pair : subModules) {
		for (namedTerminalPair& t_pair : m_pair.second->moduleTerminals) {
			if (t_pair.second->type == termType::outType)
				t_pair.second->side = terminalSide::rightSide;
			else
				t_pair.second->side = terminalSide::leftSide;
			t_pair.second->createSplicedTerminals();
		}
	}

	std::unordered_set<splicedTerminal*> sTSet;
	for (namedModulePair& m_pair : subModules) {
		for (splicedTerminal* t : m_pair.second->moduleSplicedTerminals) {
			sTSet.insert(t);
		}
	}

	while (!sTSet.empty()) {
		splicedTerminal* t = *sTSet.begin();
		if (!t->lowBitTerminal->isConst) {
			net* n = new net();
			n->connectSplicedTerminal(t);
			internalNets.insert(n);
			std::unordered_set<bitTerminal*>& tSet = t->lowBitTerminal->connectedBitNet->connectedBitTerminals;
			for (bitTerminal* connBT : tSet) {
				n->connectSplicedTerminal(connBT->actualTerminal);
				sTSet.erase(connBT->actualTerminal);
			}
		} else
			sTSet.erase(t);
	}

	for (namedModulePair& m : subModules) {
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (!t->lowBitTerminal->isConst)
				for (moduleSplicedTerminalPair& mST_pair : t->attachedNet->connectedModuleSplicedTerminalMap) {
					if (mST_pair.first != t->baseTerminal->parentModule)
						m.second->connectedModuleLinkMap[mST_pair.first].push_back(
								new ulink(t->attachedNet, t, &mST_pair.second));
				}
		}
	}
	subModules.erase(systemModule.moduleIdentifier);

	// Terminal Positioning
	for (namedModulePair m : subModules) {
		int input = 0;  // count inout on left side :/
		int output = 0;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->baseTerminal->type == termType::outType) {
				output++;
			} else
				input++;
		}
		int maxout = output + 1;
		int maxin = input + 1;
		int placingLength = m.second->size.y - 2 * m.second->cornerTerminalPadding;
		int cornerPad = m.second->cornerTerminalPadding;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->baseTerminal->type == termType::outType)
				t->originalPosition = {m.second->size.x, cornerPad + (output-- * placingLength) / maxout};
			else
				t->originalPosition = {0, cornerPad + (input-- * placingLength) / maxin};
			t->placedPosition = t->originalPosition;
		}
	}
}

std::string coreDesign::doRouting() {
	routing routeObject(this);
	routeObject.route();
	return exportRoutingJson();
}

net::~net() {
	for (line* l : renderedLine) {
		delete l;
	}
}
