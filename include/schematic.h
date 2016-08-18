#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <string>

class coreDesign;
class terminal;
class module;
class terminalImpl;
class moduleImpl;

/**
 * @enum terminalType
 * Enumerates the port type of a terminal
 */
enum class terminalType { in, out, inout };

/**
 * @class module
 * public side module class, used to create and access modules
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
 * @class terminal
 * public side terminal class, used to create and access modules
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
	/**
	 * Constructor for constructing constant terminals, non-constant terminals need module::addTerminals or
	 * schematic::addSystemTerminals
	 * @param constValue
	 * @return
	 */
	terminal(std::string constValue)
		: terminalPointer(nullptr),
		  highestIndex(static_cast<unsigned int>(constValue.length() - 1)),
		  lowestIndex(0),
		  constantValue(constValue) {}
	/**
	 * Returns a partial terminal, useful for splicing terminals
	 * @param highIndex
	 * @param lowIndex
	 * @return
	 */
	terminal partial(unsigned int highIndex, unsigned int lowIndex);
	/**
	 * Connects two terminals provided they are equal in width, If not use terminal::partial to splice them first
	 * @param t
	 * @return
	 */
	terminal& connect(terminal t);

	unsigned int getWidth() const {
		return highestIndex - lowestIndex + 1;
	}
};

/**
 * @class schematic
 * Main class for schematic generation
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
	unsigned int aspectRatioX = 4, aspectRatioY = 3;

	/**
	 * Adds a module to the design
	 * @param moduleName unique name for the module
	 * @return
	 */
	module addModule(const std::string& moduleName);

	/**
	 * Retrieve a module with corrosponding name
	 * @param moduleName
	 * @return
	 */
	module getModule(const std::string& moduleName);
	/**
	 * Adds a top level terminal
	 * @param terminalName
	 * @param type
	 * @param width
	 * @return
	 */
	terminal addSystemTerminal(const std::string& terminalName, const terminalType type, const int width);

	/**
	 * Retrieve a toplevel terminal with corrosponding name
	 * @param terminalIdentifier
	 * @return
	 */
	terminal getSystemTerminal(const std::string& terminalIdentifier);
	/**
	 * Run Placement Algorithm
	 */
	void doPlacement();

	/**
	 * Run Routing Algorithm
	 */
	void doRouting();
	void parseJsonFile(std::string fileName);

	void parseYosysJson(std::string jsonText);
	/**
	 * Returns the placement results as a json string
	 * @return
	 */
	std::string getPlacedModulesJson();

	/**
	 * Returns the routing results as a json string
	 * @return
	 */
	std::string getRoutedNetsJson();
	/**
	 * Deprecated
	 * @param jsonData
	 * @return
	 */
	std::string createDetailedJsonSchematicFromJson(std::string jsonData);
	/**
	 * Deprecated
	 * @param jsonData
	 * @return
	 */
	std::string createJsonSchematicFromJson(std::string jsonData);

	/**
	 * Sets the aspect ratio of the generated schematic. It influences the cost function of placement, and may not result in exactly same ratio, instead it is more like a suggestion to the algorithm.
	 * @param x
	 * @param y
	 */
	void setAspectRatio(unsigned int x, unsigned int y);
};

#endif  // SCHEMATIC_H