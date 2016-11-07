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

#ifndef TERMINAL_H
#define TERMINAL_H

#include <memory>
#include <unordered_set>
#include "common.h"

struct bitTerminal {
	std::shared_ptr<bitNet> connectedBitNet = nullptr;
	terminalImpl* baseTerminal;
	unsigned int index;
	bool isConst = false;
	char brokenValue = 0;
	splicedTerminal* actualTerminal = nullptr;

	bitTerminal(terminalImpl* baseTerminal, unsigned int index) : baseTerminal(baseTerminal), index(index) {}
};


/**
 * @class terminal
 * public side terminal class, used to create and access modules
 */
class terminal {
	friend class schematic;
	friend class module;
	terminalImpl* terminalPointer;
	unsigned int highestIndex, lowestIndex;

private:
	std::string constantValue;
	terminal(terminalImpl* terminalPointer, const unsigned int highestIndex, const unsigned int lowestIndex)
			: terminalPointer(terminalPointer), highestIndex(highestIndex), lowestIndex(lowestIndex), constantValue("") {}

public:
	/**
	 * Constructor for constructing constant terminals, non-constant terminals need module::addTerminals or
	 * schematic::addSystemTerminals
	 * @param constValue
	 * @return
	 */
	terminal(std::string constValue)
			: terminalPointer(nullptr),
			  highestIndex(static_cast<unsigned int>(constValue.length() - 1)),
			  lowestIndex(0),
			  constantValue(constValue) {}
	/**
	 * Returns a partial terminal, useful for splicing terminals
	 * @param highIndex
	 * @param lowIndex
	 * @return
	 */
	terminal partial(unsigned int highIndex, unsigned int lowIndex);
	/**
	 * Connects two terminals provided they are equal in width, If not use terminal::partial to splice them first
	 * @param t
	 * @return
	 */
	terminal& connect(terminal t);

	unsigned int getWidth() const {
		return highestIndex - lowestIndex + 1;
	}
};

/**
 * @class terminal
 * Represents terminals of modules (connectivity points)
 */
class terminalImpl {
	friend class splicedTerminal;
	friend class schematic;
	friend class placement;
	friend class module;
	friend class module;
	friend class terminal;
	friend class schematic;
	friend class net;

	const std::string terminalIdentifier;
	const terminalType type;
	const int terminalWidth;
	module* const parentModule;
	std::vector<bitTerminal> bitTerminals;

	const bool systemTerminal;
	const unsigned int highestIndex;
	const unsigned int lowestIndex;

	terminalImpl(const std::string& terminalIdentifier, const terminalType type, const unsigned int terminalWidth,
			module* const parentModule, const bool systemTerminal);
	~terminalImpl();
	static void setBitTerminalToConst(bitTerminal& t, char v);
	static void joinbitTerminals(bitTerminal& a, bitTerminal& b);

	void splice(unsigned int highIndex, unsigned int lowIndex);
	void createSplicedTerminals();

	terminalSide side;

	// TODO: Need to add terminal positioning functionality
	intPair terminalPositionHint;
};

class splicedTerminal {
	friend class schematic;
	friend class placement;
	friend class routing;
	friend class module;
	friend class terminalImpl;
	friend class net;

	const terminalImpl* const baseTerminal;
	const std::string terminalIdentifier;
	const std::string constTValue;
	bitTerminal* lowBitTerminal;
	net* attachedNet = nullptr;

	intPair originalPosition;
	intPair placedPosition;
	terminalSide placedSide;

	splicedTerminal(const terminalImpl* const baseTerminal, const std::string& terminalIdentifier,
			const std::string& constTValue, bitTerminal* lowBitTerminal)
		: baseTerminal(baseTerminal),
		  terminalIdentifier(terminalIdentifier),
		  constTValue(constTValue),
		  lowBitTerminal(lowBitTerminal),
		  placedSide(baseTerminal->side) {}

	bool isSystemTerminal() const {
		return baseTerminal->systemTerminal;
	}
};

struct bitNet {
	std::unordered_set<bitTerminal*> connectedBitTerminals;
};

class net {
	friend class schematic;
	friend class placement;
	friend class routing;
	moduleSplicedTerminalMap connectedModuleSplicedTerminalMap;
	std::vector<line*> renderedLine;

	static intPair addLineSegment(line* l, intPair p0, intPair p1);

public:
	~net();
	void connectSplicedTerminal(splicedTerminal* t);
};

#endif  // TERMINAL_H
