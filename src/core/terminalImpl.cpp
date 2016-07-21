#include "terminalImpl.h"
#include "moduleImpl.h"
#include <functional>
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
//			throw "INVALID TERMINAL POSITION: terminals can only be placed at moduleImpl edges";
//	}
//	else
//		side = schematic::noneSide;
//}
//

terminalImpl::terminalImpl(const std::string& terminalIdentifier, const termType type, const unsigned int terminalWidth,
		moduleImpl* const parentModule, const bool systemTerminal)
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

terminalImpl::~terminalImpl() {

}

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
		else
			joinable = (t0.connectedBitNet->connectedBitTerminals == t1.connectedBitNet->connectedBitTerminals &&
						t0.index + 1 == t1.index);

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
