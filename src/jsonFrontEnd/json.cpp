#include <fstream>
#include "schematicGenerator.h"
#include "json/json.hpp"
namespace schematic{
	schematic::terminalType parseTerminalType(std::string t) {
		if(t =="in" || t == "input")
			return schematic::inType;
		else if(t == "output" || t == "out")
			return schematic::outType;
		else if(t == "inoutType")
			return schematic::inoutType;
		else
			throw std::invalid_argument("unknown port direction");
	}
}


void schematicGenerator::parseJson(std::string fName) {


    std::ifstream jsonFile(fName);
    if (!jsonFile)
        throw std::system_error(errno, std::system_category());

	nlohmann::json parsedJson;
	jsonFile>>parsedJson;

	if(parsedJson["modules"].size() > 1)
		//FIXME: better message?
		throw std::length_error("Only one module systems are supported");

	for (nlohmann::json::iterator m_iter = parsedJson["modules"].begin();
	     m_iter != parsedJson["modules"].end(); ++m_iter) {

		for (nlohmann::json::iterator sysT_iter = m_iter.value()["ports"].begin();
		     sysT_iter != m_iter.value()["ports"].end(); ++sysT_iter) {
			addSystemTerminal(sysT_iter.key(),schematic::parseTerminalType(sysT_iter.value()["direction"]),sysT_iter.value()["bits"].size());
		}

	}




}

