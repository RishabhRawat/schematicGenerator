#ifndef TERMINAL_H
#define TERMINAL_H
#include <common.h>

class terminal
{
	friend class placement;
private:
	const std::string terminalIdentifier;
	const schematic::terminalType type;
	schematic::terminalSide side;
	intPair terminalPosition;
	int terminalWidth;
	module * const parentModule;
	const bool systemTerminal;
	net * attachedNet;

	void updateTerminalSide();

	module *getParent() const { return parentModule; }

private:
	net * getNet() const { return attachedNet; }

public:

	terminal(const std::string &terminalIdentifier, const schematic::terminalType type, module *const parentModule,
	         const bool systemTerminal) : terminalIdentifier(terminalIdentifier), type(type),
	                                      parentModule(parentModule), systemTerminal(systemTerminal) { }


	terminal &operator= (terminal) {
		//FIXME: If this operator just throws stuff why doesnt the program compile without it!!
		// This function will most probably never be called!!
		throw std::runtime_error("should not copy assign terminal class");
		return *this;
	}

/* sets position relative to the module which owns the terminal,
	 * relative to bottom left corner, must be set after the module size is set
	 * has no meaning for systemTerminals
	 */
	void setRelativePosition(int x, int y);

	bool isSystemTerminal() const { return systemTerminal; }

	const std::string &getTerminalIdentifier() const { return terminalIdentifier; }


};

#endif // TERMINAL_H
