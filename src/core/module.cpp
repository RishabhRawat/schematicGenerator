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
