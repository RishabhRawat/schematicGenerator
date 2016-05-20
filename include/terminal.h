#ifndef TERMINAL_H
#define TERMINAL_H
#include <common.h>

class terminal
{
	friend class placement;
private:
	std::string terminalIdentifier;
	schematic::terminalType type;
	schematic::terminalSide side;
	intPair terminalPosition;
	int terminalWidth;
	module * parentModule;
	bool systemTerminal;
	net * attachedNet;

	void updateTerminalSide();
	module * getParent() const { return parentModule; }
	net * getNet() const { return attachedNet; }

public:
	terminal(std::string name, module *parentPointer):terminalIdentifier(name),parentModule(parentPointer) {
	}

	/* sets position relative to the module which owns the terminal,
	 * relative to bottom left corner, must be set after the module size is set
	 * has no meaning for systemTerminals
	 */
	void setRelativePosition(int x, int y);
	bool isSystemTerminal() const { return systemTerminal; }


};

#endif // TERMINAL_H
