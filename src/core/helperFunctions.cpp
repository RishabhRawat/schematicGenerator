#include <sstream>
#include <iostream>
#include <schematic.h>
#include <moduleImpl.h>
#include "common.h"
using namespace std;
void allModules(namedModuleCollection nC) {
	int i = 0;
	for (namedModulePair& np : nC) {
		cout<<i<<" "<<np.first<<" "<<np.second<<std::endl;
		i++;
	}
}