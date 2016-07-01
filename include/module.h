#ifndef MODULES_H
#define MODULES_H
#include "common.h"
#include "terminal.h" //for terminal vector


class module
{
    friend class coreDesign;
	friend class placement;
	friend class box;

private:

	std::string moduleType;
    const std::string moduleIdentifier;
    intPair position;

    /* First along x-axis then along y-axis*/
    intPair size = {50,100}; //Default size

	mutable clockwiseRotation moduleRotation = clockwiseRotation::d_0;

	namedTerminalCollection moduleTerminals;

	splicedTerminalSet moduleSplicedTerminals;
    
    moduleLinkMap connectedModuleLinkMap;

    mutable box* parentBox;

	splicedTerminal * addSplicedTerminal(const terminal *const baseTerminal, const std::string &terminalName,
	                                     net *const attachedNet);

	void setParentBox(box *b) const {
		parentBox = b;
	}

	void rotateModule(clockwiseRotation newRotValue);


public:
	virtual ~module();

	module(const std::string &moduleIdentifier) : moduleIdentifier(moduleIdentifier) { }

	std::string getIdentifier() const {
		return moduleIdentifier;
	}
    intPair getSize() const {
	    return size;
    }

    terminal &addTerminal(const std::string &terminalName, const terminalType type, const int width);

	terminal & getTerminal(const std::string &basic_string);

	// Clockwise from bottom left
	intPair getVertex(const int index) {
		switch (index) {
		case 0:
			return position;
		case 1:
			return position + size.component(1);
		case 2:
			return position + size;
		case 3:
			return position + size.component(0);
		default:
			throw std::invalid_argument("Invalid index");
		}
	};



};
#endif // MODULES_H
