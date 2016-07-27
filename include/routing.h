#ifndef ROUTING_H
#define ROUTING_H

#include <set>
#include <unordered_set>
#include "common.h"

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
		direction scanDirection;
		int bends = -1;
		int crossedNets = 0;
		activeSegment* prevSegment;

		activeSegment(int index, int end1, int end2, direction scanDirection, activeSegment* prevSegment)
			: segment(index, end1, end2), scanDirection(scanDirection), prevSegment(prevSegment) {}

		activeSegment(int bends, int crossedNets, int index, int end1, int end2, direction scanDirection,
				activeSegment* prevSegment)
			: segment(index, end1, end2),
			  scanDirection(scanDirection),
			  bends(bends),
			  crossedNets(crossedNets),
			  prevSegment(prevSegment) {}
		bool isUpRight() {
			return scanDirection == right || scanDirection == up;
		}

		bool scansVertical() {
			return scanDirection == up || scanDirection == down;
		}
	};

	struct endSegment : segment {
		// Index is distance from the baseSegment index, in the direction of baseSegment->dir
		int length;
		int crossovers;
		activeSegment* baseSegment;
		endSegment(int end1, int end2, activeSegment* baseSegment)
			: segment(0, end1, end2), length(0), crossovers(0), baseSegment(baseSegment){};
		endSegment(int index, int length, int end1, int end2, int crossovers, activeSegment* baseSegment)
			: segment(index, end1, end2), length(length), crossovers(crossovers), baseSegment(baseSegment){};
		int nearIndex() {
			return baseSegment->index + (baseSegment->isUpRight() ? index : -index);
		}
		int farIndex() {
			return baseSegment->index + (baseSegment->isUpRight() ? (index + length) : -(index + length));
		}
	};

	struct obstacleSegmentAscendingComparator {
		bool operator()(const obstacleSegment* lhs, const obstacleSegment* rhs) const {
			// I want to order them using index, but I want them to be equal only if pointers match
			if (lhs->type == obstacleSegment::compare || rhs->type == obstacleSegment::compare)
				return lhs->index < rhs->index;
			else
				return (lhs->index != rhs->index) ? (lhs->index < rhs->index) : (lhs < rhs);
		}
	};

	struct endSegmentLessComparator {
		bool operator()(const endSegment* lhs, const endSegment* rhs) const {
			// I want to order it using left ends, but they may overlap only if we have transparent obstacles (nets)
			// In that case we want to order it according to the distance from base segment
			// Note: two endSegments cannot overlap with the same index
			return (lhs->end1 != rhs->end1) ? (lhs->end1 < rhs->end1) : (lhs->index < rhs->index);
		}
	};

	struct optimumSolution {
		int cost = INT16_MAX;
		intPair optimalPoint = {0, 0};
		activeSegment *a = nullptr, *b = nullptr;
		void clear() {
			cost = INT16_MAX;
			optimalPoint = {0, 0};
			a = nullptr;
			b = nullptr;
		}
	} soln;

	using orderedObstacleSet = std::vector<obstacleSegment*>;
	orderedObstacleSet hObstacleSet, vObstacleSet;
	const coreDesign* core;

	net* currentNet;
	std::unordered_set<activeSegment *> activesA, activesB, inactives;
	std::set<endSegment*, endSegmentLessComparator> E;

	bool hSortNeeded = false, vSortNeeded = false;

	void addObstacleBounding();

	void initNet(splicedTerminal* t0, splicedTerminal* t1);

	void expandNet(splicedTerminal* pTerminal);

	void route();

	routing(coreDesign* core) : core(core) {}

public:
	virtual ~routing();

private:
	void initActives(std::unordered_set<activeSegment*>& activeSet, const splicedTerminal* t);

	bool expandActives(
			std::unordered_set<activeSegment*>& actSegmentSet, std::unordered_set<activeSegment*>& newActiveSegments);

	void reconstructSolution();

	bool straightLine(splicedTerminal* t0, splicedTerminal* t1);

	unsigned int pathLength(activeSegment* actS, int x);

	void addActiveFunction(endSegment* ePrevHighest, endSegment* e, endSegment* eNextHighest, activeSegment* actS,
			std::unordered_set<activeSegment*>& newActiveSegments);
	void newActives(activeSegment* actS, std::unordered_set<activeSegment*>& newActiveSegments);

	void updateSolution(segment s, obstacleSegment* obstacle, activeSegment* actSegment);

	void clearActiveObstacles();

	void clearActiveSet(std::unordered_set<activeSegment*>& set);

	obstacleSegment* addObstacle(
			int index, int end1, int end2, obstacleSegment::obstacleType oType, void* sourcePtr, bool horizontal);

	void sortObstacles();

	obstacleSegment* findObstacle(segment s, bool direction, orderedObstacleSet& obstacleSet);

	bool generateEndSegments(activeSegment* actSegment, orderedObstacleSet& obstacleSet);
};
#endif  // ROUTING_H
