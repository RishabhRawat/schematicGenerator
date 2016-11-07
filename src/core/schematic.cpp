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

#ifdef WEB_COMPILATION
#include "emscripten.h"
#include "emscripten/bind.h"
#endif
#include <initializer_list>
#include "schematic.h"
#include "placement.h"
#include "routing.h"

schematic::~schematic() {
	for (auto&& item : internalNets) {
		delete item;
	}

	for (auto&& item : subModules) {
		delete item.second;
	}
}

void schematic::doPlacement(schematicParameters parameters) {
	designParameters = parameters;
	initializeStructures();
	placement placementObject;
	placementObject.place(this, designParameters);
	placementObject.flattenSchematic();
}

void schematic::doPlacement() {
	schematicParameters parameters;
	doPlacement(parameters);
}

void schematic::initializeStructures() {
	subModules.insert({systemModule.moduleIdentifier, &systemModule});
	// Iterates over all bitTerminal in the whole system
	for (namedModulePair& m_pair : subModules) {
		for (namedTerminalPair& t_pair : m_pair.second->moduleTerminals) {
			if (t_pair.second->type == terminalType::out)
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
			if (t->baseTerminal->type == terminalType::out) {
				output++;
			} else
				input++;
		}
		int maxout = output + 1;
		int maxin = input + 1;
		int placingLength = m.second->size.y - 2 * m.second->cornerTerminalPadding;
		int cornerPad = m.second->cornerTerminalPadding;
		for (splicedTerminal* t : m.second->moduleSplicedTerminals) {
			if (t->baseTerminal->type == terminalType::out)
				t->originalPosition = {m.second->size.x, cornerPad + (output-- * placingLength) / maxout};
			else
				t->originalPosition = {0, cornerPad + (input-- * placingLength) / maxin};
			t->placedPosition = t->originalPosition;
		}
	}
}

void schematic::doRouting() {
	routing routeObject(this);
	routeObject.route();
}

net::~net() {
	for (line* l : renderedLine) {
		delete l;
	}
}

std::string schematic::createDetailedJsonSchematicFromJson(std::string jsonData) {
	parseYosysJson(jsonData);
	return createDebugJsonSchematic();
}

std::string schematic::createJsonSchematicFromJson(std::string jsonData) {
	parseYosysJson(jsonData);
	doPlacement(designParameters);
	return createJsonSchematic();
}

terminal schematic::addSystemTerminal(const std::string& terminalName, const terminalType type,
		const unsigned int width) {
	return systemModule.addTerminal(terminalName, type, width, true);
}

terminal schematic::getSystemTerminal(const std::string& terminalIdentifier) {
	return systemModule.getTerminal(terminalIdentifier);
}

module* schematic::addModule(const std::string& moduleName) {
	return subModules.insert(std::make_pair(moduleName, new module(moduleName))).first->second;
}

module* schematic::getModule(const std::string& moduleName) {
	return subModules.find(moduleName)->second;
}

std::string schematic::getRoutedNetsJson() {
	return exportRoutingJson();
}

std::string schematic::getPlacedModulesJson() {
	return createJsonSchematic();
}

void schematic::setAspectRatio(unsigned short x, unsigned short y) {
	designParameters.aspectRatio = {x, y};
}

#ifdef WEB_COMPILATION
EMSCRIPTEN_BINDINGS(schematic) {
	emscripten::class_<schematic>("topDesign")
			.constructor<>()
			.function("createJsonSchematicFromJson", &schematic::createJsonSchematicFromJson)
			.function("createDetailedJsonSchematicFromJson", &schematic::createDetailedJsonSchematicFromJson)
			.function("addModule", &schematic::addModule)
			.function("getModule", &schematic::getModule)
			.function("addSystemTerminal", &schematic::addSystemTerminal)
			.function("getSystemTerminal", &schematic::getSystemTerminal)
			.function("doPlacement", &schematic::doPlacement)
			.function("getPlacedModulesJson", &schematic::getPlacedModulesJson)
			.function("doRouting", &schematic::doRouting)
			.function("getRoutedNetsJson", &schematic::getRoutedNetsJson)
			.function("setAspectRatio", &schematic::setAspectRatio);

	emscripten::class_<module>("module")
			.function("setSize", &module::setSize)
			.function("setPosition", &module::setPosition)
			.function("getWidth", &module::getWidth)
			.function("getHeight", &module::getHeight)
			.function("getPositionX", &module::getPositionX)
			.function("getPositionY", &module::getPositionY)
			.function("addTerminal", &module::addTerminal)
			.function("getTerminal", &module::getTerminal);

	emscripten::class_<terminal>("terminal")
			.constructor<std::string>()
			.function("partial", &terminal::partial)
			.function("connect", &terminal::connect)
			.function("getWidth", &terminal::getWidth);

	emscripten::enum_<terminalType>("terminalType")
			.value("in", terminalType::in)
			.value("out", terminalType::out)
			.value("inout", terminalType::inout);
}
#endif  // WEB_COMPILATION
