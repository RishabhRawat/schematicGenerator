#include <fstream>
#include <iostream>
#include "schematic.h"

int main(int argc, char** argv) {
	//	if(argc < 2)
	//		throw std::invalid_argument("test file not specified");
	std::ifstream t("test.json");
	std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	schematic myPlacementObject;
//	myPlacementObject.createJsonSchematicFromJson(str);
	std::cout << myPlacementObject.createJsonSchematicFromJson(str) << std::endl;
	myPlacementObject.doRouting();
	return 0;
}
