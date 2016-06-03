#include "module.h"

terminal & module::addTerminal(const std::string &terminalName, const schematic::terminalType type, const int width) {
	//FIXME: I need to check whether the returned reference is correct
	return *(moduleTerminals.insert({terminalName, new terminal(terminalName, type, width, this, false)}).first->second);
}

terminal & module::getTerminal(const std::string &terminalName) {
	return *moduleTerminals.find(terminalName)->second;
}

splicedTerminal * module::addSplicedTerminal(const terminal *const baseTerminal, const std::string &terminalName,
                                             net *const attachedNet) {
	return *(moduleSplicedTerminals.insert(new splicedTerminal(baseTerminal, terminalName, attachedNet)).first);
}

void module::rotateModule(schematic::clockwiseRotation newRotValue) {
	for (splicedTerminal *t: moduleSplicedTerminals) {
		intPair temp;
		switch (newRotValue) {
			case schematic::d_0:
				t->placedPosition = t->originalPosition;
				break;
			case schematic::d_90:
				 temp = (t->placedPosition - moduleSize/2);
				t->placedPosition = intPair{-temp.y, temp.x} + moduleSize/2;
				break;
			case schematic::d_180:
				t->placedPosition =  moduleSize - t->placedPosition;
				break;
			case schematic::d_270:
				temp = (t->placedPosition - moduleSize/2);
				t->placedPosition = intPair{temp.y, -temp.x} + moduleSize/2;
				break;
		}
	}
}
