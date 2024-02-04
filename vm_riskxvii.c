#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "vm_riskxvii.h"
#include "vm.h"

int main(int argc, char* argv[]) {
    struct VM* vm = (struct VM*)malloc(sizeof(struct VM));
    if (argc < 1) {
        return 1;
    }
    FILE * file = fopen(argv[1], "rb");
    // FILE * file = fopen("./examples/add_2_numbers/add_2_numbers.mi", "rb");
    fseek(file, 0L, SEEK_END);
    int size = ftell(file);
    fseek(file, 0L, SEEK_SET);
    char* mem = (char *)(malloc(size + 1));
    fread(mem, sizeof(char), size, file);
    fclose(file);
    memcpy(vm->mem, mem, size * sizeof(char));
    free(mem);

    //Start
    start(vm);
    while(vm->is_running) {
        // vm->dump_pc(vm);
        // printf("PC: %d\n", vm->pc);
        uint32_t instruction = fetch(vm);
        int op = opcode(instruction);
        switch(op) {
            case STORE:
                {
                    uint32_t f3 = func3(instruction);
                    switch(f3) 
                        {
                            case LB:
                                lb(vm, instruction);
                                break;
                            case LH:
                                lh(vm, instruction);
                                break;
                            case LW:
                                lw(vm, instruction);
                                //TODO
                                break;
                            case LBU:
                                lbu(vm, instruction);
                                break;
                            case LHU:
                                lhu(vm, instruction);
                                break;
                        }
                    vm->pc += 0x4;
                    break;
                }
            case I_TYPE:
                {
                    uint32_t f3 = func3(instruction);
                    switch(f3) {
                        case ADDI:
                            addi(vm, instruction);
                            break;
                        case SLTI:
                            slti(vm, instruction);
                            break;
                        case SLTIU:
                            sltiu(vm, instruction);
                            break;
                        case XORI:
                            xori(vm, instruction);
                            break;
                        case ORI:
                            ori(vm, instruction);
                            break;
                        case ANDI:
                            andi(vm, instruction);
                            break;
                    }
                    vm->pc += 0x4;
                    break;
                }
            case S_TYPE:
            {
                uint32_t f3 = func3(instruction);
                switch(f3) {
                    case SB:
                        sb(vm, instruction);
                        break;

                    case SH:
                        sh(vm, instruction);
                        break;
                    case SW:
                            sw(vm, instruction);
                            break;
                        
                }
                vm->pc += 0x4;
                break;
            }
            case R_TYPE:
            {
                uint32_t f3 = func3(instruction);
                switch(f3) {
                    case ADD_SUB:
                        {
                            uint32_t f7 = func7(instruction);
                            switch(f7) {
                                case ADD:
                                    add(vm, instruction);
                                    break;
                                case SUB:
                                    sub(vm, instruction);
                                    break;
                            }
                            break;
                        }
                        break;
                
                    case SLL:
                        sll(vm, instruction);
                        break;
                    case SLT:
                        slt(vm, instruction);
                        break;
                    case SLTU:
                        sltu(vm, instruction);
                        break;
                    case XOR:
                        xor(vm, instruction);
                        break;
                    case SHIFT_RIGHT:
                        {
                            uint32_t f7 = func7(instruction);
                            switch(f7) {
                                case SRL:
                                    srl(vm, instruction);
                                    break;
                                case SRA:
                                    sra(vm, instruction);
                                    break;
                            }
                        }
                    case OR:
                        or(vm, instruction);
                        break;
                    case AND:
                        and(vm, instruction);
                        break;
                }
                vm->pc += 0x4;
                break;
            }
            case TYPE_U:
                lui(vm, instruction);
                vm->pc += 0x4;
                break;

            case TYPE_SB:
                {
                    switch(func3(instruction)) {
                        case BEQ:
                            beq(vm, instruction);
                            break;
                        case BNE:
                            bne(vm, instruction);
                            break;
                        case BLT:
                            blt(vm, instruction);
                            break;
                        case BGE:
                            bge(vm, instruction);
                            break;
                        case BGEU:
                            bgeu(vm, instruction);
                            break;
                    }
                    vm->pc += 0x4;
                    break;
                }
            case TYPE_JALR:
                jalr(vm, instruction);
                break;
            case TYPE_UJ:
                jal(vm, instruction);
                break;
        }
    }
    // vm->drb(vm);
    free(vm);
}
