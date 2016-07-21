#include "json/json.hpp"
#include <fstream>
#include <system_error>
#include "schematic.h"

terminalType parseTerminalType(std::string t) {
	if (t == "in" || t == "input")
		return terminalType::in;
	else if (t == "output" || t == "out")
		return terminalType::out;
	else if (t == "inoutType")
		return terminalType::inout;
	else
		throw std::invalid_argument("unknown port direction");
}

void schematic::parseJsonFile(std::string fileName) {
	std::ifstream jsonFile(fileName);
	if (!jsonFile)
		throw std::system_error(errno, std::system_category());

	std::string jsonText((std::istreambuf_iterator<char>(jsonFile)), (std::istreambuf_iterator<char>()));
	parseYosysJson(jsonText);
}
void schematic::parseYosysJson(std::string jsonText) {
	nlohmann::json parsedJson = nlohmann::json::parse(jsonText);
	unsigned int netNumberGuess = 50;
	unsigned int netIncrementNumberGuess = 10;

	if (parsedJson["modules"].size() > 1)
		// FIXME: better message?
		throw std::length_error("Only one top level moduleImpl systems are supported");

	std::vector<std::pair<terminal, bool>> netData(netNumberGuess,std::pair<terminal, bool>{terminal{"0"},false});

	for (auto m_iter = parsedJson["modules"].begin(); m_iter != parsedJson["modules"].end(); ++m_iter) {
		for (auto c_iter = m_iter.value()["cells"].begin(); c_iter != m_iter.value()["cells"].end(); ++c_iter) {
			module m = addModule(c_iter.key());

			for (auto t_iter = c_iter.value()["port_directions"].begin();
					t_iter != c_iter.value()["port_directions"].end(); ++t_iter) {
				terminal t =
						m.addTerminal(t_iter.key(), parseTerminalType(c_iter.value()["port_directions"][t_iter.key()]),
								c_iter.value()["connections"][t_iter.key()].size());

				for (unsigned int i = 0; i < t.getWidth(); ++i) {
					unsigned int bit;
					try {
						bit = c_iter.value()["connections"][t_iter.key()][i].get<int>();
						if (bit > netData.size() || !netData.at(bit).second) {
							netNumberGuess += netIncrementNumberGuess;
							netData.resize(netNumberGuess,std::pair<terminal, bool>{terminal{"0"},false});
							netData.at(bit) = std::pair<terminal,bool>{t.partial(i, i), true};
						} else
							t.partial(i, i).connect(netData.at(bit).first);
					} catch (std::domain_error ex) {
						std::string temp = c_iter.value()["connections"][t_iter.key()][i].get<std::string>();
						t.partial(i, i).connect(temp);
					}
				}
			}
		}

		for (nlohmann::json::iterator sysT_iter = m_iter.value()["ports"].begin();
				sysT_iter != m_iter.value()["ports"].end(); ++sysT_iter) {
			terminal t = addSystemTerminal(sysT_iter.key(), parseTerminalType(sysT_iter.value()["direction"]),
					sysT_iter.value()["bits"].size());

			for (int i = 0; i < t.getWidth(); ++i) {
				unsigned int bit;
				try {
					bit = sysT_iter.value()["bits"][i].get<int>();
					if (bit > netData.size() || !netData.at(bit).second) {
						netNumberGuess += netIncrementNumberGuess;
						netData.resize(netNumberGuess,std::pair<terminal, bool>{terminal{"0"},false});
						netData.at(bit) = std::pair<terminal,bool>{t.partial(i, i), true};
					} else
						t.partial(i, i).connect(netData.at(bit).first);
				} catch (std::domain_error ex) {
					std::string temp = sysT_iter.value()["bits"][i].get<std::string>();
					t.partial(i, i).connect(temp);
				}
			}
		}
	}
}
