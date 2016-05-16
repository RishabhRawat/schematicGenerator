#include "partition.h"
#include "box.h"

void partition::add(box *b){
    b->setParentPartition(this);
    partitionBoxes.insert(b);
}
