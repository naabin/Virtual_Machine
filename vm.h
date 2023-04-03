#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DM_START 0x400
#define CWC 0x800 // console write character
#define CWSI 0x804  // console wirte signed integer
#define CWUI 0x808 // console write unsigned integer
#define CRC 0x812 // console read character
#define CRSI 0x816 // console read signed integer
#define DP 0x820 // DUMP PC
#define DRB // DUMP register Banks
#define HB 0x830, 0x834 // HEAP BANKS
#define R 0x850 // Reserved
#define HALT 0x80C // HALT



struct VM {
    char mem[2048];
    uint32_t regs[32];
    uint32_t pc;
    int is_running;
    void (*wc)(char s);
    void (*wsi)(uint32_t s);
    void (*wui)(uint32_t s);
    void (*rc)(struct VM *vm, uint32_t instruction);
    void (*rsi)(struct VM *vm, uint32_t instruction);
    void (*dump_pc)(struct VM *vm);
    void (*drb)(struct VM *vm);
};

void start(struct VM *vm);
// Memory I/O
uint32_t load_memory(char *mem, uint32_t addr);

//Fetch current instructions
uint32_t fetch(struct VM *vm);
// execute instruction
int execute(struct VM * vm, uint32_t instruction);
// dump registers
void dump(struct VM * vm);

int func3(uint32_t i);
int func7(uint32_t i);
int opcode(uint32_t i);
int rd(uint32_t i);
int rs1(uint32_t i);
int rs2(uint32_t i);
int imm_I(uint32_t i);
int imm_S(uint32_t i);
int imm_U(uint32_t i);
int imm_SB(uint32_t i);
int imm_UJ(uint32_t i);

//Arithmetic and logic operation
void add(struct VM *vm, uint32_t i);
void addi(struct VM *vm, uint32_t i);
void sub(struct VM *vm, uint32_t i);
void lui(struct VM *vm, uint32_t i);
void xor(struct VM *vm, uint32_t i);
void xori(struct VM *vm, uint32_t i);
void or(struct VM *vm, uint32_t i);
void ori(struct VM *vm, uint32_t i);
void and(struct VM *vm, uint32_t i);
void andi(struct VM *vm, uint32_t i);
void sll(struct VM *vm, uint32_t i);
void srl(struct VM *vm, uint32_t i);
void sra(struct VM *vm, uint32_t i);

//Memory access operations
void lb(struct VM *vm, uint32_t i);
void lh(struct VM *vm, uint32_t i);
void lw(struct VM *vm, uint32_t i);
void lbu(struct VM *vm, uint32_t i);
void lhu(struct VM *vm, uint32_t i);
void sb(struct VM *vm, uint32_t i);
void sh(struct VM *vm, uint32_t i);
void sw(struct VM *vm, uint32_t i);

//Program flow operations
void slt(struct VM *vm, uint32_t i);
void slti(struct VM *vm, uint32_t i);
void sltu(struct VM *vm, uint32_t i);
void sltiu(struct VM *vm, uint32_t i);
void beq(struct VM *vm, uint32_t i);
void bne(struct VM *vm, uint32_t i);
void blt(struct VM *vm, uint32_t i);
void bltu(struct VM *vm, uint32_t i);

void bge(struct VM *vm, uint32_t i);
void bgeu(struct VM *vm, uint32_t i);
void jal(struct VM *vm, uint32_t i);
void jalr(struct VM *vm, uint32_t i);

//Virtual routines

void write_character(char s);
void write_signed_integer(uint32_t s);
void write_unsigned_integer(uint32_t s);
void read_charcater(struct VM *vm, uint32_t address);
void read_signed_int(struct VM *vm, uint32_t address);
void dump_pc(struct VM *vm);
void dump_register_banks(struct VM *vm);

void vm_routines(struct VM *vm, uint32_t address, uint32_t mem_access);