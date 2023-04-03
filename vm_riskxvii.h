//Define type

#define STORE 0x3
#define LB 0x0
#define LH 0x1
#define LW 0x2
#define LBU 0x4
#define LHU 0x5

#define I_TYPE 0x13
#define ADDI 0x0
#define SLTI 0x2
#define SLTIU 0x3
#define XORI 0x4
#define ORI 0x5
#define ANDI 0x7

#define S_TYPE 0x23
#define SB 0x0
#define SH 0x1
#define SW 0x2
//remaining willl come later on

#define R_TYPE 0x33
#define ADD_SUB 0
#define ADD 0x0
#define SUB 0x20
#define SLL  0x1
#define SLT  0x2
#define SLTU 0x3
#define XOR  0x4
#define SRL 0x0
#define SHIFT_RIGHT 0x5
#define SRA 0x20
#define OR 0x6
#define AND 0x7

#define TYPE_U 0x37

#define TYPE_SB 0x63
#define BEQ 0x0
#define BNE 0x1
#define BLT 0x4
#define BGE 5
#define BLTU 6
#define BGEU 7

#define TYPE_JALR 0x67
#define TYPE_UJ 0x6f
