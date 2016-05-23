#include "module.h"

terminal &module::addTerminal(const std::string &terminalIdentifier, const schematic::terminalType type) {
	//FIXME: I need to check whether the returned reference is correct
	return moduleTerminals.insert(std::make_pair(terminalIdentifier,terminal(terminalIdentifier,type,this,false))).first->second;
}


