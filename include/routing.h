#ifndef ROUTING_H
#define ROUTING_H

#include <set>
#include <unordered_set>
#include "common.h"
#include "net.h"

class routing {
	enum class direction { left, up, right, down };
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
		activeSegment* originSegment;
		segment currentSegment;
		direction dir;

		activeSegment(activeSegment* origin, segment current, direction dir)
			: originSegment(origin), segment(current), dir(dir) {}

		activeSegment(
				int bends, int crossedNets, activeSegment* originSegment, const segment& currentSegment, direction dir)
			: bends(bends),
			  crossedNets(crossedNets),
			  originSegment(originSegment),
			  currentSegment(currentSegment),
			  dir(dir) {}
	};

	struct endSegment : segment {
		enum class endType { leftCrossing, rightCrossing, segment };
		endType type;

		endSegment(int index, int end1, int end2, endType type) : index(index), end1(end1), end2(end2), type(type){};
	};
	struct obstacleSegmentLessComparator {
		bool operator()(const obstacleSegment* lhs, const obstacleSegment* rhs) const {
			// I want to order it using index, but I want them to be equal only if pointers match
			if (lhs->index < rhs->index)
				return true;
			else
				return lhs < rhs;
		}
	};
	std::set<obstacleSegment *, obstacleSegmentLessComparator> hObstacleSet, vObstacleSet;
	const coreDesign* core;

	coalescedNet* currentNet;
	std::unordered_set<activeSegment *> activesA, activesB;
	std::unordered_set<endSegment*> E;
	bool solutionFound;

	void addObstacleBounding();
	void initNet(splicedTerminal* t0, splicedTerminal* t1);
	void expandNet(splicedTerminal* pTerminal);
	void route();

	routing(coreDesign* core) : core(core) {}
	void initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t);
	void expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
			std::unordered_set<activeSegment*>& incrementedActSegmentSet);
	void reconstructSolution();
	void expandSegment(activeSegment* actSegment, std::unordered_set<activeSegment*>& actSegmentSet);

	bool straightLine(splicedTerminal* t0, splicedTerminal* t1);

	void solidObstacle(int i, int end1, int end2, activeSegment* actS);

	void transparentObstacle();

	void reconstructPath(activeSegment* pSegment, int x);

	int routing::pathLength(routing::activeSegment* actS, int x);
};
#endif  // ROUTING_H
