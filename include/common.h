#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include "hashlib.h"

enum class terminalType { inType, outType, inoutType };

enum class terminalSide { leftSide, rightSide, topSide, bottomSide, noneSide };
enum class clockwiseRotation { d_0 = 0, d_90 = 1, d_180 = 2, d_270 = 3 };

struct intPair {
	int x;
	int y;
	intPair operator+(const intPair& rhs) const {
		return {x + rhs.x, y + rhs.y};
	}
	intPair& operator+=(const intPair& rhs) {
		return *this=*this+rhs;
	}
	intPair operator-(const intPair& rhs) const {
		return {x - rhs.x, y - rhs.y};
	}
	intPair operator/(int a) const {
		return {x / a, y / a};
	}
	intPair operator*(int a) const {
		return {x * a, y * a};
	}
	bool operator<(const intPair& rhs) const {
		return (x < rhs.x && y < rhs.y);
	}
	bool operator>(const intPair& rhs) const {
		return (x > rhs.x && y > rhs.y);
	}
	int& operator[](const int a) {
		return a ? y : x;
	}
	int operator[](const int a) const {
		return a ? y : x;
	}
	const intPair component(const int a) const {
		// returns y component of evaluating true
		return a ? intPair{0, y} : intPair{x, 0};
	}
	static unsigned int L2norm_sq(const intPair a, const intPair b) {
		return static_cast<unsigned int>((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}
};

struct schematicParameters {
	/*
	 * This is the distance used in placing modules reserved for string wires
	 */
	unsigned int wireModuleDistance = 5;
	// Important Layout Parameters, just guesses for now
	unsigned int maxPartitionSize = 50;
	unsigned int maxPartitionConnections = 20;
	unsigned int maxPathLength = 10;

};

class coreDesign;

class module;
class box;
class partition;

class terminal;
class splicedTerminal;
struct bitTerminal;

class net;
class coalescedNet;
struct bitNet;


namespace hashlib {
template <>
struct hash_ops<net*> : hash_ptr_ops {};
template <>
struct hash_ops<module*> : hash_ptr_ops {};
template <>
struct hash_ops<box*> : hash_ptr_ops {};
template <>
struct hash_ops<partition*> : hash_ptr_ops {};
template <>
struct hash_ops<splicedTerminal*> : hash_ptr_ops {};
template <>
struct hash_ops<coalescedNet*> : hash_ptr_ops {};
}

struct ulink {
	coalescedNet* linkNet;
	splicedTerminal* linkSource;
	// This vector is not owned by ulink but comes from moduleSplicedTerminalMap
	std::vector<splicedTerminal*>* linkSink;
	ulink(coalescedNet* linkNet, splicedTerminal* linkSource, std::vector<splicedTerminal*>* linkSink)
		: linkNet(linkNet), linkSource(linkSource), linkSink(linkSink) {}
};

typedef hashlib::dict<std::string, module*> namedModuleCollection;
typedef hashlib::dict<std::string, terminal*> namedTerminalCollection;
typedef hashlib::dict<std::string, net*> namedNetCollection;

typedef std::pair<std::string, module*> namedModulePair;
typedef std::pair<std::string, terminal*> namedTerminalPair;
typedef std::pair<std::string, net*> namedNetPair;

typedef std::vector<module*> moduleVector;
typedef std::vector<splicedTerminal*> splicedTerminalVector;
typedef std::vector<ulink*> linkVector;
typedef std::vector<box*> boxVector;
typedef std::vector<partition*> partitionVector;

typedef hashlib::dict<module*, std::vector<ulink*>> moduleLinkMap;
typedef hashlib::dict<module*, std::vector<splicedTerminal*>> moduleSplicedTerminalMap;

typedef std::pair<module*, std::vector<ulink*>> moduleLinkPair;
typedef std::pair<module*, std::vector<splicedTerminal*>> moduleSplicedTerminalPair;

typedef hashlib::pool<splicedTerminal*> splicedTerminalSet;
typedef hashlib::pool<module*> moduleSet;

struct exportStructure;

#endif  // COMMON_H
