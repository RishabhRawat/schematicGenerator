#include "kernel/yosys.h"
USING_YOSYS_NAMESPACE
struct MyPass : public Pass {
    MyPass() : Pass("schematic_place", "Debug Functionality?") { }
    virtual void execute(std::vector<std::string> args, RTLIL::Design *design)
    {
        log("Arguments to my_cmd:\n");
        for (auto &arg : args)
            log("%s\n", arg.c_str());

        //TODO:Check about selected modules
        vector<Module*> yosysModules = design->modules();

    }
} MyPass;
