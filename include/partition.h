#ifndef PARTITION_H
#define PARTITION_H
#include "common.h"

class partition {
    friend class placement;
private:
    hashlib::pool<box*> partitionBoxes;
public:
    void add(box* b);
};

#endif // PARTITION_H
