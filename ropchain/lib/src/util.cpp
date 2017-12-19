#include <algorithm>
#include <string>
#include "arch.h"
#include "util.h"

OptROP Util::toOptROP(const OptGadget& gadget) {
    if(gadget.has_value()) {
        return ROPChain(gadget.value());
    }
    return {};
}
Gadgets Util::loadGadgets(const std::string& fileName) {
    //TODO
    return std::vector<Gadget>();
}

std::vector<RegType::Reg> *Util::toBits(const RegSet& s) {
	auto bits = new std::vector<RegType::Reg>();
	for(int i=0; i<s.size(); i++) {
		if(s.test(i)) {
			bits->push_back(i);
		}
	}
	return bits;
}

OptGadget Util::find(const Gadgets& gadgets, const RegSet& avl,
        const Mnem& mnem, const std::optional<Opcode> op1) {
    return find(gadgets, avl, mnem, op1, {}, {});
}
OptGadget Util::find(const Gadgets& gadgets, const RegSet& avl,
        const Mnem& mnem, const std::optional<Opcode> op1,
        const std::optional<Opcode> op2) {
    return find(gadgets, avl, mnem, op1, op2, {});
}
OptGadget Util::find(const Gadgets& gadgets, const RegSet& avl,
        const Mnem& mnem, const std::optional<Opcode> op1,
        const std::optional<Opcode> op2,
        const std::optional<Opcode> op3) {
    auto ops = std::vector<Opcode>();
    if(op1.has_value()) {
        ops.push_back(op1.value());
    }
    if(op2.has_value()) {
        ops.push_back(op2.value());
    }
    if(op3.has_value()) {
        ops.push_back(op3.value());
    }
    const Insn insn = Insn{mnem, ops};
    for(const auto& gadget : gadgets) {
        const auto insn_g = gadget.getInsns()[0];
        if(insn == insn_g && gadget.isAvailable(avl)) {
            return gadget;
        }
    }
    return {};
}

RegType::Reg Util::findRegType(RegType::Reg reg) {
	switch(reg) {
	case RegType::rax: case RegType::eax:
	case RegType::ax: case RegType::ah: case RegType::al:
		return RegType::rax;

	case RegType::rbx: case RegType::ebx:
	case RegType::bx: case RegType::bh: case RegType::bl:
		return RegType::rbx;

	case RegType::rcx: case RegType::ecx:
	case RegType::cx: case RegType::ch: case RegType::cl:
		return RegType::rcx;

	case RegType::rdx: case RegType::edx:
	case RegType::dx: case RegType::dh: case RegType::dl:
		return RegType::rdx;

	case RegType::rdi: case RegType::edi:
	case RegType::di: case RegType::dil:
		return RegType::rdi;

	case RegType::rsi: case RegType::esi:
	case RegType::si: case RegType::sil:
		return RegType::rdi;

	case RegType::rbp: case RegType::ebp:
	case RegType::bp: case RegType::bpl:
		return RegType::rbp;

	case RegType::rsp: case RegType::esp:
	case RegType::sp: case RegType::spl:
		return RegType::rsp;

	case RegType::r8: case RegType::r8d:
	case RegType::r8w: case RegType::r8b:
		return RegType::r8;

	case RegType::r9: case RegType::r9d:
	case RegType::r9w: case RegType::r9b:
		return RegType::r9;

	case RegType::r10: case RegType::r10d:
	case RegType::r10w: case RegType::r10b:
		return RegType::r10;

	case RegType::r11: case RegType::r11d:
	case RegType::r11w: case RegType::r11b:
		return RegType::r11;

	case RegType::r12: case RegType::r12d:
	case RegType::r12w: case RegType::r12b:
		return RegType::r12;

	case RegType::r13: case RegType::r13d:
	case RegType::r13w: case RegType::r13b:
		return RegType::r13;

	case RegType::r14: case RegType::r14d:
	case RegType::r14w: case RegType::r14b:
		return RegType::r14;

	case RegType::r15: case RegType::r15d:
	case RegType::r15w: case RegType::r15b:
		return RegType::r15;
	}
    return RegType::none;
}

//FIXME This might be buggy in some type of insn.
RegSet Util::listChangedRegs(const Insns& insns) {
	RegSet regs;
	for(const auto& insn : insns) {
		if(insn.ops.size() > 0) {
			//FIXME avoid std:get
			regs.set(std::get<RegType::Reg>(insn.ops[0]));
		}
		if(insn.mnem == "xchg") {
			//FIXME avoid std:get
			regs.set(std::get<RegType::Reg>(insn.ops[0]));
			regs.set(std::get<RegType::Reg>(insn.ops[1]));
		}
	}
	return regs;
}

//internal function of Util::calcUseStack
size_t _calcUseStack(const Insn& insn) {
	auto mnem = insn.mnem;
	auto ops = insn.ops;
	if(mnem == "pop") {
		return Arch::word();
	} else if(mnem == "popad") {
		return Arch::word() * 7;
	} else if(mnem == "add") {
		//FIXME use std::visit
		auto r = std::get<RegType::Reg>(ops[0]);
		if(r == RegType::esp || r == RegType::rsp) {
			auto s = std::get<uint64_t>(ops[1]);
			size_t sz = 2;
			return Arch::word() * std::stoll(RegType::toString(s), &sz, 16);
		}
	}
	return 0;
}
size_t Util::calcUseStack(const Insns& insns) {
	size_t ret = 0;
	for(auto& insn : insns) {
		ret += _calcUseStack(insn);
	}
	return ret;

}

RegSet Util::allRegs() {
	RegSet s;
	if(Arch::arch == Arch::X86) {
		s.set(RegType::eax
				| RegType::ebx
				| RegType::ecx
				| RegType::edx
				| RegType::esi
				| RegType::edi
				| RegType::ebp);
		return s;
	}
	if(Arch::arch == Arch::AMD64) {
		s.set(RegType::rax
				| RegType::rbx
				| RegType::rcx
				| RegType::rdx
				| RegType::rdi
				| RegType::rsi
				| RegType::rbp
				| RegType::rsp
				| RegType::r8
				| RegType::r9
				| RegType::r10
				| RegType::r11
				| RegType::r12
				| RegType::r13
				| RegType::r14
				| RegType::r15);
		return s;
	}
	return RegSet();
}

std::string Util::pack(uint64_t v) {
	std::string ret;
	int bytes;
	if(Arch::arch == Arch::X86) {
		bytes = 4;
	} else if(Arch::arch == Arch::AMD64) {
		bytes = 8;
	} else {
		//error
		return "";
	}
	for(int i=bytes-1; i>=0; i--) {
		ret += (char)((v >> 8*i) & 0xff);
	}
	return ret;
}
