#include <iostream>
#include <fstream>
#include "schematic.h"

int main(int argc, char ** argv){
//	if(argc < 2)
//		throw std::invalid_argument("test file not specified");
	std::ifstream t("test.json");
	std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
	schematic myPlacementObject;
	std::cout<< myPlacementObject.createDetailedJsonSchematicFromJson(str)<<std::endl;
	return 0;
}
