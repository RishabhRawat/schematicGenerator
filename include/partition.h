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

#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
	friend class placement;
	friend class coreDesign;

private:
	std::vector<box*> partitionBoxes;
	moduleSet partitionModules;
	intPair offset, position, size;

	// used after box formation
	void addModule(module* m);
	bool contains(module* m);

public:
	void add(box* b);
	unsigned int length() {
		return static_cast<unsigned int>(partitionBoxes.size());
	}
	~partition();
	intPair getVertex(const int index) {
		switch (index) {
		case 0:
			return position;
		case 1:
			return position + size.component(1);
		case 2:
			return position + size;
		case 3:
			return position + size.component(0);
		default:
			throw std::invalid_argument("Invalid index");
		}
	};
};

#endif  // PARTITION_H
