#include "vm.h"
#include <math.h>
//virtual routines
void write_character(char i) {
    fprintf(stdout, "%c", i);
}
void write_signed_integer(uint32_t i) {
    fprintf(stdout, "%d", i);
}
void write_unsigned_integer(uint32_t i) {
    fprintf(stdout, "%u", (unsigned int)i);
}
void read_charcater(struct VM *vm, uint32_t address) {
    char c;
    fscanf(stdin, "%c", &c);
    vm->regs[address] = (uint32_t)c;
}
void read_signed_int(struct VM *vm, uint32_t address) {
    int x;
    fscanf(stdin, "%d", &x);
    vm->regs[address] = x;
}
void dump_pc(struct VM *vm) {
    fprintf(stdout, "0x%08x\n", vm->pc);
}
void dump_register_banks(struct VM *vm) {
    fprintf(stdout, "PC = 0x%08x\n", vm->pc);
    for (int i = 0; i < 31; i++) {
        fprintf(stdout, "R[%d] = 0x%08x\n", i, vm->regs[i]);
    }
}
void vm_routines(struct VM *vm, uint32_t address, uint32_t mem_access) {
    switch(address) {
        case CWC:
            vm->wc((char)(vm->regs[mem_access]));
            break;
        case CWSI:
            vm->wsi(vm->regs[mem_access]);
            break;
        case CWUI:
            vm->wui(vm->regs[address]);
            break;
        case CRC:
            vm->rc(vm, mem_access);
            break;
        case CRSI:
            vm->rsi(vm, mem_access);
            break;
        case HALT:
            printf("CPU Halt Requested\n");
            vm->is_running = 0;
            break;
    }
}

void start(struct VM *vm) {
    for (int i = 0; i < 32; i++) {
        vm->regs[i] = 0;
    }
    vm->is_running = 1;
    vm->pc = 0x00;
    vm->wc = &write_character;
    vm->wsi = &write_signed_integer;
    vm->wui = &write_unsigned_integer;
    vm->rc = &read_charcater;
    vm->rsi = &read_signed_int;
    vm->dump_pc = &dump_pc;
    vm->drb = &dump_register_banks;
}

uint32_t fetch(struct VM *vm) {
    return *(uint32_t*)(vm->mem + vm->pc);
}

int opcode(uint32_t i) {
    return (i & (0x7f << 0)) >> 0;
}
int func3(uint32_t i) {
    return (i & (0x7 << 12)) >> 12;
}
int func7(uint32_t i) {
    return (i & (0x1f << 25)) >> 25;
}

int rd(uint32_t i) {
    return (i & (0x1f << 7)) >> 7;
}
int rs1(uint32_t i) {
    return (i & (0x1f << 15)) >> 15;
}
int rs2(uint32_t i) {
    return (i & (0x1f << 20)) >> 20;
}

int imm_I(uint32_t i) {
    uint32_t imm_i = (i & (0xfff << 20)) >> 20;
    imm_i = imm_i & 0xfff;
    if (imm_i & ( 1 << 11)) {
        imm_i |= ~((1 << 11) - 1);
    }
    return imm_i;
}
int imm_S(uint32_t i) {
    uint32_t imm_s = (((i >> 25) & 0x7f) << 5) | ((i >> 7) & 0x1f);
    if (imm_s & (1 << 11)) {
        imm_s |= ~((1 << 11) - 1);
    }
    return imm_s;
}
int imm_U(uint32_t i) {
    uint32_t imm_u = ((i >> 12) & 0xfffff) << 12;
    return imm_u;
}
int imm_SB(uint32_t i) {
    uint32_t imm_sb = (i >> 31) << 12;
    imm_sb |= ((i >> 7) & 0x1) << 11;
    imm_sb |= ((i >> 25) & 0x3f) << 5;
    imm_sb |= ((i >> 8) & 0xf) << 1;
    if (imm_sb & 0x1000) {
        imm_sb |= 0xffffe000;
    }
    return imm_sb;
}
int imm_UJ(uint32_t i) {
    int imm_20 = ((i >> 31) & 0x1) << 20;
    int imm10_1 = ((i >> 21) & 0x3ff) << 1;
    int imm_11 = ((i >> 20) & 0x1) << 11;
    int imm19_12 = ((i >> 12) & 0xff) << 12;
    int imm_uj = imm_20 | imm10_1 | imm_11 | imm19_12;
    imm_uj = (imm_uj << 11) >> 11;
    return imm_uj;
}
//Arithmetic and logic operation
void add(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] + vm->regs[rs2(i)];
}
void addi(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] + imm_I(i);
}
void sub(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] - vm->regs[rs2(i)];
}
void lui(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = imm_U(i);
}
void xor(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] ^ vm->regs[rs2(i)];
}
void xori(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] ^ imm_I(i);
}
void or(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] | vm->regs[rs2(i)];
}
void ori(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] | imm_I(i);
}
void and(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] & vm->regs[rs2(i)];
}
void andi(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] & imm_I(i);
}
void sll(struct VM *vm, uint32_t i) {
    // (uint32_t) (imm_I(inst) & 0x1f)
    vm->regs[rd(i)] = vm->regs[rs1(i)] << vm->regs[rs2(i)];
}
void srl(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] >> vm->regs[rs2(i)];
}
void sra(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = vm->regs[rs1(i)] >> vm->regs[rs2(i)];
}

