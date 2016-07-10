#ifndef ROUTING_H
#define ROUTING_H

#include <set>
#include <unordered_set>
#include "common.h"
#include "net.h"

class routing {
	friend class coreDesign;
	enum direction { left = 0, up = 1, right = 2, down = 3 };

	struct segment {
		int index;
		int end1;
		int end2;

		segment(){};

		segment(int index, int end1, int end2) : index(index), end1(end1), end2(end2){};
	};

	struct obstacleSegment : segment {
		enum obstacleType { module, net, startA, startB, compare };
		obstacleType type;
		void* sourcePtr = nullptr;

		obstacleSegment(int index, int end1, int end2, obstacleType oType, void* sourcePtr)
			: segment(index, end1, end2), type(oType), sourcePtr(sourcePtr) {}
		obstacleSegment(int i) : segment(i, 0, 0), type(compare) {}
	};

	struct activeSegment : segment {
		direction dir;
		int bends = -1;
		int crossedNets = 0;
		activeSegment* prevSegment;

		activeSegment(int index, int end1, int end2, direction dir, activeSegment* prevSegment)
			: segment(index, end1, end2), dir(dir), prevSegment(prevSegment) {}

		activeSegment(
				int bends, int crossedNets, int index, int end1, int end2, direction dir, activeSegment* prevSegment)
			: segment(index, end1, end2), dir(dir), bends(bends), crossedNets(crossedNets), prevSegment(prevSegment) {}
		bool isUpRight() {
			return dir == right || dir == up;
		}

		bool isVertical() {
			return dir == up || dir == down;
		}
	};

	struct endSegment : segment {
		// Index is distance from the baseSegment index, in the direction of baseSegment->dir
		int length;
		int crossovers;
		activeSegment* baseSegment;
		endSegment(int length, int end1, int end2, int crossovers, activeSegment* baseSegment)
			: segment(0, end1, end2), length(length), crossovers(crossovers), baseSegment(baseSegment){};
		endSegment(int index, int length, int end1, int end2, int crossovers, activeSegment* baseSegment)
			: segment(index, end1, end2), length(length), crossovers(crossovers), baseSegment(baseSegment){};
		bool isUpRight() {
			return baseSegment->isUpRight();
		}
		bool isVertical() {
			return baseSegment->isVertical();
		}
	};

	struct obstacleSegmentAscendingComparator {
		bool operator()(const obstacleSegment* lhs, const obstacleSegment* rhs) const {
			// I want to order it using index, but I want them to be equal only if pointers match
			if (lhs->type == obstacleSegment::compare || rhs->type == obstacleSegment::compare)
				return lhs->index < rhs->index;
			else
				return lhs->index < rhs->index || lhs < rhs;
		}
	};

	struct endSegmentLessComparator {
		bool operator()(const endSegment* lhs, const endSegment* rhs) const {
			// I want to order it using left ends, but they may overlap only if we have transparent obstacles (nets)
			// In that case we want to order it according to the distance from base segment
			// Note: two endSegments cannot overlap with the same index
			return (lhs->end1 != rhs->end1)?(lhs->end1 < rhs->end1):(lhs->index < rhs->index);
		}
	};

	struct optimumSolution {
		int cost = INT32_MAX;
		intPair optimalPoint = {0,0};
		activeSegment *a = nullptr, *b = nullptr;
	} soln;

	using orderedObstacleSet = std::set<obstacleSegment*, obstacleSegmentAscendingComparator>;
	orderedObstacleSet hObstacleSet, vObstacleSet;
	const coreDesign* core;

	coalescedNet* currentNet;
	std::unordered_set<activeSegment *> activesA, activesB;
	std::set<endSegment*, endSegmentLessComparator> E;

	void addObstacleBounding();

	void initNet(splicedTerminal* t0, splicedTerminal* t1);

	void expandNet(splicedTerminal* pTerminal);

	void route();

	routing(coreDesign* core) : core(core) {}

	void initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t);

	bool expandActives(std::unordered_set<activeSegment*>& actSegmentSet,
			std::unordered_set<activeSegment*>& incrementedActSegmentSet);

	void reconstructSolution();

	bool generateEndSegments(activeSegment* actSegment, segment s, int crossovers, orderedObstacleSet& obstacleSet);
	bool straightLine(splicedTerminal* t0, splicedTerminal* t1);

	void reconstructPath(activeSegment* pSegment, int x);

	unsigned int pathLength(activeSegment* actS, int x);

	void addActiveFunction(endSegment* ePrevHighest, endSegment* e, endSegment* eNextHighest, activeSegment* actS,
			std::unordered_set<activeSegment*>& nextActSSet);
	void newActives(activeSegment* actS, std::unordered_set<activeSegment*>& newActSegmentSet);

	void updateSolution(segment s, obstacleSegment* obstacle, activeSegment* actSegment);
};
#endif  // ROUTING_H
