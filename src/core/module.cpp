#include "module.h"

terminal& module::addTerminal(const std::string& terminalName, const terminalType type, const int width) {
	// FIXME: I need to check whether the returned reference is correct
	return *(
			moduleTerminals.insert({terminalName, new terminal(terminalName, type, width, this, false)}).first->second);
}

terminal& module::getTerminal(const std::string& terminalName) {
	return *moduleTerminals.find(terminalName)->second;
}

splicedTerminal* module::addSplicedTerminal(
		const terminal* const baseTerminal, const std::string& terminalName, net* const attachedNet) {
	return *(moduleSplicedTerminals.insert(new splicedTerminal(baseTerminal, terminalName, attachedNet)).first);
}

void module::rotateModule(clockwiseRotation newRotValue) {
	for (splicedTerminal* t : moduleSplicedTerminals) {
		intPair temp;
		switch (newRotValue) {
			case clockwiseRotation::d_0:
				t->placedPosition = t->originalPosition;
				break;
			case clockwiseRotation::d_90:
				temp = (t->originalPosition - size / 2);
				t->placedPosition = intPair{-temp.y, temp.x} + size / 2;
				break;
			case clockwiseRotation::d_180:
				t->placedPosition = size - t->originalPosition;
				break;
			case clockwiseRotation::d_270:
				temp = (t->originalPosition - size / 2);
				t->placedPosition = intPair{temp.y, -temp.x} + size / 2;
				break;
		}
	}
}

module::~module() {
	// FIXME: DO YOU DELETE THE ULINKS HERE?? for connectedModuleLinkMap;
	for (auto&& item : moduleTerminals) {
		delete item.second;
	}

	for (auto&& item : moduleSplicedTerminals) {
		delete item;
	}
}
