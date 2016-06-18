#include <iostream>
#include "schematicGenerator.h"

int main(int argc, char ** argv){
//	if(argc < 2)
//		throw std::invalid_argument("test file not specified");
    schematicGenerator myPlacementObject;
	myPlacementObject.parseJsonFile("test.json");
	myPlacementObject.doPlacement();
    return 0;
}
