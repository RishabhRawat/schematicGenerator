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

#include "moduleImpl.h"

terminalImpl* moduleImpl::addTerminal(
		const std::string& terminalName, const termType type, const int width, const bool isSystemTerminal) {
	// FIXME: I need to check whether the returned reference is correct
	return moduleTerminals.insert({terminalName, new terminalImpl(terminalName, type, width, this, isSystemTerminal)})
			.first->second;
}

terminalImpl* moduleImpl::getTerminal(const std::string& terminalName) {
	return moduleTerminals.find(terminalName)->second;
}

void moduleImpl::rotateModule(clockwiseRotation newRotValue) {
	for (splicedTerminal* t : moduleSplicedTerminals) {
		intPair temp;
		switch (newRotValue) {
			case clockwiseRotation::d_0:
				t->placedPosition = t->originalPosition;
				t->placedSide = t->baseTerminal->side;
				break;
			case clockwiseRotation::d_90:
				temp = (t->originalPosition - size / 2);
				t->placedPosition = intPair{-temp.y, temp.x} + size / 2;
				t->placedSide = static_cast<terminalSide>((t->baseTerminal->side + 1) % 4);
				break;
			case clockwiseRotation::d_180:
				t->placedPosition = size - t->originalPosition;
			t->placedSide = static_cast<terminalSide>((t->baseTerminal->side + 2) % 4);
				break;
			case clockwiseRotation::d_270:
				temp = (t->originalPosition - size / 2);
				t->placedPosition = intPair{temp.y, -temp.x} + size / 2;
				t->placedSide = static_cast<terminalSide>((t->baseTerminal->side + 3) % 4);
				break;
		}
	}
}

moduleImpl::~moduleImpl() {
	for (auto&& item : moduleTerminals) {
		delete item.second;
	}

	for (auto&& item : moduleSplicedTerminals) {
		delete item;
	}

	for (auto&& linkMap : connectedModuleLinkMap) {
		for (auto&& item : linkMap.second) {
			delete item;
		}
	}
}
