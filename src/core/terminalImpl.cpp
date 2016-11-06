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

#include "terminalImpl.h"
#include <functional>
#include "module.h"
//
// void terminalImpl::setRelativePosition(int x, int y){
//	terminalPosition = {x, y};
//	updateTerminalSide();
//}
// void terminalImpl::updateTerminalSide(){
//	if(isSystemTerminal()){
//		if(terminalPosition.x == 0 && 0 <=  terminalPosition.y &&  terminalPosition.y <=
// parentModule->getSize().y)
//			side = schematic::leftSide;
//		else if(terminalPosition.y == 0 && 0 <  terminalPosition.x &&  terminalPosition.x <
// parentModule->getSize().x)
//			side = schematic::bottomSide;
//		else if(terminalPosition.y == parentModule->getSize().y && 0 < terminalPosition.x && terminalPosition.x <
// parentModule->getSize().x)
//			side = schematic::topSide;
//		else if(terminalPosition.x == parentModule->getSize().x && 0 <= terminalPosition.y && terminalPosition.y <=
// parentModule->getSize().y)
//			side = schematic::rightSide;
//		else
//			throw "INVALID TERMINAL POSITION: terminals can only be placed at module edges";
//	}
//	else
//		side = schematic::noneSide;
//}
//

terminalImpl::terminalImpl(const std::string& terminalIdentifier, const terminalType type,
		const unsigned int terminalWidth, module* const parentModule, const bool systemTerminal)
	: terminalIdentifier(terminalIdentifier),
	  type(type),
	  terminalWidth(terminalWidth),
	  parentModule(parentModule),
	  bitTerminals(terminalWidth, bitTerminal{this, 0}),
	  systemTerminal(systemTerminal),
	  highestIndex(terminalWidth - 1),
	  lowestIndex(0) {
	for (unsigned int index = 0; index < terminalWidth; ++index)
		bitTerminals.at(index).index = index;
}

terminalImpl::~terminalImpl() {}

void terminalImpl::joinbitTerminals(bitTerminal& a, bitTerminal& b) {
	if (a.isConst && b.isConst) {
		if (a.brokenValue != b.brokenValue)
			throw std::invalid_argument("Trying to connect different constant terminals");
	} else if (a.isConst) {
		setBitTerminalToConst(b, a.brokenValue);
	} else if (b.isConst) {
		setBitTerminalToConst(a, b.brokenValue);
	} else {
		if (!a.connectedBitNet && !b.connectedBitNet) {
			std::shared_ptr<bitNet> newBitNet(std::make_shared<bitNet>());
			newBitNet->connectedBitTerminals.insert(&a);
			newBitNet->connectedBitTerminals.insert(&b);
			a.connectedBitNet = newBitNet;
			b.connectedBitNet = newBitNet;
		} else if (!a.connectedBitNet) {
			b.connectedBitNet->connectedBitTerminals.insert(&a);
			a.connectedBitNet = b.connectedBitNet;
		} else if (!b.connectedBitNet) {
			a.connectedBitNet->connectedBitTerminals.insert(&b);
			b.connectedBitNet = a.connectedBitNet;
		} else {
			a.connectedBitNet->connectedBitTerminals.insert(&b);
			for (bitTerminal* cBT : b.connectedBitNet->connectedBitTerminals) {
				cBT->connectedBitNet = a.connectedBitNet;
			}
			b.connectedBitNet = a.connectedBitNet;
		}
	}
}

void terminalImpl::setBitTerminalToConst(bitTerminal& t, char value) {
	t.isConst = true;
	t.brokenValue = value;
	if (t.connectedBitNet) {
		std::unordered_set<bitTerminal*>& cBT = t.connectedBitNet->connectedBitTerminals;
		while (!cBT.empty()) {
			if (*cBT.begin() != &t)
				setBitTerminalToConst(**cBT.begin(), value);
			cBT.erase(cBT.begin());
		}
	}
	t.connectedBitNet = nullptr;
}

void terminalImpl::splice(unsigned int highIndex, unsigned int lowIndex) {
	std::string constValue = "";
	if (bitTerminals.at(lowIndex).isConst) {
		for (unsigned int i = lowIndex; i <= highIndex; ++i) {
			constValue = bitTerminals.at(i).brokenValue + constValue;
		}
	}

	auto t = *parentModule->moduleSplicedTerminals
					  .insert(new splicedTerminal{this, terminalIdentifier + "[" + std::to_string(highIndex) + ":" +
																std::to_string(lowIndex) + "]",
							  constValue, &bitTerminals.at(lowIndex)})
					  .first;
	for (unsigned int i = lowIndex; i <= highIndex; ++i) {
		bitTerminals.at(i).actualTerminal = t;
	}
}

void terminalImpl::createSplicedTerminals() {
	unsigned int lI = 0;
	unsigned int hI = 0;
	bool joinable;
	for (unsigned int index = 0; index < highestIndex - lowestIndex; ++index) {
		bitTerminal& t0 = bitTerminals.at(lowestIndex + index);
		bitTerminal& t1 = bitTerminals.at(lowestIndex + index + 1);
		if (t0.isConst && t1.isConst)
			joinable = true;
		else if (t0.isConst || t1.isConst)
			joinable = false;
		else {
			joinable = true;
			std::unordered_set<bitTerminal*> lowerBitTerminal;
			for (auto&& cBT : t1.connectedBitNet->connectedBitTerminals) {
				int lesserIndex = cBT->index;
				lesserIndex--;
				if (lesserIndex < 0) {
					joinable = false;
					break;
				} else
					lowerBitTerminal.insert(&cBT->baseTerminal->bitTerminals.at(lesserIndex));
			}
			joinable &= (t0.connectedBitNet->connectedBitTerminals == lowerBitTerminal);
		}

		if (joinable)
			hI++;
		else {
			splice(lowestIndex + hI, lowestIndex + lI);
			lI = ++hI;
		}
	}
	splice(lowestIndex + hI, lowestIndex + lI);
}

void net::connectSplicedTerminal(splicedTerminal* t) {
	t->attachedNet = this;
	connectedModuleSplicedTerminalMap[t->baseTerminal->parentModule].push_back(t);
}

intPair net::addLineSegment(line* l, intPair p0, intPair p1) {
	intPair corner = {0, 0};
	bool horizontal = (p0.x == p1.x);
	if (l->empty()) {
		l->push_back(p0);
		l->push_back(p1);
	} else {
		bool lBeginHorizontal = (l->at(0).x == l->at(1).x);
		bool lEndHorizontal = (l->at(l->size() - 1).x == l->at(l->size() - 2).x);
		if ((l->back()) == p0) {
			if (horizontal == lEndHorizontal)
				l->back() = p1;
			else {
				l->push_back(p1);
				corner = p0;
			}
		} else if ((l->back()) == p1) {
			if (horizontal == lEndHorizontal)
				l->back() = p0;
			else {
				l->push_back(p0);
				corner = p1;
			}
		} else if ((l->front()) == p0) {
			if (horizontal == lBeginHorizontal)
				l->front() = p1;
			else {
				l->push_front(p1);
				corner = p0;
			}
		} else if ((l->front()) == p1) {
			if (horizontal == lBeginHorizontal)
				l->front() = p0;
			else {
				l->push_front(p0);
				corner = p1;
			}
		} else
			throw std::runtime_error("Incorrent point being added");
	}
	return corner;
}
