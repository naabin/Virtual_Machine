#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INST_MEM_SIZE 1024
#define DATA_MEM_SIZE 1024

uint8_t sext(uint32_t n, int b) {
    return ((n >> (b - 1)) && 1) ?
        (n|(0xFFFF << b)) : n;
}

// static void print_character(char c) {
//     printf("%c", c);
// }
// void print_signed_integer(int c) {
//     printf("%d", c);
// }

// void print_unsigned_integer(uint32_t num) {
//     printf("%08x", num);
// }

// char read_character() {
//     char c = getchar();
//     return c;
// }

// int read_int(int* address) {
//     int x = scanf("%d", address);
//     return x;
// }

// void dump_pc(uint32_t pc) {
//     printf("%08x", pc);
// }



int main(int argc, char* argv[]) {
    // if (argc < 2) {
    //     return 1;
    // }
    FILE *file = fopen("./examples/5_sum/5_sum.mi", "rb");
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char* mem = (char *)(malloc(sizeof(char) * size * 4));
    uint32_t reg[32] = {0};
    uint32_t pc = 0x00;
    fread(mem, sizeof(char), size, file);
    fclose(file);
    
    // unsigned int rd_mask = 0x5f << 7;
    // unsigned int func3_mask = 0x3f << 11; 
    // unsigned int rs1_mask = 0x5f << 15;
    // unsigned int imm_mask = 0xFFF << 20;
    // unsigned int rd = (current_instruction & rd_mask) >> 7;
    // unsigned int func3 = (current_instruction & func3_mask) >> 11;
    // unsigned int rs1 = (current_instruction & rs1_mask) >> 15;
    // unsigned int imm = (current_instruction & imm_mask) >> 20;
    // printf("opcode = %u\n", opcode);
    // printf("rd = %u\n", rd);
    // printf("func3 = %u\n", func3);
    // printf("rs1 = %u\n", rs1);
    // printf("imm=%u\n", imm);
    /*
    alu_op = 
    1. 0000011, 0000011, 0000011, 0000011, 0000011
    2. 0010011, 0010011, 0010011, 0010011, 0010011, 0010011
    3. 0100011, 0100011, 0100011
    4. 0110011, 0110011, 0110011, 0110011, 0110111, 0110011, 0110011, 0110011, 0110011, 0110011, 0110011
    5. 1100011, 1100011, 1100011, 1100011, 1100011, 1100011
    6. 1100111
    7. 1101111
    */
   int running = 1;
   while (running) {
    // printf("PC: %d\n", pc);
    uint32_t current_instruction = *(uint32_t*)(mem + pc);
    // printf("%x\n", current_instruction);
    // for (int i = 31; i >=0; i--) {
    //     printf("%u", (current_instruction >> i) & 1);
    // }
    // printf("\n");
    unsigned int opcode_mask = 0x7f << 0;
    unsigned int opcode = (current_instruction & opcode_mask) >> 0;
    switch(opcode) {
        case 3:
        {
            int func3_mask = 0x7 << 12;
            int rs1_mask = 0x1f << 15;
            int rd_mask = 0x1f << 7;
            // int imm_mask = 0xFFF << 20;
            int rs1 = (current_instruction & rs1_mask) >> 15;
            int func3 = (current_instruction & func3_mask) >> 12;
            int rd = (current_instruction & rd_mask) >> 7;
            int imm = (current_instruction & (0xfff << 20)) >> 20;
            if (imm & (1 << 11)) {
                imm |= ~((1 << 11) - 1);
            }
            // imm = (current_instruction & 0x800) ? imm |= 0xfffff000 : imm;
            switch(func3) {
                case 0:
                    //type I
                    // lb
                    reg[rd] = sext(mem[reg[rs1] + imm], 15);
                    break;
                case 1:
                    //Type I
                    // lh
                    reg[rd] = sext(mem[reg[rs1] + imm], 32);
                    break;
                case 2:
                case 4:
                case 5:
                    //type I
                    //lw, lbu, lhu
                    switch(reg[rs1] + imm) {
                        case 0x816:
                        {
                            
                            int x;
                            scanf("%d", &x);
                            // mem[reg[rs1] + imm] = &x;
                            memcpy(&mem[reg[rs1] + imm], &x, 4);
                            break;
                        }
                    }
                    // reg[rd] = mem[reg[rs1] + imm];
                    memcpy(&reg[rd], &mem[reg[rs1] + imm], 4);
                    break;
                
            }
            pc += 0x4;
            break;
        }
        case 19:
        {
            unsigned int func3_mask = 0x7 << 12;
            int rs1 = (current_instruction & (0x1f << 15)) >> 15;
            int rd = (current_instruction & (0x1f << 7)) >> 7;
            int func3 = (current_instruction & func3_mask) >> 12;
            int imm_i = ((current_instruction & (0xfff << 20)) >> 20);
            imm_i = imm_i & 0xFFF;
            if (imm_i & (1 << 11)) {
                imm_i |= ~((1 << 11) - 1);
            }
            switch(func3) {
                case 0:
                    //type I
                    // addi
                    reg[rd] = reg[rs1] + imm_i;
                    break;
                case 2:
                    //type I
                    // slti
                    reg[rd] = (reg[rs1] < imm_i) ? 1: 0;
                    break;
                case 3:
                    // type I
                    // sltiu
                    reg[rd] = (reg[rs1] < imm_i) ? 1: 0;
                    break;
                case 4:
                    // type I
                    // xori
                    reg[rd] = reg[rs1] ^ imm_i;
                    break;
                case 5:
                    // type I
                    // ori
                    reg[rd] = reg[rs1] | imm_i;
                    break;
                case 7:
                    // type I
                    // andi
                    reg[rd] = reg[rs1] & imm_i;
                    break;
            }
            pc += 0x4;
            break;
        }
        case 35:
        {
            int func3_mask = 0x7 << 12;
            int func3 = (current_instruction & func3_mask) >> 12;
            int rs1_mask = 0x1f << 15;
            int rs1 = (current_instruction & rs1_mask) >> 15;
            int rs2_mask = 0x1f << 20;
            int rs2 = (current_instruction & rs2_mask) >> 20;
            int imm_s = (((current_instruction >> 25) & 0x7f) << 5) | ((current_instruction >> 7) & 0x1f);
            if (imm_s & (1 << 11)) {
                imm_s |= ~((1 << 11) - 1);
            }
            // imm_s = (current_instruction & 0x800000) ? imm_s |= 0xFFFFF000 : imm_s;
            switch(func3) {
                case 0:
                case 1:
                case 2:
                    //type S
                    // sb, sh, sw
                    mem[reg[rs1] + imm_s] = reg[rs2];
                    switch (reg[rs1] + imm_s)
                    {
                    case 0x0800:
                    {
                        char x = mem[reg[rs1] + imm_s];
                        fprintf(stdout, "%c\n", x);
                        break;
                    }
                    case 0x0804:
                    {
                        fprintf(stdout, "%d\n", mem[reg[rs1] + imm_s]);
                        break;
                    }
                    case 0x816:
                    {
                        fprintf(stdout,"%d\n", mem[reg[rs1] + imm_s]);
                        break;
                    }
                    case 0x0812:
                        // char c = read_character();
                        break;
                    case 0x80c:
                        fprintf(stdout, "CPU Halt requested\n");
                        running = 0;
                        break;
                    default:
                        break;
                    }
                    break;
            }
            pc += 0x4;
            break;
        }
        case 51:
        {
            unsigned int func3_mask = 0x7 << 12;
            unsigned int rd = (current_instruction & (0x1f << 7)) >> 7;
            unsigned int rs1 = (current_instruction & (0x1f << 15)) >> 15;
            unsigned int rs2 = (current_instruction & (0x1f << 20)) >> 20;
            unsigned int func3 = (current_instruction & func3_mask) >> 12;
            switch(func3) {
                case 0:
                {
                    // unsigned int func7_mask = 0x7f << 25;
                    unsigned int func_7 = (current_instruction & (0x1f << 25)) >> 25;
                    switch (func_7) {
                        case 0:
                            //type R
                            //add
                            reg[rd] = reg[rs1] + reg[rs2];
                            break;
                        case 32:
                            //type R
                            //sub
                            reg[rd] = reg[rs1] - reg[rs2];
                            break;
                    }   
                    break;
                }
                case 1:
                    // type R
                    // sll
                    reg[rd] = reg[rs1] << reg[rs2];
                    break;
                case 2:
                    //type R
                    //slt
                    reg[rd] = (reg[rs1] < reg[rs2]) ? 1: 0;
                    break;
                case 3:
                    // type R
                    // sltu
                    reg[rd] = (reg[rs1] < reg[rs2]) ? 1: 0;
                    break;
                case 4:
                    //type R
                    //xor
                    reg[rd] = reg[rs1] ^ reg[rs2];
                    break;
                case 5:
                {
                    unsigned int func_7 = (current_instruction & (0x7f << 25)) >> 25;
                    switch (func_7) {
                        case 0:
                            //type R
                            //srl
                            reg[rd] = reg[rs1] >> reg[rs2];
                            break;
                        case 32:
                            //type R
                            //sra
                            reg[rd] = reg[rs1] >> reg[rs2];
                            break;
                    }
                }
                case 6:
                    //type R
                    // Or
                    reg[rd] = reg[rs1] | reg[rs2];
                    break;
                case 7:
                    //type R
                    //and
                    reg[rd] = reg[rs1] & reg[rs2];
                    break;

            }
            pc += 0x4;
            break;
        }
        case 55: 
        {
            //type U
            // lui
            unsigned int rd = (current_instruction & (0x1f << 7)) >> 7;
            unsigned int imm_u = ((current_instruction >> 12) & 0xfffff) << 12;
            reg[rd] = imm_u;
            pc += 0x4;
            break;
        }
        case 99:
        {
             int func3_mask = 0x7 << 12;
             int func3 = (current_instruction & func3_mask) >> 12;
             int rs1 = (current_instruction & (0x1f << 15)) >> 15;
             int rs2 = (current_instruction & (0x1f << 15)) >> 20;
            // imm[12 | 10:5]
            // imm_sb = (current_instruction & 0x800000) ? imm_sb |= 0xFFFFF00 : imm_sb;
            unsigned int imm_sb = (current_instruction >> 31) << 12;
            imm_sb |= ((current_instruction >> 7) & 0x1) << 11;
            imm_sb |= ((current_instruction >> 25) & 0x3f) << 5;
            imm_sb |= ((current_instruction >> 8) & 0xf) << 1;
            
            if (imm_sb & 0x1000) {
                imm_sb |= 0xffffe000;
            }
            switch(func3) {
                case 0:
                    //type SB
                    //beq
                    if (reg[rs1] == reg[rs2]) {
                        pc = pc + (imm_sb << 1);
                        continue;
                    }
                    break;
                case 1:
                    //type SB
                    //bne
                    if (reg[rs1] != reg[rs2]) {
                        pc = pc + (imm_sb << 1);
                    }
                    break;
                case 4:
                    //type SB
                    //blt
                    if (reg[rs1] < reg[rs2]) {
                        pc = pc + (imm_sb << 1);
                    }
                    break;
                case 5:
                    //type SB
                    // bge
                    if (reg[rs1] >= reg[rs2]) {
                        pc = pc + (imm_sb << 1);
                    }
                    break;
                case 6:
                    //type SB
                    //bltu
                    if (reg[rs1] < reg[rs2]) {
                        pc = pc + (imm_sb << 1) ;
                    }
                    break;
                case 7:
                    //type SB
                    // bgeu
                    if (reg[rs1] >= reg[rs2]) {
                        pc = pc + (imm_sb << 1);
                    }
                    break;
                default:
                    pc += 0x4;
            }
            default: 
                pc += 0x4;
                break;
        }
        case 103:
        {
            //type I
            //jalr
            unsigned int rd = (current_instruction & (0x1f << 7)) >> 7;
            unsigned int imm_i = ((current_instruction & (0xfff << 20)) >> 20);
            imm_i = imm_i & 0xFFF;
            unsigned int rs1 = (current_instruction & (0x1f << 15)) >> 15;
            reg[rd] = pc + 4;
            pc = reg[rs1] + imm_i;
            break;
        }
        case 111:
        {
            //type UJ
            //jal
            uint32_t imm_20 = (current_instruction >> 31) & 0x1;
            uint32_t imm10_1 = (current_instruction >> 21) & 0x3ff;
            uint32_t imm_11 = (current_instruction >> 20) & 0x1;
            uint32_t imm19_12 = (current_instruction >> 12) & 0xff;
            uint32_t imm_uj = imm_20 | imm10_1 | imm_11| imm19_12;
            uint32_t rd = (current_instruction & (0x1f << 7)) >> 7; 
            reg[rd] = pc + 4;
            pc = pc + (imm_uj << 1);
            break;
        }
    }
   }
   free(mem);
   return 0;
}
