#include <iostream>
#include "schematicGenerator.h"

int main(){
    std::string fileName = "tests/test.json";
    schematicGenerator myPlacementObject;
	myPlacementObject.parseJson(fileName);
    return 0;
}
