#include "exportStructure.h"
#include <box.h>
#include <partition.h>
#ifdef WEB_COMPILATION
#include "emscripten/bind.h"
#endif
#include "json/json.hpp"
#include "schematicGenerator.h"

exportStructure schematicGenerator::createSchematicFromJson(std::string jsonData) {
	parseJsonString(jsonData);
	doPlacement();
	exportStructure designData;

	designData.name = systemModule.moduleIdentifier;
	designData.pos_x = 0;
	designData.pos_y = 0;
	designData.size_x = size.x;
	designData.size_y = size.y;
	designData.offset_x = offset.x;
	designData.offset_y = offset.y;
	for (splicedTerminal* t : systemModule.moduleSplicedTerminals) {
		terminalStructure tS;
		tS.name = t->terminalIdentifier;
		tS.pos_x = t->placedPosition.x;
		tS.pos_y = t->placedPosition.y;
		designData.systemTerminalList.push_back(tS);
	}

	int pCount = 0;
	for (partition* p : allPartitions) {
		partitionStructure pS;
		pS.name = std::to_string(pCount);
		;
		pS.pos_x = p->position.x;
		pS.pos_y = p->position.y;
		pS.size_x = p->size.x;
		pS.size_y = p->size.y;
		pS.offset_x = p->offset.x;
		pS.offset_y = p->offset.y;
		int bCount = 0;
		for (box* b : p->partitionBoxes) {
			boxStructure bS;
			bS.name = std::to_string(bCount);
			;
			bS.pos_x = b->position.x;
			bS.pos_y = b->position.y;
			bS.size_x = b->size.x;
			bS.size_y = b->size.y;
			bS.offset_x = b->offset.x;
			bS.offset_y = b->offset.y;
			for (module* m : b->boxModules) {
				moduleStructure mS;
				mS.name = m->moduleIdentifier;
				mS.pos_x = m->position.x;
				mS.pos_y = m->position.y;
				mS.size_x = m->size.x;
				mS.size_y = m->size.y;
				mS.offset_x = 0;
				mS.offset_y = 0;
				for (splicedTerminal* t : m->moduleSplicedTerminals) {
					terminalStructure tS;
					tS.name = t->terminalIdentifier;
					tS.pos_x = t->placedPosition.x;
					tS.pos_y = t->placedPosition.y;
					mS.terminalList.push_back(tS);
				}
				bS.moduleList.push_back(mS);
			}
			pS.boxList.push_back(bS);
		}
		designData.partitionList.push_back(pS);
	}
	return designData;
}
#ifdef WEB_COMPILATION
EMSCRIPTEN_BINDINGS(schematicGenerator) {
	emscripten::class_<schematicGenerator>("schematicGenerator")
			.constructor<>()
			//.function("createSchematicFromJson", &schematicGenerator::createSchematicFromJson)
			.function("createJsonSchematicFromJson", &schematicGenerator::createJsonSchematicFromJson);
}
#endif // WEB_COMPILATION
//	emscripten::register_vector<partitionStructure>("partitionVector");
//	emscripten::register_vector<boxStructure>("boxVector");
//	emscripten::register_vector<moduleStructure>("moduleVector");
//	emscripten::register_vector<terminalStructure>("terminalVector");
//
//
//	emscripten::value_object<exportStructure>("designStructure")
//			.field("pos_x", &exportStructure::pos_x)
//			.field("pos_y", &exportStructure::pos_y)
//			.field("size_x", &exportStructure::size_x)
//			.field("size_y", &exportStructure::size_y)
//			.field("offset_x", &exportStructure::offset_x)
//			.field("offset_y", &exportStructure::offset_y)
//			.field("name", &exportStructure::name)
//			.field("partitionList", &exportStructure::partitionList)
//			.field("systemTerminalList", &exportStructure::systemTerminalList);
//
//	emscripten::value_object<partitionStructure>("partitionStructure")
//			.field("pos_x", &partitionStructure::pos_x)
//			.field("pos_y", &partitionStructure::pos_y)
//			.field("size_x", &partitionStructure::size_x)
//			.field("size_y", &partitionStructure::size_y)
//			.field("offset_x", &partitionStructure::offset_x)
//			.field("offset_y", &partitionStructure::offset_y)
//			.field("name", &partitionStructure::name)
//			.field("boxList", &partitionStructure::boxList);
//
//	emscripten::value_object<boxStructure>("boxStructure")
//			.field("pos_x", &boxStructure::pos_x)
//			.field("pos_y", &boxStructure::pos_y)
//			.field("size_x", &boxStructure::size_x)
//			.field("size_y", &boxStructure::size_y)
//			.field("offset_x", &boxStructure::offset_x)
//			.field("offset_y", &boxStructure::offset_y)
//			.field("name", &boxStructure::name)
//			.field("moduleList", &boxStructure::moduleList);
//
//	emscripten::value_object<moduleStructure>("moduleStructure")
//			.field("pos_x", &moduleStructure::pos_x)
//			.field("pos_y", &moduleStructure::pos_y)
//			.field("size_x", &moduleStructure::size_x)
//			.field("size_y", &moduleStructure::size_y)
//			.field("offset_x", &moduleStructure::offset_x)
//			.field("offset_y", &moduleStructure::offset_y)
//			.field("name", &moduleStructure::name)
//			.field("terminalList", &moduleStructure::terminalList);
//
//	emscripten::value_object<terminalStructure>("terminalStructure")
//			.field("pos_x", &terminalStructure::pos_x)
//			.field("pos_y", &terminalStructure::pos_y)
//			.field("name", &terminalStructure::name);
//}
std::string schematicGenerator::createJsonSchematicFromJson(std::string jsonData) {
	parseJsonString(jsonData);
	doPlacement();
	exportStructure designData;
	nlohmann::json outputJson;
	outputJson["name"] = systemModule.moduleIdentifier;
	outputJson["size_x"] = size.x;
	outputJson["size_y"] = size.y;
	outputJson["offset_x"] = offset.x;
	outputJson["offset_y"] = offset.y;
	// outputJson["systemTerminals"] = nlohmann::json{};

	for (splicedTerminal* t : systemModule.moduleSplicedTerminals) {
		nlohmann::json tS;
		tS["name"] = t->terminalIdentifier;
		tS["pos_x"] = t->placedPosition.x;
		tS["pos_y"] = t->placedPosition.y;
		outputJson["systemTerminals"].push_back(tS);
	}

	int pCount = 0;
	for (partition* p : allPartitions) {
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

		std::cout<<std::endl;
		std::cout<<"dsd"<<std::endl;

		for (auto&& p : allPartitions) {
			std::cout<<"P_pos: "<<p->position.x<<", "<<p->position.y<<std::endl;
			std::cout<<"P_size: "<<p->size.x<<", "<<p->size.y<<std::endl;
			std::cout<<"P_offset: "<<p->offset.x<<", "<<p->offset.y<<std::endl;
			for (auto&& b : p->partitionBoxes) {
				std::cout<<"B_pos: "<<b->position.x<<", "<<b->position.y<<std::endl;
				std::cout<<"B_size: "<<b->size.x<<", "<<b->size.y<<std::endl;
				std::cout<<"B_offset: "<<b->offset.x<<", "<<b->offset.y<<std::endl;
//				for (auto&& m : b->boxModules) {
//					std::cout<<"M_pos: "<<m->position.x<<", "<<m->position.y<<std::endl;
//					std::cout<<"M_size: "<<m->size.x<<", "<<m->size.y<<std::endl;
//				}
			}
		}
	}
	return outputJson.dump(2);
}
