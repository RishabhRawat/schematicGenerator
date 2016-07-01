#include <box.h>
#include "coreDesign.h"
#include "json/json.hpp"
#include "partition.h"
#include "placement.h"

std::string coreDesign::createDebugJsonSchematicFromJson(std::string jsonData) {
	parseJsonString(jsonData);
	initializeStructures();
	placement placementObject;
	placementObject.place(this, designParameters);

	nlohmann::json outputJson;
	outputJson["name"] = systemModule.moduleIdentifier;
	outputJson["size_x"] = size.x;
	outputJson["size_y"] = size.y;
	outputJson["offset_x"] = offset.x;
	outputJson["offset_y"] = offset.y;

	for (splicedTerminal* t : systemModule.moduleSplicedTerminals) {
		nlohmann::json tS;
		tS["name"] = t->terminalIdentifier;
		tS["pos_x"] = t->placedPosition.x;
		tS["pos_y"] = t->placedPosition.y;
		outputJson["systemTerminals"].push_back(tS);
	}

	int pCount = 0;
	for (partition* p : placementObject.allPartitions) {
		nlohmann::json pS;
		pS["name"] = std::to_string(pCount);
		pS["pos_x"] = p->position.x;
		pS["pos_y"] = p->position.y;
		pS["size_x"] = p->size.x;
		pS["size_y"] = p->size.y;
		pS["offset_x"] = p->offset.x;
		pS["offset_y"] = p->offset.y;
		int bCount = 0;
		for (box* b : p->partitionBoxes) {
			nlohmann::json bS;
			bS["name"] = std::to_string(bCount);
			bS["pos_x"] = b->position.x;
			bS["pos_y"] = b->position.y;
			bS["size_x"] = b->size.x;
			bS["size_y"] = b->size.y;
			bS["offset_x"] = b->offset.x;
			bS["offset_y"] = b->offset.y;
			for (module* m : b->boxModules) {
				nlohmann::json mS;
				mS["name"] = m->moduleIdentifier;
				mS["pos_x"] = m->position.x;
				mS["pos_y"] = m->position.y;
				mS["size_x"] = m->size.x;
				mS["size_y"] = m->size.y;
				for (splicedTerminal* t : m->moduleSplicedTerminals) {
					nlohmann::json tS;
					tS["name"] = t->terminalIdentifier;
					tS["pos_x"] = t->placedPosition.x;
					tS["pos_y"] = t->placedPosition.y;
					mS["terminals"].push_back(tS);
				}
				bS["modules"].push_back(mS);
			}
			pS["boxes"].push_back(bS);
		}
		outputJson["partitions"].push_back(pS);
	}
	return outputJson.dump(2);
}

std::string coreDesign::createJsonSchematicFromJson(std::string jsonData) {
	parseJsonString(jsonData);
	doPlacement();

	nlohmann::json outputJson;
	outputJson["name"] = systemModule.moduleIdentifier;
	outputJson["size_x"] = size.x;
	outputJson["size_y"] = size.y;

	for (splicedTerminal* t : systemModule.moduleSplicedTerminals) {
		nlohmann::json tS;
		tS["name"] = t->terminalIdentifier;
		tS["pos_x"] = t->placedPosition.x;
		tS["pos_y"] = t->placedPosition.y;
		outputJson["systemTerminals"].push_back(tS);
	}

	for (namedModulePair& m_pair : subModules) {
		nlohmann::json mS;
		mS["name"] = m_pair.second->moduleIdentifier;
		mS["pos_x"] = m_pair.second->position.x;
		mS["pos_y"] = m_pair.second->position.y;
		mS["size_x"] = m_pair.second->size.x;
		mS["size_y"] = m_pair.second->size.y;
		for (splicedTerminal* t : m_pair.second->moduleSplicedTerminals) {
			nlohmann::json tS;
			tS["name"] = t->terminalIdentifier;
			tS["pos_x"] = t->placedPosition.x;
			tS["pos_y"] = t->placedPosition.y;
			mS["terminals"].push_back(tS);
		}
		outputJson["modules"].push_back(mS);
	}
	return outputJson.dump(2);
}
