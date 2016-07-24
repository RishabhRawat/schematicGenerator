#ifdef WEB_COMPILATION
#include "emscripten.h"
#include "emscripten/bind.h"
#endif
#include <initializer_list>
#include "coreDesign.h"
#include "schematic.h"

schematic::schematic() {
	pSchematicGenerator = new coreDesign();
}

void schematic::doPlacement() {
	schematicParameters designParameters{wireModuleDistance, maxPartitionSize, maxPartitionConnections, maxPathLength};
	pSchematicGenerator->doPlacement(designParameters);
}

std::string schematic::createDetailedJsonSchematicFromJson(std::string jsonData) {
	parseYosysJson(jsonData);
	return pSchematicGenerator->createDebugJsonSchematicFromJson();
}

std::string schematic::createJsonSchematicFromJson(std::string jsonData) {
	parseYosysJson(jsonData);
	schematicParameters designParameters{wireModuleDistance, maxPartitionSize, maxPartitionConnections, maxPathLength};
	pSchematicGenerator->doPlacement(designParameters);
	return pSchematicGenerator->createJsonSchematicFromJson();
}
void schematic::doRouting() {
	routedJsonFile = pSchematicGenerator->doRouting();
}

schematic::~schematic() {
	delete pSchematicGenerator;
}

terminal schematic::addSystemTerminal(const std::string& terminalName, const terminalType type, const int width) {
	terminalImpl* t;
	switch (type) {
		case terminalType::in:
			t = pSchematicGenerator->systemModule.addTerminal(terminalName, termType::inType, width, true);
			break;
		case terminalType::out:
			t = pSchematicGenerator->systemModule.addTerminal(terminalName, termType::outType, width, true);
			break;
		case terminalType::inout:
			t = pSchematicGenerator->systemModule.addTerminal(terminalName, termType::inoutType, width, true);
			break;
	}
	return {t, t->highestIndex, t->lowestIndex};
}

terminal schematic::getSystemTerminal(const std::string& terminalIdentifier) {
	terminalImpl* t = pSchematicGenerator->systemModule.getTerminal(terminalIdentifier);
	return {t, t->highestIndex, t->lowestIndex};
}

module schematic::addModule(const std::string& moduleName) {
	return {pSchematicGenerator->subModules.insert(std::make_pair(moduleName, new moduleImpl(moduleName)))
					.first->second};
}

module schematic::getModule(const std::string& moduleName) {
	return {pSchematicGenerator->subModules.find(moduleName)->second};
}

std::string schematic::getRoutedNetsJson() {
	return routedJsonFile;
}

module& module::setSize(const int width, const int height) {
	modulePointer->size = {width, height};
	return *this;
}
module& module::setPosition(const int x, const int y) {
	modulePointer->position = {x, y};
	return *this;
}

int module::getHeight() {
	return modulePointer->size.y;
}

int module::getWidth() {
	return modulePointer->size.x;
}

int module::getPositionX() {
	return modulePointer->position.x;
}

int module::getPositionY() {
	return modulePointer->position.y;
}

terminal module::addTerminal(const std::string& terminalName, const terminalType type, const int width) {
	terminalImpl* t;
	switch (type) {
		case terminalType::in:
			t = modulePointer->addTerminal(terminalName, termType::inType, width, false);
			break;
		case terminalType::out:
			t = modulePointer->addTerminal(terminalName, termType::outType, width, false);
			break;
		case terminalType::inout:
			t = modulePointer->addTerminal(terminalName, termType::inoutType, width, false);
			break;
	}
	return {t, t->highestIndex, t->lowestIndex};
}

terminal module::getTerminal(const std::string& terminalIdentifier) {
	auto t = modulePointer->getTerminal(terminalIdentifier);
	return {t, t->highestIndex, t->lowestIndex};
}

terminal terminal::partial(unsigned int highIndex, unsigned int lowIndex) {
	if (!constantValue.empty())
		throw std::invalid_argument("constant Terminal cannot be spliced");

	if (highIndex < lowIndex || highIndex > terminalPointer->highestIndex || lowIndex < terminalPointer->lowestIndex)
		throw std::invalid_argument("Invalid value of indexes");

	return {terminalPointer, highIndex, lowIndex};
}

terminal& terminal::connect(terminal t) {
	if (!constantValue.empty())
		throw std::invalid_argument("constant Terminal cannot be connected");

	if (t.highestIndex - t.lowestIndex != highestIndex - lowestIndex)
		throw std::invalid_argument("Make sure the width of the connections and the ");

	if (t.constantValue.empty()) {
		for (unsigned int i = 0; i <= highestIndex - lowestIndex; ++i) {
			terminalImpl::joinbitTerminals(terminalPointer->bitTerminals.at(lowestIndex + i),
					t.terminalPointer->bitTerminals.at(t.lowestIndex + i));
		}
	} else {
		for (unsigned int i = 0; i <= highestIndex - lowestIndex; ++i) {
			terminalImpl::setBitTerminalToConst(
					terminalPointer->bitTerminals.at(lowestIndex + i), t.constantValue.at(t.highestIndex - i));
		}
	}
	return *this;
}

#ifdef WEB_COMPILATION
EMSCRIPTEN_BINDINGS(schematic) {
	emscripten::class_<schematic>("schematic")
			.constructor<>()
			.function("createJsonSchematicFromJson", &schematic::createJsonSchematicFromJson)
			.function("createDetailedJsonSchematicFromJson", &schematic::createDetailedJsonSchematicFromJson)
			.function("doRouting", &schematic::doRouting)
			.function("getRoutedNetsJson", &schematic::getRoutedNetsJson);
}
#endif  // WEB_COMPILATION
