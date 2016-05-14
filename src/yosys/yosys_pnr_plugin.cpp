#include "kernel/yosys.h"
USING_YOSYS_NAMESPACE
struct MyPass : public Pass {
    MyPass() : Pass("my_plugin", "just a simple test") { }
    virtual void execute(std::vector<std::string> args, RTLIL::Design *design)
    {
        log("Arguments to my_cmd:\n");
        for (auto &arg : args)
            log("%s\n", arg.c_str());
        log("Modules in current design:\n");
        for (auto mod : design->modules())
                        log("%s (%d wires, %d cells)\n", log_id(mod), GetSize(mod->wires()), GetSize(mod->cells()));
//            log("%s \n", log_id(mod));
    }
} MyPass;
