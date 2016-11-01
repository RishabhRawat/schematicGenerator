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

#ifndef PLACEMENT_H
#define PLACEMENT_H

#include <vector>
#include "box.h"
#include "partition.h"

class placement {
	friend class coreDesign;

	schematicParameters designParameters;
	partitionVector allPartitions;
	coreDesign* core;
	~placement();

	template <typename T>
	struct positionalStructure {
		T* item;
		std::vector<const T*> side[4];  // clockwise from top
		void add(const T* const newItem) {
			if (item == newItem)
				return;
			if (item->position.x > newItem->position.x)
				side[3].push_back(newItem);
			if (item->position.y > newItem->position.y)
				side[2].push_back(newItem);
			if (item->position.x + item->size.x < newItem->position.x + newItem->size.x)
				side[1].push_back(newItem);
			if (item->position.x + item->size.y > newItem->position.x + newItem->size.y)
				side[0].push_back(newItem);
		}
		positionalStructure(T* firstItem) {
			item = firstItem;
		}
	};

	void place(coreDesign* inputDesign, schematicParameters& parameters);

	void partitionFormation();
	moduleImpl* selectPartitionSeed(hashlib::pool<moduleImpl*> moduleSet) const;
	partition* createPartition(hashlib::pool<moduleImpl*>& moduleSet, moduleImpl* seed);

	void boxFormation();
	moduleSet* selectBoxSeeds(partition* p);
	box* selectPath(box* rootBox, moduleSet remainingModules);

	void modulePlacement();
	void initModulePlacement(box* b, intPair& leftBottom, intPair& rightTop);
	void placeModule(box* b, unsigned int index, intPair& pair, intPair& intPair);
	int calculatePadding(unsigned int n);

	void boxPlacement();  // SHARING BOUNDARIES IS PERFECTLY LEGAL
	box* selectNextBox(const hashlib::pool<box*, hashlib::hash_ops<box*>>& remainingBoxes,
			const hashlib::pool<box*, hashlib::hash_ops<box*>>& placedBoxes);
	intPair calculateOptimumBoxPosition(const box* b, hashlib::pool<box*>& placedBoxes);
	template <typename T>
	intPair calculateActualPosition(
			const intPair size, const intPair optimumPosition, hashlib::dict<T*, positionalStructure<T>>& layoutData);
	void partitionPlacement();  // SHARING BOUNDARIES IS PERFECTLY LEGAL
	partition* selectNextPartition(
			hashlib::pool<partition*> remainingPartition, hashlib::pool<partition*> placedPartition);
	intPair calculateOptimumPartitionPosition(partition* p, hashlib::pool<partition*>& placedPartition);

	void systemTerminalPlacement();

	void flattenSchematic();

	unsigned int distanceCostFunction(const intPair a, const intPair b);
};

#endif  // SCHEMATICGENERATOR_PLACEMENT_H
