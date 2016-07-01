#include "json/json.hpp"
#include <fstream>
#include <system_error>
#include "coreDesign.h"
namespace schematic {
terminalType parseTerminalType(std::string t) {
	if (t == "in" || t == "input")
		return terminalType::inType;
	else if (t == "output" || t == "out")
		return terminalType::outType;
	else if (t == "inoutType")
		return terminalType::inoutType;
	else
		throw std::invalid_argument("unknown port direction");
}
}

void coreDesign::parseJsonFile(std::string fName) {
	std::ifstream jsonFile(fName);
	if (!jsonFile)
		throw std::system_error(errno, std::system_category());

	nlohmann::json parsedJson;
	jsonFile >> parsedJson;
	parseJson(parsedJson);
}
void coreDesign::parseJsonString(std::string jsonText) {
	parseJson(nlohmann::json::parse(jsonText));
}
void coreDesign::parseJson(nlohmann::json parsedJson) {
	unsigned int netNumberGuess = 50;
	unsigned int netIncrementNumberGuess = 10;

	if (parsedJson["modules"].size() > 1)
		// FIXME: better message?
		throw std::length_error("Only one module systems are supported");

	struct netStruct {
		unsigned int index;
		net* baseNet;
	};

	std::vector<netStruct> netData(netNumberGuess);

	for (nlohmann::json::iterator m_iter = parsedJson["modules"].begin(); m_iter != parsedJson["modules"].end();
			++m_iter) {
		for (nlohmann::json::iterator n_iter = m_iter.value()["netnames"].begin();
				n_iter != m_iter.value()["netnames"].end(); ++n_iter) {
			net& n = addNet(n_iter.key(), n_iter.value()["bits"].size());

			for (unsigned int i = 0; i < n_iter.value()["bits"].size(); ++i) {
				unsigned int bit;
				try {
					bit = n_iter.value()["bits"][i].get<int>();
				} catch (std::domain_error ex) {
					// FIXME: Take care of the constants :/
					bit = (unsigned)std::stoi(n_iter.value()["bits"][i].get<std::string>());
				}
				try {
					netData.at(bit) = netStruct{i, &n};
				} catch (std::out_of_range ex) {
					netData.resize(bit + netIncrementNumberGuess);
					netData.at(bit) = netStruct{i, &n};
				}
			}
		}

		for (nlohmann::json::iterator cell_iter = m_iter.value()["cells"].begin();
				cell_iter != m_iter.value()["cells"].end(); ++cell_iter) {
			module& m = addModule(cell_iter.key());

			for (nlohmann::json::iterator t_iter = cell_iter.value()["port_directions"].begin();
					t_iter != cell_iter.value()["port_directions"].end(); ++t_iter) {
				terminal& t = m.addTerminal(t_iter.key(),
						schematic::parseTerminalType(cell_iter.value()["port_directions"][t_iter.key()]),
						cell_iter.value()["connections"][t_iter.key()].size());

				for (int i = 0; i < t.terminalWidth; ++i) {
					unsigned int bit;
					try {
						bit = cell_iter.value()["connections"][t_iter.key()][i].get<int>();
						t[i].connect((*netData[bit].baseNet)[netData[bit].index]);
					} catch (std::domain_error ex) {
						// FIXME: Take care of the constants :/
						std::string temp = cell_iter.value()["connections"][t_iter.key()][i].get<std::string>();
						if (temp == "0" || temp == "x" || temp == "z" )
							bit = 0;
						else
							bit = 1;
					}
				}
			}
		}

		for (nlohmann::json::iterator sysT_iter = m_iter.value()["ports"].begin();
				sysT_iter != m_iter.value()["ports"].end(); ++sysT_iter) {
			terminal& t = addSystemTerminal(sysT_iter.key(),
					schematic::parseTerminalType(sysT_iter.value()["direction"]), sysT_iter.value()["bits"].size());

			for (int i = 0; i < t.terminalWidth; ++i) {
				unsigned int bit;
				try {
					bit = sysT_iter.value()["bits"][i].get<int>();
					t[i].connect((*netData[bit].baseNet)[netData[bit].index]);
				} catch (std::domain_error ex) {
					// FIXME: Take care of the constants :/
					bit = (unsigned)std::stoi(sysT_iter.value()["bits"][i].get<std::string>());
				}
			}
		}
	}
}
