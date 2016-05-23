#include <iostream>
#include "placement.h"

int main(){
    std::string fileName = "tests/test.json";
    placement myPlacementObject;
	myPlacementObject.parseJson(fileName);
    return 0;
}
