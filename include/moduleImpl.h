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

#ifndef MODULES_H
#define MODULES_H
#include "common.h"
#include "terminalImpl.h"

class moduleImpl {
	friend class coreDesign;
	friend class placement;
	friend class routing;
	friend class box;
	friend class module;
	friend class terminalImpl;
	friend class net;

private:
	std::string moduleType;
	const std::string moduleIdentifier;
	intPair position;
	int cornerTerminalPadding = 10;

	/* First along x-axis then along y-axis*/
	intPair size = {70, 120};  // Default size

	mutable clockwiseRotation moduleRotation = clockwiseRotation::d_0;

	namedTerminalCollection moduleTerminals;

	splicedTerminalSet moduleSplicedTerminals;

	moduleLinkMap connectedModuleLinkMap;

	mutable box* parentBox = nullptr;

	void setParentBox(box* b) const {
		parentBox = b;
	}

	void rotateModule(clockwiseRotation newRotValue);

public:
	~moduleImpl();

	moduleImpl(const std::string moduleIdentifier) : moduleIdentifier(moduleIdentifier) {}

	std::string getIdentifier() const {
		return moduleIdentifier;
	}
	terminalImpl* addTerminal(
			const std::string& terminalName, const termType type, const int width, const bool isSystemTerminal);

	terminalImpl* getTerminal(const std::string& basic_string);

	// Clockwise from bottom left
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
#endif  // MODULES_H
