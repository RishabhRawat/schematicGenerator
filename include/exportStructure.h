#ifndef EXPORTSTRUCTURE_H
#define EXPORTSTRUCTURE_H

#include <string>
#include <vector>

struct positionData {
	int pos_x;
	int pos_y;
	int size_x;
	int size_y;
	int offset_x;
	int offset_y;
};
struct terminalStructure {
	std::string name;
	int pos_x;
	int pos_y;
};

struct moduleStructure : positionData{
	std::string name;
	std::vector<terminalStructure> terminalList;
};

struct boxStructure : positionData {
	std::string name;
	std::vector<moduleStructure> moduleList;
};

struct partitionStructure : positionData {
	std::string name;
	std::vector<boxStructure> boxList;
};

struct exportStructure : positionData{
	std::string name;
	std::vector<partitionStructure> partitionList;
	std::vector<terminalStructure> systemTerminalList;
};

#endif //SCHEMATICGENERATOR_EXPORTSTRUCTURE_H
