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

#ifndef COREDESIGN_H
#define COREDESIGN_H

#include "json.hpp"
#include "module.h"

class schematic {
	friend class placement;
	friend class routing;

	module systemModule;  // just a place holder
	namedModuleCollection subModules;

	schematicParameters designParameters;

	intPair size, offset;
	netSet internalNets;



	void initializeStructures();

	void doPlacement(schematicParameters parameters);

	std::string createDebugJsonSchematic();
	std::string createJsonSchematic();

	std::string exportRoutingJson();

public:
	schematic() : systemModule("topModule") {}
	~schematic();


	/**
	 * Adds a module to the design
	 * @param moduleName unique name for the module
	 * @return
	 */
	module* addModule(const std::string& moduleName);

	/**
	 * Retrieve a module with corrosponding name
	 * @param moduleName
	 * @return
	 */
	module* getModule(const std::string& moduleName);
	/**
	 * Adds a top level terminal
	 * @param terminalName
	 * @param type
	 * @param width
	 * @return
	 */
	terminal addSystemTerminal(const std::string& terminalName, const terminalType type, const unsigned int width);

	/**
	 * Retrieve a toplevel terminal with corrosponding name
	 * @param terminalIdentifier
	 * @return
	 */
	terminal getSystemTerminal(const std::string& terminalIdentifier);
	/**
	 * Run Placement Algorithm
	 */
	 //TODO: what about creating json schematic
	void doPlacement();

	/**
	 * Run Routing Algorithm
	 */
	void doRouting();

	void parseJsonFile(std::string fileName);

	void parseYosysJson(std::string jsonText);
	/**
	 * Returns the placement results as a json string
	 * @return
	 */
	std::string getPlacedModulesJson();

	/**
	 * Returns the routing results as a json string
	 * @return
	 */
	std::string getRoutedNetsJson();
	/**
	 * Deprecated
	 * @param jsonData
	 * @return
	 */
	std::string createDetailedJsonSchematicFromJson(std::string jsonData);
	/**
	 * Deprecated
	 * @param jsonData
	 * @return
	 */
	std::string createJsonSchematicFromJson(std::string jsonData);

	/**
	 * Sets the aspect ratio of the generated schematic. It influences the cost function of placement, and may not result in exactly same ratio, instead it is more like a suggestion to the algorithm.
	 * @param x
	 * @param y
	 */
	void setAspectRatio(unsigned short x, unsigned short y);
};


#endif  // PLACEMENT_H
