#include "xchg.h"

OptROP Xchg::find(const Opcode& op1, const Opcode& op2,
        const uint64_t dest, const Gadgets& gadgets, RegSet& aval) {
    auto gadget = GadgetUtil::find(gadgets, aval, "xchg", op1, op2);
    return GadgetUtil::toOptROP(gadget);
}
