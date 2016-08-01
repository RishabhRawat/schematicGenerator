#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <string>

class coreDesign;
class terminal;
class module;
class terminalImpl;
class moduleImpl;

/**
 * @enum Enumerates the port type of a terminal
 */
enum class terminalType { in, out, inout };

/**
 * @class public side module class, used to create and access modules
 */
class module {
	friend class schematic;
	moduleImpl* const modulePointer;
	module(moduleImpl* const modulePointer) : modulePointer(modulePointer) {}

public:
	module& setSize(const int width, const int height);
	module& setPosition(const int x, const int y);
	int getWidth();
	int getHeight();
	int getPositionX();
	int getPositionY();
	terminal addTerminal(const std::string& terminalName, const terminalType type, const int width);
	terminal getTerminal(const std::string& terminalIdentifier);
};

/**
 * @class public side terminal class, used to create and access modules
 */
class terminal {
	friend class schematic;
	friend class module;
	terminalImpl* terminalPointer;
	unsigned int highestIndex, lowestIndex;

private:
	std::string constantValue;
	terminal(terminalImpl* terminalPointer, const unsigned int highestIndex, const unsigned int lowestIndex)
		: terminalPointer(terminalPointer), highestIndex(highestIndex), lowestIndex(lowestIndex), constantValue("") {}

public:
	terminal(std::string constValue)
			: terminalPointer(nullptr),
			  highestIndex(static_cast<unsigned int>(constValue.length() - 1)),
			  lowestIndex(0),
			  constantValue(constValue) {}
	terminal partial(unsigned int highIndex, unsigned int lowIndex);
	terminal& connect(terminal t);

	unsigned int getWidth() const {
		return highestIndex - lowestIndex + 1;
	}
};

/**
 * @class Main class for schematic generation
 */
class schematic {
private:
	coreDesign* pSchematicGenerator;
	std::string routedJsonData;
	std::string placedJsonData;

public:
	schematic();

	~schematic();
	// Design Parameters
	// With default Values
	unsigned int wireModuleDistance = 5;
	unsigned int maxPartitionSize = 50;
	unsigned int maxPartitionConnections = 20;

	unsigned int maxPathLength = 10;
	module addModule(const std::string& moduleName);

	module getModule(const std::string& moduleName);
	terminal addSystemTerminal(const std::string& terminalName, const terminalType type, const int width);

	terminal getSystemTerminal(const std::string& terminalIdentifier);
	void doPlacement();

	void doRouting();
	void parseJsonFile(std::string fileName);

	void parseYosysJson(std::string jsonText);
	std::string getPlacedModulesJson();

	std::string getRoutedNetsJson();
	std::string createDetailedJsonSchematicFromJson(std::string jsonData);

	std::string createJsonSchematicFromJson(std::string jsonData);
};

#endif  // SCHEMATIC_H