//Memory access operations
void lb(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address != 0) {
        vm->regs[rd(i)] = (uint32_t)(int8_t)vm->mem[address];
    }
}
void lh(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address != 0) {
            vm->regs[rd(i)] = (uint32_t)(int8_t)((vm->mem[address]) 
                                | ((uint8_t)((vm->mem[address] + 1) << 8)));
    }

}
void lw(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if ((address >= 0x800) && (address <= 0x8ff)) {
        vm_routines(vm, address, rd(i));
    } else {
        if (address != 0) {
                vm->regs[rd(i)] = (uint32_t)(int8_t)((vm->mem[address]) 
                            | ((uint8_t)((vm->mem[address] + 1) << 8))
                            | ((uint8_t)((vm->mem[address] + 2) << 16))
                            | ((uint8_t)((vm->mem[address] + 3) << 24))
                            );
        }

    }
}
void lbu(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address >= 0x800 && (address <= 0x8fff)) {
        vm_routines(vm, address, rd(i));
    } else {
        if (address != 0) {
            vm->regs[rd(i)] = vm->mem[address];
        }
    }
    
}
void lhu(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address != 0) {
        vm->regs[rd(i)] = (vm->mem[address]) | ((uint32_t)((vm->mem[address] + 1) << 8));
    }
    
}
void sb(struct VM *vm, uint32_t i) {
    int imm = imm_S(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if ((address >= 0x800) && (address <= 0x8ff)) {
        vm_routines(vm, address, rs2(i));
    } else {
        vm->mem[address] = (uint8_t)(( vm->regs[rs2(i)]) & 0xff);
    }
}
void sh(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_S(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address >= 0x800 && address <= 0x8ff) {
        vm_routines(vm, address, rs2(i));
    } else {
        vm->mem[address] = (uint8_t)((vm->regs[rs2(i)]) & 0xff);
        vm->mem[address] |= (uint8_t)((((vm->regs[rs2(i)]) & 0xff) >> 8) & 0xff);
    }
    
}
void sw(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_S(i);
    uint32_t address = vm->regs[rs1(i)] + imm;
    if (address >= 0x800 && address <= 0x8ff) {
        vm_routines(vm, address, rs2(i));
    } else if ((address >= 0x400) && (address <= 0x7ff)) {
        vm->mem[address] = (uint32_t)((vm->regs[rs2(i)]) & 0xff);
    }
    
}

//Program flow operations
void slt(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = (vm->regs[rs1(i)] < (uint32_t)vm->regs[rs2(i)]) ? 1 : 0;
}
void slti(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    vm->regs[rd(i)] = (vm->regs[rs1(i)] < imm) ? 1 : 0;
}
void sltu(struct VM *vm, uint32_t i) {
    vm->regs[rd(i)] = (vm->regs[rs1(i) < vm->regs[rs2(i)]]) ? 1 : 0;
}
void sltiu(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    vm->regs[rd(i)] = (vm->regs[rs1(i)] < imm) ? 1 : 0;
}
void beq(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_SB(i);
    if (vm->regs[rs1(i)] == vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void bne(struct VM *vm, uint32_t i) {
    int imm = imm_SB(i);
    if (vm->regs[rs1(i)] != vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void blt(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_SB(i);
    if ((uint32_t)vm->regs[rs1(i)] < (uint32_t)vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void bltu(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_SB(i);
    if (vm->regs[rs1(i)] < vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void bge(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_SB(i);
    if ((uint32_t)vm->regs[rs1(i)] >= (uint32_t)vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void bgeu(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_SB(i);
    if (vm->regs[rs1(i)] != vm->regs[rs2(i)]) {
        vm->pc = vm->pc + imm - 4;
    }
}
void jal(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_UJ(i);
    vm->regs[rd(i)] = vm->pc + 4;
    vm->pc = vm->pc + (imm);
}
void jalr(struct VM *vm, uint32_t i) {
    uint32_t imm = imm_I(i);
    vm->regs[rd(i)] = vm->pc + 4;
    vm->pc = vm->regs[rs1(i)] + imm;
}