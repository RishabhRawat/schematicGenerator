#include "terminal.h"
#include "module.h"

void terminal::setRelativePosition(int x, int y){
	terminalPosition.x=x;
	terminalPosition.y=y;
	updateTerminalSide();
}
void terminal::updateTerminalSide(){
	if(isSystemTerminal()){
		if(terminalPosition.x == 0 && 0 <=  terminalPosition.y &&  terminalPosition.y <= parentModule->getModuleSize().y)
			side = schematic::left;
		else if(terminalPosition.y == 0 && 0 <  terminalPosition.x &&  terminalPosition.x < parentModule->getModuleSize().x)
			side = schematic::bottom;
		else if(terminalPosition.y == parentModule->getModuleSize().y && 0 < terminalPosition.x && terminalPosition.x < parentModule->getModuleSize().x)
			side = schematic::top;
		else if(terminalPosition.x == parentModule->getModuleSize().x && 0 <= terminalPosition.y && terminalPosition.y <= parentModule->getModuleSize().y)
			side = schematic::right;
		else
			throw "INVALID TERMINAL POSITION: terminals can only be placed at module edges";
	}
	else
		side = schematic::none;
}

