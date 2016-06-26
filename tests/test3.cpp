#include <iostream>
#include <fstream>
#include "schematicGenerator.h"

int main(int argc, char ** argv){
//	if(argc < 2)
//		throw std::invalid_argument("test file not specified");
	std::ifstream t("test.json");
	std::string str((std::istreambuf_iterator<char>(t)),
			std::istreambuf_iterator<char>());
	schematicGenerator myPlacementObject;
	myPlacementObject.createJsonSchematicFromJson(str);
	return 0;
}
