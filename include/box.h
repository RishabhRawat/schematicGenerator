#ifndef BOX_H
#define BOX_H
#include "common.h"

class box {
    friend class partition;
private:
    partition *parentPartition;
    hashlib::pool<module*> boxModules;
public:
    void add(module *m);
    void setParentPartition(partition *p){parentPartition=p;}
};

#endif // BOX_H
