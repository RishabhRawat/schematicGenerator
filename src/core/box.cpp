#include "box.h"
#include "module.h"

void box::add(module *m){
    m->setParentBox(this);
    boxModules.insert(m);
}
