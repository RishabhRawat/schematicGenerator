#ifndef MODULES_H
#define MODULES_H
#include "common.h"
#include "terminal.h" //for terminal vector


class module
{
    friend class schematicGenerator;

private:

	std::string moduleType;
    const std::string moduleIdentifier;
    intPair modulePosition;

    /* First along x-axis then along y-axis*/
    intPair moduleSize = {50,100}; //Default size

	namedTerminalCollection moduleTerminals;

	splicedTerminalSet moduleSplicedTerminals;
    
    moduleLinkMap connectedModuleLinkMap;

    box* parentBox;

	splicedTerminal * addSplicedTerminal(const terminal *const baseTerminal, const std::string &terminalName,
	                                     net *const attachedNet);


public:


	module(const std::string &moduleIdentifier) : moduleIdentifier(moduleIdentifier) { }

	std::string getIdentifier() const {
		return moduleIdentifier;
	}
    intPair getModuleSize() const {
	    return moduleSize;
    }

    void setParentBox(box *b) {
	    parentBox = b;
    }

    terminal &addTerminal(const std::string &terminalName, const schematic::terminalType type, const int width);

	terminal & getTerminal(const std::string &basic_string);


};
#endif // MODULES_H
