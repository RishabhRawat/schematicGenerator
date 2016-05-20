#ifndef COMMON_H
#define COMMON_H
#include <hashlib.h>
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
	terminal * linksource;
	std::vector<terminal*> * linksink; //This will come from moduleTerminalMap
	//So the moment that gets deleted this pointer is invalid
	//But that is a good thing i believe
};


typedef std::vector<module*> moduleCollection;
typedef std::vector<terminal*> terminalCollection;
typedef std::vector<net*> netCollection;
typedef std::vector<ulink*> linkCollection;

typedef hashlib::dict<module*,std::vector<ulink*>> moduleLinkMap;
typedef hashlib::dict<module*,std::vector<terminal*>> moduleTerminalMap;

typedef std::pair<module*,std::vector<ulink*>> moduleLinkPair;
typedef std::pair<module*,std::vector<terminal*>> moduleTerminalPair;


#endif // COMMON_H
