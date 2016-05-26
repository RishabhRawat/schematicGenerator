#include <net.h>
#include "terminal.h"
//
//void terminal::setRelativePosition(int x, int y){
//	terminalPosition = {x, y};
//	updateTerminalSide();
//}
//void terminal::updateTerminalSide(){
//	if(isSystemTerminal()){
//		if(terminalPosition.x == 0 && 0 <=  terminalPosition.y &&  terminalPosition.y <= parentModule->getModuleSize().y)
//			side = schematic::leftSide;
//		else if(terminalPosition.y == 0 && 0 <  terminalPosition.x &&  terminalPosition.x < parentModule->getModuleSize().x)
//			side = schematic::bottomSide;
//		else if(terminalPosition.y == parentModule->getModuleSize().y && 0 < terminalPosition.x && terminalPosition.x < parentModule->getModuleSize().x)
//			side = schematic::topSide;
//		else if(terminalPosition.x == parentModule->getModuleSize().x && 0 <= terminalPosition.y && terminalPosition.y <= parentModule->getModuleSize().y)
//			side = schematic::rightSide;
//		else
//			throw "INVALID TERMINAL POSITION: terminals can only be placed at module edges";
//	}
//	else
//		side = schematic::noneSide;
//}
//


terminal::terminal(const std::string &terminalIdentifier, const schematic::terminalType type, const int terminalWidth,
                   module *const parentModule, const bool systemTerminal) :
		terminalIdentifier(terminalIdentifier), type(type), terminalWidth(terminalWidth), parentModule(parentModule),
		systemTerminal(systemTerminal), highIndex(terminalWidth - 1), lowIndex(0), highToLow(true) {

	if(highIndex<lowIndex)
		throw std::invalid_argument("Invalid Index values to terminal constructor");

	internalBitTerminals = new bitTerminal*[terminalWidth];
	for(int i = 0; i < terminalWidth; ++i) {
		internalBitTerminals[i] = new bitTerminal(this,i);
	}
}

terminal::terminal(const terminal &baseTerminal, const int highIndex, const int lowIndex, const bool highToLow)
		: terminalIdentifier(
		baseTerminal.terminalIdentifier + "[" + std::to_string(highIndex) + ":" + std::to_string(lowIndex) + "]"),
		  type(baseTerminal.type), terminalWidth(highIndex-lowIndex+1), parentModule(baseTerminal.parentModule),
		  systemTerminal(baseTerminal.systemTerminal), highIndex(highIndex), lowIndex(lowIndex), highToLow(highToLow) {

	if(highIndex<=lowIndex)
		throw std::invalid_argument("Invalid Index values to terminal constructor");

	internalBitTerminals = new bitTerminal*[terminalWidth];
	for(int i = 0; i < terminalWidth; ++i) {
		internalBitTerminals[i] = baseTerminal.internalBitTerminals[lowIndex+i];
	}
}

terminal::~terminal() {
	if(internalBitTerminals[lowIndex]->baseTerminal == this) {
		for (int i = 0; i < terminalWidth; ++i) {
			delete internalBitTerminals[i];
		}
	}
	delete[] internalBitTerminals;

}

terminal terminal::partialTerminal(int index1, int index2) {
	if(index1 < lowIndex || index2 < lowIndex || index1 > highIndex || index2 > highIndex)
		throw std::invalid_argument("Index value out of range");
	return terminal(*this,index1,index2,index1>index2);
}

coalescedNet * splicedTerminal::getCoalescedNet(){
	return attachedNet->coalesced;
}

