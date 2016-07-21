#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include "hashlib.h"

enum class termType { inType, outType, inoutType };

enum terminalSide { leftSide = 0, topSide = 1, rightSide = 2, bottomSide = 3, noneSide = 4 };
enum class clockwiseRotation { d_0 = 0, d_90 = 1, d_180 = 2, d_270 = 3 };

struct intPair {
	int x;
	int y;
	intPair operator+(const intPair& rhs) const {
		return {x + rhs.x, y + rhs.y};
	}
	intPair& operator+=(const intPair& rhs) {
		return *this = *this + rhs;
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
	 * This is the length used in placing modules reserved for string wires
	 */
	unsigned int wireModuleDistance;
	// Important Layout Parameters, just guesses for now
	unsigned int maxPartitionSize;
	unsigned int maxPartitionConnections;
	unsigned int maxPathLength;

	schematicParameters(unsigned int wireModuleDistance, unsigned int maxPartitionSize,
			unsigned int maxPartitionConnections, unsigned int maxPathLength)
		: wireModuleDistance(wireModuleDistance),
		  maxPartitionSize(maxPartitionSize),
		  maxPartitionConnections(maxPartitionConnections),
		  maxPathLength(maxPathLength) {}

	schematicParameters()
		: wireModuleDistance(5), maxPartitionSize(50), maxPartitionConnections(20), maxPathLength(10) {}
};

class coreDesign;

class moduleImpl;
class box;
class partition;

class terminalImpl;
class splicedTerminal;

class net;
struct bitNet;

namespace hashlib {
template <>
struct hash_ops<moduleImpl*> : hash_ptr_ops {};
template <>
struct hash_ops<box*> : hash_ptr_ops {};
template <>
struct hash_ops<partition*> : hash_ptr_ops {};
template <>
struct hash_ops<splicedTerminal*> : hash_ptr_ops {};
template <>
struct hash_ops<net*> : hash_ptr_ops {};
}

struct ulink {
	net* linkNet;
	splicedTerminal* linkSource;
	// This vector is not owned by ulink but comes from moduleSplicedTerminalMap
	std::vector<splicedTerminal*>* linkSink;
	ulink(net* linkNet, splicedTerminal* linkSource, std::vector<splicedTerminal*>* linkSink)
		: linkNet(linkNet), linkSource(linkSource), linkSink(linkSink) {}
};

typedef hashlib::dict<std::string, moduleImpl*> namedModuleCollection;
typedef hashlib::dict<std::string, terminalImpl*> namedTerminalCollection;

typedef std::pair<std::string, moduleImpl*> namedModulePair;
typedef std::pair<std::string, terminalImpl*> namedTerminalPair;

typedef std::vector<moduleImpl*> moduleVector;
typedef std::vector<splicedTerminal*> splicedTerminalVector;
typedef std::vector<ulink*> linkVector;
typedef std::vector<box*> boxVector;
typedef std::vector<partition*> partitionVector;

typedef hashlib::dict<moduleImpl*, std::vector<ulink*>> moduleLinkMap;
typedef hashlib::dict<moduleImpl*, std::vector<splicedTerminal*>> moduleSplicedTerminalMap;

typedef std::pair<moduleImpl*, std::vector<ulink*>> moduleLinkPair;
typedef std::pair<moduleImpl*, std::vector<splicedTerminal*>> moduleSplicedTerminalPair;

typedef hashlib::pool<splicedTerminal*> splicedTerminalSet;
typedef hashlib::pool<moduleImpl*> moduleSet;
typedef hashlib::pool<net*> netSet;

struct exportStructure;

#endif  // COMMON_H
