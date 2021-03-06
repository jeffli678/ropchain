#pragma once
#include "../util.h"
#include "../ropchain.h"
#include "../regs.h"
namespace Shl {
    OptROP find(const Operand& op1, const Operand& op2,
            const Gadgets& gadgets, RegSet& aval);
};
