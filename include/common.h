#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <vector>
#include <deque>
#include "hashlib.h"

/**
 * @enum Enumerates the type/direction of the terminal
 */
enum class termType { inType, outType, inoutType };

/**
 * @enum
 * Enumerates the terminal positioning relative to the module
 */
enum terminalSide { leftSide = 0, topSide = 1, rightSide = 2, bottomSide = 3, noneSide = 4 };

/**
 * @enum
 * Enumerates the possible rotations of a module
 */
enum class clockwiseRotation { d_0 = 0, d_90 = 1, d_180 = 2, d_270 = 3 };

/**
 * @struct intPair
 * A class for containing a pair of integers. Mostly used as a container for coordinates
 */
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
	intPair operator/(int divisor) const {
		return {x / divisor, y / divisor};
	}
	intPair operator*(int multiplier) const {
		return {x * multiplier, y * multiplier};
	}
	bool operator<(const intPair& rhs) const {
		return (x < rhs.x && y < rhs.y);
	}
	bool operator>(const intPair& rhs) const {
		return (x > rhs.x && y > rhs.y);
	}

	/**
	 * Returns a reference to the contained element
	 * @param index Selects the element to be returned, returns second element if the index evaluates to a truth value
	 * @return a reference to the requested element
	 */
	int& operator[](const int index) {
		return index ? y : x;
	}

	/**
	 * Returns a contained element
	 * @param index Selects the element to be returned, returns second element if the index evaluates to a truth value
	 * @return the requested element
	 */
	int operator[](const int a) const {
		return a ? y : x;
	}
	/**
	 * Returns another intPair with only the contained element
	 * @param index Selects the element to be returned, returns second element if the index evaluates to a truth value
	 * @return a intPair with the requested element
	 */
	const intPair component(const int a) const {
		return a ? intPair{0, y} : intPair{x, 0};
	}

	bool operator==(const intPair& rhs) const {
		return x==rhs.x &&
				y==rhs.y;
	}

	bool operator!=(const intPair& rhs) const {
		return !(rhs==*this);
	}
	/**
	 * Returns the L2Norm of two intPair. Useful only if both parameters store coordinates
	 * @param a First coordinate
	 * @param b Second coordinate
	 * @return distance between the two coordinates
	 */
	static unsigned int L2norm_sq(const intPair a, const intPair b) {
		return static_cast<unsigned int>((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
	}
};

/**
 * @class Stores configuration data for generating the schematic
 */
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

/**
 * @struct Represents a unidirectional link, will soon be merged with net class
 */
//TODO: Merge this with net class
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

typedef std::deque<intPair> line;

struct exportStructure;

#endif  // COMMON_H
