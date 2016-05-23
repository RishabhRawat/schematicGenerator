#ifndef COMMON_H
#define COMMON_H
#include "hashlib.h"
#include <vector>
#include <string>

namespace schematic {
	enum terminalType {
		in, out, inout
	};

	enum terminalSide {
		left, right, top, bottom, none
	};

	enum clockwiseRotation {
		d_0 = 0,
		d_90 = 1,
		d_180 = 2,
		d_270 = 3
	};


}

struct intPair {
	int x;
	int y;
};



class module;
class terminal;
class net;
class box;
class partition;

namespace hashlib {
	template<> struct hash_ops<net*> : hash_ptr_ops {};
	template<> struct hash_ops<module*> : hash_ptr_ops {};
	template<> struct hash_ops<box*> : hash_ptr_ops {};
	template<> struct hash_ops<partition*> : hash_ptr_ops {};
}

struct ulink{
	net* linkNet;
	terminal * linkSource;
	std::vector<terminal*> * linkSink; //This will come from moduleTerminalMap
	//So the moment that gets deleted this pointer is invalid
	//But that is a good thing i believe
	ulink(net *linkNet, terminal *linkSource, std::vector<terminal *> *linkSink) : linkNet(linkNet),
	                                                                               linkSource(linkSource),
	                                                                               linkSink(linkSink) { }
};


typedef hashlib::dict<std::string,module> namedModuleCollection;
typedef hashlib::dict<std::string,terminal> namedTerminalCollection;
typedef hashlib::dict<std::string,net> namedNetCollection;

typedef std::pair<std::string,module> namedModulePair;
typedef std::pair<std::string,terminal> namedTerminalPair;
typedef std::pair<std::string,net> namedNetPair;


typedef std::vector<module*> moduleCollection;
typedef std::vector<terminal*> terminalCollection;
typedef std::vector<net*> netCollection;
typedef std::vector<ulink*> linkCollection;
typedef std::vector<box*> boxCollection;
typedef std::vector<partition*> partitionCollection;

typedef hashlib::dict<module*,std::vector<ulink*>> moduleLinkMap;
typedef hashlib::dict<module*,std::vector<terminal*>> moduleTerminalMap;

typedef std::pair<module*,std::vector<ulink*>> moduleLinkPair;
typedef std::pair<module*,std::vector<terminal*>> moduleTerminalPair;


#endif // COMMON_H
