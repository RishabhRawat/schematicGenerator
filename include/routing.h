#ifndef ROUTING_H
#define ROUTING_H

#include "common.h"
#include "net.h"

class routing {
public:
	enum class direction { left, up, right, down, none };
	struct segment {
		int index;
		int end1;
		int end2;
	};

	struct obstacleSegment : segment {
		enum class obstacleType { module, net, startA, startB };
		obstacleType type;
		coalescedNet* net;

		obstacleSegment(int i, int end1, int end2, obstacleType oType, coalescedNet* net)
			: index(i), end1(end1), end2(end2), type(type), net(net) {}
	};

	struct activeSegment {
		int bends = -1;
		int crossedNets = 0;
		segment originSegment, currentSegment;
		direction originDirection, currentDirection;

		activeSegment(segment origin, direction originDir, segment current, direction currentDir)
			: originSegment(origin), originDirection(originDir), originSegment(origin), originDirection(originDir) {}

		activeSegment(
				segment origin, direction originDir, segment current, direction currentDir, int bends, int crossedNets)
			: bends(bends),
			  crossedNets(crossedNets),
			  originSegment(origin),
			  originDirection(originDir),
			  originSegment(origin),
			  originDirection(originDir) {}
	};

	struct endSegment : segment {
		enum class endType { leftCrossing, rightCrossing, segment };
		endType type;
	};

private:
	hashlib::pool<obstacleSegment *> hObstacleSet, vObstacleSet;
	const coreDesign* core;

	coalescedNet* currentNet;
	hashlib::pool<activeSegment*> A, B;
	bool solutionFound;

	void addObstacleBounding();
	void initNet(splicedTerminal* t0, splicedTerminal* t1);
	void expandNet(splicedTerminal* pTerminal);
	void route();

	routing(coreDesign* core) : core(core) {}
	void initActives(hashlib::pool<activeSegment*>& activeSet, const splicedTerminal* t);
	bool expandActives(hashlib::pool<activeSegment*>& actSegmentSet, hashlib::pool<activeSegment*>& incrementedActSegmentSet);
	void reconstructSolution();
	bool expandSegment(activeSegment* actSegment, hashlib::pool<activeSegment*>& actSegmentSet);
};

namespace hashlib {
template <>
struct hash_ops<routing::obstacleSegment*> : hash_ptr_ops {};
template <>
struct hash_ops<routing::activeSegment*> : hash_ptr_ops {};
template <>
struct hash_ops<routing::endSegment*> : hash_ptr_ops {};
};

#endif  // ROUTING_H
