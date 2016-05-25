#include "module.h"

terminal & module::addTerminal(const std::string &terminalName, const schematic::terminalType type) {
	//FIXME: I need to check whether the returned reference is correct
	return moduleTerminals.insert(std::make_pair(terminalName,terminal(terminalName,type,this,false))).first->second;
}

terminal & module::getTerminal(const std::string &terminalName) {
	return moduleTerminals.find(terminalName)->second;
}
