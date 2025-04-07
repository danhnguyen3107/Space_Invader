#include <cstdint>

#include "include/I8080.h"
#include <iostream>
#include <cstdint>
#include <cstring>
#include <fstream>

I8080::I8080() {
    A = B = C = D = E = H = L = 0;
    PC = 0; // Initialize PC to 0
    SP = 0;
    carry = zero = sign = parity = aux_carry = false;
    cycles = 0;
    exit = halt = false;
    interruptsEnabled = false;

    // InPort[0] = 0b10001111;
    InPort[0] |= 1 << 1;
	InPort[0] |= 1 << 2;
	InPort[0] |= 1 << 3;
    InPort[1] = 0b10001000;
    InPort[2] = 0b10001011;

    OutPort[3] = 0b00000000;

    
}

I8080::~I8080() {

}


// Load Rom
void I8080::loadROM(const char* path, uint16_t startAddress) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open ROM file: " << path << std::endl;
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg); // Move to the beginning of the file

    if (size <= 0) {
        std::cerr << "ROM file is empty or an error occurred: " << path << std::endl;
        return;
    }

    // Check if size fits in the emulator's memory array.
    if (size > MEMORY_SIZE) {
        std::cerr << "ROM file size (" << size << " bytes) exceeds available memory." << std::endl;
        return;
    }

    char* buffer = new char[size];

    // Print buffer
    // for (int i = 0; i < size; i++) {
    //     std::cout << std::hex << static_cast<int>(buffer[i]) << " ";
    // }
    // std::cout << std::endl;

    if (!file.read(buffer, size)) {
        std::cerr << "Failed to read ROM file: " << path << std::endl;
        delete[] buffer;
        return;
    }
    file.close();

    for (int i = 0; i < size; i++) {
        memory[startAddress + i] = buffer[i];
    }

    delete[] buffer;
    
}

void I8080::Cycle(uint32_t num_cycles){
    cycles = 0;
    while(cycles < num_cycles) {
        RegularInstruction();
    }
}

void I8080::RegularInstruction() {
    // Check if the program counter (PC) is within the valid range of memory
    if (PC > 0x3fff) {
        std::cerr << "Program Counter out of bounds: " << PC << std::endl;
        halt = true; // Set exit flag to true
        return;
    }

    // Fetch the opcode from memory at the current program counter (PC)
    uint8_t opcode = memory[PC];
    executeInstruction(opcode);
}


// Set Flags
// void I8080::setFlagsArith(uint16_t result) {
//     setFlagZero(result);
//     setFlagSign(result);
//     setFlagParity(result);
//     setFlagAuxCarry(result);
//     setFlagCarry(result);
// }

void I8080::setFlagZeroArith(uint16_t value) {
    zero = ((value & 0xFF) == 0);
}

void I8080::setFlagSignArith(uint16_t value) {
    sign = (0x80 == (value & 0x80));
}

void I8080::setFlagParityArith(uint16_t value) {
    uint8_t res = value & 0xFF;
    int p = 0;
    while (res){
        p += res & 1;
        res >>= 1;
    }
    parity = ((p  % 2) == 0);
    
}
void I8080::setFlagAuxCarryArith(uint8_t num1, uint8_t num2, int operation) {
    // if operation is addition
    switch (operation){
    case 0: // ADD
        aux_carry = ((num1 & 0x0F) + (num2 & 0x0F)) > 0x0F;
        break;
    case 1: // MULT
        aux_carry = ((num1 & 0x0F) * (num2 & 0x0F)) > 0x0F;
        break;
    case 2: // SUB
        aux_carry = ((num1 & 0x0F) < (num2 & 0x0F)) ;
        break;
    // case 3: // DIV
    //     aux_carry = ((num1 & 0x0F) / (num2 & 0x0F)) > 0;
    //     break;
    default:
        break;
    }
    
    

}

void I8080::setFlagAuxCarryArithCarry(uint8_t num1, uint8_t num2, uint8_t carry, int operation) {
    // if operation is addition
    switch (operation){
    case 0: // ADD
        aux_carry = ((num1 & 0x0F) + (num2 & 0x0F)) + carry > 0x0F;
        break;
    case 1: // MULT
        aux_carry = ((num1 & 0x0F) * (num2 & 0x0F)) + carry > 0x0F;
        break;
    case 2: // SUB
        aux_carry = ((num1 & 0x0F) < (num2 & 0x0F)) ;
        break;
    // case 3: // DIV
    //     aux_carry = ((num1 & 0x0F) / (num2 & 0x0F)) > 0;
    //     break;
    default:
        break;
    }
    
    

}

void I8080::setFlagCarryArith(uint16_t value) {
    carry = (value >> 8) != 0;
}


void I8080::pushStackCall() {
    uint16_t ret = PC + 3;
    memory[SP - 1] = (uint8_t)(ret >> 8) & 0xff;
    memory[SP - 2] = (uint8_t)(ret & 0xFF);
    SP -= 2;
}

void I8080::popStackCall() {
    
    PC = (memory[SP + 1] << 8) | memory[SP];
    SP += 2;
}
//-----------------------------

// Instructions Carry Bits

void I8080::INT_CMC() {
    carry = !carry;
    PC++;
    cycles += 4; 
}

void I8080::INT_STC() {
    carry = true;
    PC++;
    cycles += 4;
}

// SINGLE REGISTER INSTRUCTIONS

void I8080::INT_INR(uint8_t* addr) {
    uint16_t temp = *addr;
    uint16_t res = temp + 1;
    *addr += 1;

    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, 1, 0);
    // setFlagCarryArith(res);

    PC++;
    cycles += 4;
    
}

void I8080::INT_INR_MEM(uint8_t* addr) {
    uint16_t temp = *addr;
    uint16_t res = temp + 1;
    *addr += 1;

    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, 1, 0);
    // setFlagCarryArith(res);

    PC++;
    cycles += 10;
    
}


void I8080::INT_DCR(uint8_t* addr) {
    uint16_t temp = *addr;
    uint16_t res = temp - 1;
    *addr -= 1;

    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, 1, 2);
    // setFlagCarryArith(res);

    PC++;
    cycles += 4;
    
}

void I8080::INT_DCR_MEM(uint8_t* addr) {
    uint16_t temp = *addr;
    uint16_t res = temp - 1;
    *addr -= 1;

    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, 1, 2);
    // setFlagCarryArith(res);

    PC++;
    cycles += 10;
    
}

void I8080::INT_CMA() {
    A = ~A;
    PC++;
    cycles += 4;
}


void I8080::INT_DAA() {
    uint16_t temp = A;

    if ((temp & 0x0F) > 9 || aux_carry){
        temp += 0x06;
        setFlagAuxCarryArith(A, 0x06, 0);
    }
    if ((temp & 0xF0) > 0x90 || carry){

        temp += 0x60;
        setFlagCarryArith(temp);
       
    }
   
    setFlagZeroArith(temp);
    setFlagSignArith(temp);
    setFlagParityArith(temp);
    

    A = (temp & 0xFF);


    PC++;
    cycles += 4;
}

// NOP INSTRUCTIONS
void I8080::INT_NOP() {
    PC += 1;
    cycles += 4;
}

// DATA TRANSFER INSTRUCTIONS

void I8080::INT_MOV_REG(uint8_t* dest, uint8_t* src) {
    *dest = *src;
    PC++;
    cycles += 5;
}

void I8080::INT_MOV_MEM(uint8_t* addr, bool isSrc) {
    HL = (H << 8) | L;
    if (isSrc){
        
        *addr = memory[HL];

    } else {
        memory[HL] = *addr;
    }
    

    PC++;
    cycles += 7;
}



void I8080::INT_STAX(bool isBC) {
    if (isBC){
        BC = (B << 8) | C;
        memory[BC] = A;
    } else {
        DE = (D << 8) | E;
        memory[DE] = A;
    }
    PC++;
    cycles += 7;
}


void I8080::INT_LDAX(bool isBC) {
    if (isBC){
        BC = (B << 8) | C;
        A = memory[BC];
    } else {
        DE = (D << 8) | E;
        A = memory[DE];
    }
    PC++;
    cycles += 7;

}

// REGISTER OR MEMORY TO ACCUMULATOR INSTRUCTIONS

void I8080::INT_ADD(uint8_t* addr) {
    uint16_t temp = A;
    uint16_t res = temp + *addr;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, *addr, 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 4;
}


void I8080::INT_ADD_MEM() {
    HL = (H << 8) | L;
    uint16_t temp = A;
    uint16_t res = temp + memory[HL];
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, memory[HL], 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 7;
}

void I8080::INT_ADC(uint8_t* addr) {
    uint16_t temp = A;
    uint16_t res = temp + *addr + (uint8_t)carry;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArithCarry(temp, *addr, carry, 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 4;
}

void I8080::INT_ADC_MEM() {
    HL = (H << 8) | L;
    uint16_t temp = A;
    uint16_t res = temp + memory[HL] + carry;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArithCarry(temp, memory[HL], carry, 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 7;
}

void I8080::INT_SUB(uint8_t* addr) {
    uint16_t temp = A;
    uint16_t res = temp - *addr;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -(*addr), 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 4;
}

void I8080::INT_SUB_MEM() {
    HL = (H << 8) | L;
    uint16_t temp = A;
    uint16_t res = temp - memory[HL];
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -memory[HL], 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 7;
}

void I8080::INT_SBB(uint8_t* addr) {
    uint16_t temp = A;
    uint8_t value = *addr + (uint8_t)carry;
    uint16_t res = temp - value;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    // setFlagAuxCarryArithCarry(temp, -(*addr), carry, 0);
    setFlagAuxCarryArith(temp, -value, 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 4;

}

void I8080::INT_SBB_MEM() {
    HL = (H << 8) | L;
    uint16_t temp = A;
    uint8_t value = memory[HL] + (uint8_t)carry;
    uint16_t res = temp - value;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    // setFlagAuxCarryArithCarry(temp, -(memory[HL]), carry, 0);
    setFlagAuxCarryArith(temp, -value, 0);
    setFlagCarryArith(res);

    PC++;
    cycles += 7;
}

void I8080::INT_ANA(uint8_t* addr) {
    A &= *addr;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    // aux_carry = true;
    
    PC++;
    cycles += 4;
    
}

void I8080::INT_ANA_MEM() {
    HL = (H << 8) | L;
    A &= memory[HL];
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    // aux_carry = true;
    
    PC++;
    cycles += 7;
}

void I8080::INT_XRA(uint8_t* addr) {
    A ^= *addr;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    aux_carry = false;
    
    PC++;
    cycles += 4;
}

void I8080::INT_XRA_MEM() {
    HL = (H << 8) | L;
    A ^= memory[HL];
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    aux_carry = false;
    
    PC++;
    cycles += 7;
}

void I8080::INT_ORA(uint8_t* addr) {
    A |= *addr;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    aux_carry = false;
    
    PC++;
    cycles += 4;
}

void I8080::INT_ORA_MEM() {
    HL = (H << 8) | L;
    A |= memory[HL];
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    carry = false;
    aux_carry = false;
    
    PC++;
    cycles += 7;
}

void I8080::INT_CMP(uint8_t* addr) {
    uint16_t temp = A;
    uint16_t res = temp - *addr;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -(*addr), 0);
    setFlagCarryArith(res);
    
    PC++;
    cycles += 4;
}

void I8080::INT_CMP_MEM() {
    HL = (H << 8) | L;
    uint16_t temp = A;
    uint16_t res = temp - memory[HL];
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -memory[HL], 0);
    setFlagCarryArith(res);
    
    PC++;
    cycles += 7;
}

// ROTATE ACCUMULATOR INSTRUCTIONS

void I8080::INT_RLC() {
    carry = ((A & 0x80) != 0);
    A = (A << 1) | carry;
    PC++;
    cycles += 4;
}

void I8080::INT_RRC() {
    carry = (A & 0x01) != 0;
    A = (A >> 1) | (carry << 7);
    PC++;
    cycles += 4;
}

void I8080::INT_RAL() {
    uint8_t temp = carry;
    carry = (A & 0x80) != 0;
    A = (A << 1) | temp;
    PC++;
    cycles += 4;
}

void I8080::INT_RAR() {
    uint8_t temp = carry;
    carry = (A & 0x01) != 0;
    A = (A >> 1) | (temp << 7);
    PC++;
    cycles += 4;
}

// REGISTER PAl R INSTRUCTIONS

void I8080::INT_PUSH(uint8_t* highReg, uint8_t* lowReg) {
 
    memory[SP - 1] = *highReg;
    memory[SP - 2] = *lowReg;
    SP -= 2;
    PC++;
    cycles += 11;
}

void  I8080::INT_PUSH_PSW() {
    memory[SP - 1] = A;
    memory[SP - 2] = (sign << 7) | (zero << 6)  | (aux_carry << 4) | (parity << 2 ) |  (0x02)  | (carry & 0x01);
    SP -= 2;
    PC++;
    cycles += 11;
}

void I8080::INT_POP(uint8_t* highReg, uint8_t* lowReg) {
    *highReg = memory[SP + 1];
    *lowReg = memory[SP];
    SP += 2;
    PC++;
    cycles += 10;
}

void I8080::INT_POP_PSW() {
    uint8_t temp = memory[SP];
    sign = (temp & 0x80) != 0;
    zero = (temp & 0x40) != 0;
    aux_carry = (temp & 0x10) != 0;
    parity = (temp & 0x04) != 0;
    carry = (temp & 0x01) != 0;

    A = memory[SP + 1];

    SP += 2;
    PC++;
    cycles += 10;
}

void I8080::INT_DAD(uint16_t* reg ){

    HL = (H << 8) | L;
    uint32_t temp = HL + *reg;

    carry = (temp & 0xFFFF0000) > 0;

    H = (temp & 0xFF00) >> 8;
    L = temp & 0xFF;

    PC++;
    cycles += 10;

}

void I8080::INT_INX(uint8_t* highReg, uint8_t* lowReg) {

    uint16_t temp = (*highReg << 8) | *lowReg;
    temp += 1;
    *highReg = (temp & 0xFF00) >> 8;
    *lowReg = temp & 0xFF;

    PC++;
    cycles += 5;

}

void  I8080::INT_INX_SP() {
    SP += 1;
    PC++;
    cycles += 5;
}

void I8080::INT_DCX(uint8_t* highReg, uint8_t* lowReg) {

    uint16_t temp = (*highReg << 8) | *lowReg;    
    temp -= 1;
    *highReg = (temp & 0xFF00) >> 8;
    *lowReg = temp & 0xFF;

    PC++;
    cycles += 5;
}

void I8080::INT_DCX_SP() {
    SP -= 1;
    PC++;
    cycles += 5;
}

void I8080::INT_XCHG() {
    uint8_t temp = D;
    D = H;
    H = temp;

    temp = E;
    E = L;
    L = temp;

    PC++;
    cycles += 5;
}

void I8080::INT_XTHL() {
    uint8_t temp = L;
    L = memory[SP];
    memory[SP] = temp;

    temp = H;
    H = memory[SP + 1];
    memory[SP + 1] = temp;

    PC++;
    cycles += 18;
}

void I8080::INT_SPHL() {
    SP = (H << 8) | L;
    PC++;
    cycles += 5;
}

// IMMEDIATE INSTRUCTIONS

void I8080::INT_LXI(uint8_t* reg1, uint8_t* reg2, uint8_t high, uint8_t low) {
    *reg1 = high;
    *reg2 = low;
    
    PC += 3;
    cycles += 10;
}

void I8080::INT_LXI_SP(uint8_t high, uint8_t low) {
    SP = (high << 8) | low;
    PC += 3;
    cycles += 10;
}

void I8080::INT_MVI(uint8_t* dest, uint8_t value) {
    *dest = value;
    PC += 2;
    cycles += 7;
}
void I8080::INT_MVI_MEM(uint8_t* dest, uint8_t value) {
    *dest = value;
    PC += 2;
    cycles += 10;
}

void I8080::INT_ADI(uint8_t value) {
    uint16_t temp = A;
    uint16_t res = temp + value;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, value, 0);
    setFlagCarryArith(res);

    PC += 2;
    cycles += 7;
}

void I8080::INT_ACI(uint8_t value) {
    uint16_t temp = A;
    uint16_t res = temp + value + (uint8_t)carry;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArithCarry(temp, value, carry, 0);
    setFlagCarryArith(res);

    PC += 2;
    cycles += 7;
}


void  I8080::INT_SUI(uint8_t value) {
    uint16_t temp = A;
    uint16_t res = temp - value;
    A = res & 0xFF;
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -value, 0);
    setFlagCarryArith(res);

    PC += 2;
    cycles += 7;
}

void I8080::INT_SBI(uint8_t value) {
    uint16_t temp = A;
    uint16_t res = temp - (value + (uint8_t)carry);
    A = res & 0xFF;

    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArithCarry(temp, -value, carry, 0);
    setFlagCarryArith(res);

    PC += 2;
    cycles += 7;
}

void I8080::INT_ANI(uint8_t value) {
    A = A & value;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    // aux_carry = true;
    carry = false;

    PC += 2;
    cycles += 7;
}

void I8080::INT_XRI(uint8_t value) {
    A = A ^ value;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    // aux_carry = true;
    carry = false;

    PC += 2;
    cycles += 7;
}

void I8080::INT_ORI(uint8_t value) {
    A = A | value;
    setFlagZeroArith(A);
    setFlagSignArith(A);
    setFlagParityArith(A);
    // aux_carry = true;
    carry = false;

    PC += 2;
    cycles += 7;
}

void I8080::INT_CPI(uint8_t value) {
    uint16_t temp = A;
    uint16_t res = temp - value;
    
    setFlagZeroArith(res);
    setFlagSignArith(res);
    setFlagParityArith(res);
    setFlagAuxCarryArith(temp, -value, 0);
    setFlagCarryArith(res);

    PC += 2;
    cycles += 7;
}

// DIRECT ADDRESSING INSTRUCTIONS

void I8080::INT_STA(uint16_t addr) {
    memory[addr] = A;
    PC += 3;
    cycles += 13;
}

void I8080::INT_LDA(uint16_t addr) {
    A = memory[addr];
    PC += 3;
    cycles += 13;
}

void I8080::INT_SHLD(uint16_t addr) {
    memory[addr] = L;
    memory[addr + 1] = H;
    PC += 3;
    cycles += 16;
}

void I8080::INT_LHLD(uint16_t addr) {
    L = memory[addr];
    H = memory[addr + 1];
    PC += 3;
    cycles += 13;
}

// JUMP INSTRUCTIONS

void I8080::INT_PCHL() {
    PC = (H << 8) | L;
    cycles += 5;
}

void I8080::INT_JMP(uint16_t addr) {
    PC = addr;
    cycles += 10;
}

void I8080::INT_JC(uint16_t addr) {
    if (carry) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
    
}

void I8080::INT_JNC(uint16_t addr) {
    if (!carry) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JZ(uint16_t addr) {
    if (zero) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JNZ(uint16_t addr) {
    if (!zero) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JM(uint16_t addr) {
    if (sign) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JP(uint16_t addr) {
    if (!sign) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JPE(uint16_t addr) {
    if (parity) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

void I8080::INT_JPO(uint16_t addr) {
    if (!parity) INT_JMP(addr);
    else{
        PC += 3;
        cycles += 10;
    }
}

// CALL SUBROUTINE INSTRUCTIONS

void I8080::INT_CALL(uint16_t addr, bool cond) {
    if (cond){
        pushStackCall();
        PC = addr;
        cycles += 17;
    }
    else{
        PC += 3;
        cycles += 11;
    }
    

}

void I8080::INT_CC(uint16_t addr) {
    INT_CALL(addr, carry);
    
}

void I8080::INT_CNC(uint16_t addr) {
    INT_CALL(addr, !carry);
}

void I8080::INT_CZ(uint16_t addr) {
    INT_CALL(addr, zero);
}

void I8080::INT_CNZ(uint16_t addr) {
    INT_CALL(addr, !zero);
}

void I8080::INT_CM(uint16_t addr) {
    INT_CALL(addr, sign);
}

void I8080::INT_CP(uint16_t addr) {
    INT_CALL(addr, !sign);
}

void I8080::INT_CPE(uint16_t addr) {
    INT_CALL(addr, parity);
}

void I8080::INT_CPO(uint16_t addr) {
    INT_CALL(addr, !parity);
}


// RETURN FROM SUBROUTINE INSTRUCTIONS

void I8080::INT_RET( bool cond ) {
    if (cond){
        popStackCall();
        cycles += 10;
    }
    else{
        PC += 1;
        cycles += 5;
    }
    
}

void I8080::INT_RC() {
    INT_RET(carry);
    if (carry) cycles += 1;
    
}

void I8080::INT_RNC() {
    INT_RET(!carry);
    if (!carry) cycles += 1;
}

void I8080::INT_RZ() {
    INT_RET(zero);
    if (zero) cycles += 1;
}

void I8080::INT_RNZ() {
    INT_RET(!zero);
    if (!zero) cycles += 1;
}

void I8080::INT_RM() {
    INT_RET(sign);
    if (sign) cycles += 1;

}

void I8080::INT_RP() {
    INT_RET(!sign);
    if (!sign) cycles += 1;
}

void I8080::INT_RPE() {
    INT_RET(parity);
    if (parity) cycles += 1;
}

void I8080::INT_RPO() {
    INT_RET(!parity);
    if (!parity) cycles += 1;
}

// RST INSTRUCTION

void I8080::INT_RST(int index) {
    uint16_t ret = PC;
    memory[SP - 1] = (ret >> 8) & 0xff;
    memory[SP - 2] = (ret & 0xff);
    SP -= 2;
    PC = 8 * index;
    cycles += 11;
}

// INTERRUPT FLIP-FLOP INSTRUCTIONS

void I8080::INT_EI() {
    interruptsEnabled = true;
    PC++;
    cycles += 4;
}

void I8080::INT_DI() {
    interruptsEnabled = false;
    PC++;
    cycles += 4;
}

// Input/Output INSTRUCTIONS

void I8080::INT_IN( ) {
    uint8_t port = memory[PC + 1];
    if (port == 3) {
        A = (shift_register >> (8 - shift_offset)) & 0xFF;
    }
    else if (port == 2){
        A = InPort[port];
    }
    else if (port == 1){
        A = InPort[port];
    }
    PC += 2;
    cycles += 10;
}

void I8080::INT_OUT() {
    uint8_t port = memory[PC + 1];
    if (port == 2) {
       
        shift_offset = A & 0x07;
    }
    else if (port == 4){
        shift_register = (shift_register >> 8) | (A << 8);
    }
    else if (port == 3){
        OutPort[port] = A;
    }
    // incomplete
    PC += 2;
    cycles += 10;
}

// HALT INSTRUCTION

void I8080::INT_HLT() {
    halt = true;
    PC++;
    cycles += 4;
}


// Interrupts
void I8080::Interrupts(uint8_t opcode) {
    if (interruptsEnabled) {

        // std::cout << "Interrupts enabled, handling interrupts..." << std::endl;
        interruptsEnabled = false;
        executeInstruction(opcode);
    }
}

void I8080::executeInstruction(uint8_t opcode) {
    
    uint8_t data1 = memory[PC + 1];
    uint8_t data2 = memory[PC + 2];

    switch (opcode) {
        case 0x00: 
            INT_NOP();
            std::cout << "NOP" << std::endl;
            break;
        case 0x01: 
            INT_LXI(&B, &C, data2, data1); 
            std::cout << "LXI BC" << std::endl;
            break; // LXI BC
        case 0x02: INT_STAX(true); break; // STAX BC
        case 0x03: INT_INX(&B, &C); break; // INX BC
        case 0x04: INT_INR(&B); break; // INR B
        case 0x05: INT_DCR(&B); break; // DCR B
        case 0x06: INT_MVI(&B, data1); break; // MVI B, d8
        case 0x07: INT_RLC(); break; // RLC
        /*   0x08: - */
        case 0x09:
            BC = (B << 8) | C;
            INT_DAD(&BC); 
            break; // DAD BC
        case 0x0A: INT_LDAX(true); break; // LDAX BC
        case 0x0B: INT_DCX(&B, &C); break; // DCX BC
        case 0x0C: INT_INR(&C); break; // INR C
        case 0x0D: INT_DCR(&C); break; // DCR C
        case 0x0E: INT_MVI(&C, data1); break; // MVI C, d8
        case 0x0F: INT_RRC(); break; // RRC
        /*   0x10: - */
        case 0x11: INT_LXI(&D, &E, data2, data1); break; // LXI DE
        case 0x12: INT_STAX(false); break; // STAX DE
        case 0x13: INT_INX(&D, &E); break; // INX DE
        case 0x14: INT_INR(&D); break; // INR D
        case 0x15: INT_DCR(&D); break; // DCR D
        case 0x16: INT_MVI(&D, data1); break; // MVI D, d8
        case 0x17: INT_RAL(); break; // RAL
        /*   0x18: - */
        case 0x19:
            DE = (D << 8) | E;
            INT_DAD(&DE); 
            break; // DAD DE
        case 0x1A: INT_LDAX(false); break; // LDAX DE
        case 0x1B: INT_DCX(&D, &E); break; // DCX DE
        case 0x1C: INT_INR(&E); break; // INR E
        case 0x1D: INT_DCR(&E); break; // DCR E
        case 0x1E: INT_MVI(&E, data1); break; // MVI E, d8
        case 0x1F: INT_RAR(); break; // RAR
        /*   0x20: - */
        case 0x21: INT_LXI(&H, &L, data2, data1); break; // LXI HL
        case 0x22: INT_SHLD((data2 << 8) | data1); break; // SHLD
        case 0x23: INT_INX(&H, &L); break; // INX HL
        case 0x24: INT_INR(&H); break; // INR H
        case 0x25: INT_DCR(&H); break; // DCR H
        case 0x26: INT_MVI(&H, data1); break; // MVI H, d8
        case 0x27: INT_DAA(); break; // DAA
        /*   0x28: - */
        case 0x29:
            HL = (H << 8) | L;
            INT_DAD(&HL); 
            break; // DAD HL
        case 0x2A: INT_LHLD((data2 << 8) | data1); break; // LHLD
        case 0x2B: INT_DCX(&H, &L); break; // DCX HL
        case 0x2C: INT_INR(&L); break; // INR L
        case 0x2D: INT_DCR(&L); break; // DCR L
        case 0x2E: INT_MVI(&L, data1); break; // MVI L, d8
        case 0x2F: INT_CMA(); break; // CMA
        /*   0x30: - */
        case 0x31: INT_LXI_SP( data2, data1); break; // LXI SP
        case 0x32: INT_STA((data2 << 8) | data1); break; // STA
        case 0x33: INT_INX_SP(); break; // INX SP
        case 0x34: INT_INR_MEM( &(memory[(H << 8) | L]) ); break; // INR SP
        case 0x35: INT_DCR_MEM(&(memory[(H << 8) | L]) ); break; // DCR SP
        case 0x36: INT_MVI_MEM(&(memory[(H << 8) | L]), data1); break; // MVI SP, d8
        case 0x37: INT_STC(); break; // STC
        /*   0x38: - */
        case 0x39:
            INT_DAD(&SP); 
            break; // DAD SP
        case 0x3A: INT_LDA((data2 << 8) | data1); break; // LDA
        case 0x3B: INT_DCX_SP(); break; // DCX SP
        case 0x3C: INT_INR(&A); break; // INR A
        case 0x3D: INT_DCR(&A); break; // DCR A
        case 0x3E: INT_MVI(&A, data1); break; // MVI A, d8
        case 0x3F: INT_CMC(); break; // CMC
        /*   0x40: - */
        case 0x40: INT_MOV_REG(&B, &B); break; // MOV B, B
        case 0x41: INT_MOV_REG(&B, &C); break; // MOV B, C
        case 0x42: INT_MOV_REG(&B, &D); break; // MOV B, D
        case 0x43: INT_MOV_REG(&B, &E); break; // MOV B, E
        case 0x44: INT_MOV_REG(&B, &H); break; // MOV B, H
        case 0x45: INT_MOV_REG(&B, &L); break; // MOV B, L
        case 0x46: INT_MOV_MEM(&B, true); break; // MOV B, M
        case 0x47: INT_MOV_REG(&B, &A); break; // MOV B, A
        case 0x48: INT_MOV_REG(&C, &B); break; // MOV C, B
        case 0x49: INT_MOV_REG(&C, &C); break; // MOV C, C
        case 0x4A: INT_MOV_REG(&C, &D); break; // MOV C, D
        case 0x4B: INT_MOV_REG(&C, &E); break; // MOV C, E
        case 0x4C: INT_MOV_REG(&C, &H); break; // MOV C, H
        case 0x4D: INT_MOV_REG(&C, &L); break; // MOV C, L
        case 0x4E: INT_MOV_MEM(&C, true); break; // MOV C, M
        case 0x4F: INT_MOV_REG(&C, &A); break; // MOV C, A
        /*   0x50: - */
        case 0x50: INT_MOV_REG(&D, &B); break; // MOV D, B
        case 0x51: INT_MOV_REG(&D, &C); break; // MOV D, C
        case 0x52: INT_MOV_REG(&D, &D); break; // MOV D, D
        case 0x53: INT_MOV_REG(&D, &E); break; // MOV D, E
        case 0x54: INT_MOV_REG(&D, &H); break; // MOV D, H
        case 0x55: INT_MOV_REG(&D, &L); break; // MOV D, L
        case 0x56: INT_MOV_MEM(&D, true); break; // MOV D, M
        case 0x57: INT_MOV_REG(&D, &A); break; // MOV D, A
        case 0x58: INT_MOV_REG(&E, &B); break; // MOV E, B
        case 0x59: INT_MOV_REG(&E, &C); break; // MOV E, C
        case 0x5A: INT_MOV_REG(&E, &D); break; // MOV E, D
        case 0x5B: INT_MOV_REG(&E, &E); break; // MOV E, E
        case 0x5C: INT_MOV_REG(&E, &H); break; // MOV E, H
        case 0x5D: INT_MOV_REG(&E, &L); break; // MOV E, L
        case 0x5E: INT_MOV_MEM(&E, true); break; // MOV E, M
        case 0x5F: INT_MOV_REG(&E, &A); break; // MOV E, A
        /*   0x60: - */
        case 0x60: INT_MOV_REG(&H, &B); break; // MOV H, B
        case 0x61: INT_MOV_REG(&H, &C); break; // MOV H, C
        case 0x62: INT_MOV_REG(&H, &D); break; // MOV H, D
        case 0x63: INT_MOV_REG(&H, &E); break; // MOV H, E
        case 0x64: INT_MOV_REG(&H, &H); break; // MOV H, H
        case 0x65: INT_MOV_REG(&H, &L); break; // MOV H, L
        case 0x66: INT_MOV_MEM(&H, true); break; // MOV H, M
        case 0x67: INT_MOV_REG(&H, &A); break; // MOV H, A
        case 0x68: INT_MOV_REG(&L, &B); break; // MOV L, B
        case 0x69: INT_MOV_REG(&L, &C); break; // MOV L, C
        case 0x6A: INT_MOV_REG(&L, &D); break; // MOV L, D
        case 0x6B: INT_MOV_REG(&L, &E); break; // MOV L, E
        case 0x6C: INT_MOV_REG(&L, &H); break; // MOV L, H
        case 0x6D: INT_MOV_REG(&L, &L); break; // MOV L, L
        case 0x6E: INT_MOV_MEM(&L, true); break; // MOV L, M
        case 0x6F: INT_MOV_REG(&L, &A); break; // MOV L, A
        /*   0x70: - */
        case 0x70: INT_MOV_MEM(&B, false); break; // MOV M, B
        case 0x71: INT_MOV_MEM(&C, false); break; // MOV M, C
        case 0x72: INT_MOV_MEM(&D, false); break; // MOV M, D
        case 0x73: INT_MOV_MEM(&E, false); break; // MOV M, E
        case 0x74: INT_MOV_MEM(&H, false); break; // MOV M, H
        case 0x75: INT_MOV_MEM(&L, false); break; // MOV M, L
        case 0x76: INT_HLT(); break; // HLT
        case 0x77: INT_MOV_MEM(&A, false); break; // MOV M, A
        case 0x78: INT_MOV_REG(&A, &B); break; // MOV A, B
        case 0x79: INT_MOV_REG(&A, &C); break; // MOV A, C
        case 0x7A: INT_MOV_REG(&A, &D); break; // MOV A, D
        case 0x7B: INT_MOV_REG(&A, &E); break; // MOV A, E
        case 0x7C: INT_MOV_REG(&A, &H); break; // MOV A, H
        case 0x7D: INT_MOV_REG(&A, &L); break; // MOV A, L
        case 0x7E: INT_MOV_MEM(&A, true); break; // MOV A, M
        case 0x7F: INT_MOV_REG(&A, &A); break; // MOV A, A
        /*   0x80: - */
        case 0x80: INT_ADD(&B); break; // ADD B
        case 0x81: INT_ADD(&C); break; // ADD C
        case 0x82: INT_ADD(&D); break; // ADD D
        case 0x83: INT_ADD(&E); break; // ADD E
        case 0x84: INT_ADD(&H); break; // ADD H
        case 0x85: INT_ADD(&L); break; // ADD L
        case 0x86: INT_ADD_MEM(); break; // ADD M
        case 0x87: INT_ADD(&A); break; // ADD A
        case 0x88: INT_ADC(&B); break; // ADC B
        case 0x89: INT_ADC(&C); break; // ADC C
        case 0x8A: INT_ADC(&D); break; // ADC D
        case 0x8B: INT_ADC(&E); break; // ADC E
        case 0x8C: INT_ADC(&H); break; // ADC H
        case 0x8D: INT_ADC(&L); break; // ADC L
        case 0x8E: INT_ADC_MEM(); break; // ADC M
        case 0x8F: INT_ADC(&A); break; // ADC A
        /*   0x90: - */
        case 0x90: INT_SUB(&B); break; // SUB B
        case 0x91: INT_SUB(&C); break; // SUB C
        case 0x92: INT_SUB(&D); break; // SUB D
        case 0x93: INT_SUB(&E); break; // SUB E
        case 0x94: INT_SUB(&H); break; // SUB H
        case 0x95: INT_SUB(&L); break; // SUB L
        case 0x96: INT_SUB_MEM(); break; // SUB M
        case 0x97: INT_SUB(&A); break; // SUB A
        case 0x98: INT_SBB(&B); break; // SBB B
        case 0x99: INT_SBB(&C); break; // SBB C
        case 0x9A: INT_SBB(&D); break; // SBB D
        case 0x9B: INT_SBB(&E); break; // SBB E
        case 0x9C: INT_SBB(&H); break; // SBB H
        case 0x9D: INT_SBB(&L); break; // SBB L
        case 0x9E: INT_SBB_MEM(); break; // SBB M
        case 0x9F: INT_SBB(&A); break; // SBB A
        /*   0xA0: - */
        case 0xA0: INT_ANA(&B); break; // AND B
        case 0xA1: INT_ANA(&C); break; // AND C
        case 0xA2: INT_ANA(&D); break; // AND D
        case 0xA3: INT_ANA(&E); break; // AND E
        case 0xA4: INT_ANA(&H); break; // AND H
        case 0xA5: INT_ANA(&L); break; // AND L
        case 0xA6: INT_ANA_MEM(); break; // AND M
        case 0xA7: INT_ANA(&A); break; // AND A
        case 0xA8: INT_XRA(&B); break; // XOR B
        case 0xA9: INT_XRA(&C); break; // XOR C
        case 0xAA: INT_XRA(&D); break; // XOR D
        case 0xAB: INT_XRA(&E); break; // XOR E
        case 0xAC: INT_XRA(&H); break; // XOR H
        case 0xAD: INT_XRA(&L); break; // XOR L
        case 0xAE: INT_XRA_MEM(); break; // XOR M
        case 0xAF: INT_XRA(&A); break; // XOR A
        /*   0xB0: - */
        case 0xB0: INT_ORA(&B); break; // OR B
        case 0xB1: INT_ORA(&C); break; // OR C
        case 0xB2: INT_ORA(&D); break; // OR D
        case 0xB3: INT_ORA(&E); break; // OR E
        case 0xB4: INT_ORA(&H); break; // OR H
        case 0xB5: INT_ORA(&L); break; // OR L
        case 0xB6: INT_ORA_MEM(); break; // OR M
        case 0xB7: INT_ORA(&A); break; // OR A
        case 0xB8: INT_CMP(&B); break; // CMP B
        case 0xB9: INT_CMP(&C); break; // CMP C
        case 0xBA: INT_CMP(&D); break; // CMP D
        case 0xBB: INT_CMP(&E); break; // CMP E
        case 0xBC: INT_CMP(&H); break; // CMP H
        case 0xBD: INT_CMP(&L); break; // CMP L
        case 0xBE: INT_CMP_MEM(); break; // CMP M
        case 0xBF: INT_CMP(&A); break; // CMP A
        /*   0xC0: - */
        case 0xC0: INT_RNZ(); break; // RNZ
        case 0xC1: INT_POP(&B, &C); break; // POP BC
        case 0xC2: INT_JNZ((data2 << 8) | data1 ); break; // JNZ
        case 0xC3: INT_JMP((data2 << 8) | data1); break; // JMP
        case 0xC4: INT_CNZ((data2 << 8) | data1); break; // CNZ
        case 0xC5: INT_PUSH(&B, &C); break; // PUSH BC
        case 0xC6: INT_ADI(data1); break; // ADI
        case 0xC7: INT_RST(0); break; // RST 0
        case 0xC8: INT_RZ(); break; // RZ
        case 0xC9: INT_RET(true); break; // RET
        case 0xCA: INT_JZ((data2 << 8) | data1); break; // JZ
        // case 0xCB: INT_RST(1); break; // RST 1
        case 0xCC: INT_CZ((data2 << 8) | data1); break; // CZ
        case 0xCD: INT_CALL((data2 << 8) | data1, true); break; // CALL
        case 0xCE: INT_ACI(data1); break; // ADC
        case 0xCF: INT_RST(1); break; // RST 1
        /*   0xD0: - */
        case 0xD0: INT_RNC(); break; // RNC
        case 0xD1: INT_POP(&D, &E); break; // POP DE
        case 0xD2: INT_JNC((data2 << 8) | data1); break; // JNC
        case 0xD3: INT_OUT(); break; // OUT
        case 0xD4: INT_CNC((data2 << 8) | data1); break; // CNC
        case 0xD5: INT_PUSH(&D, &E); break; // PUSH DE
        case 0xD6: INT_SUI(data1); break; // SUI
        case 0xD7: INT_RST(2); break; // RST 2
        case 0xD8: INT_RC(); break; // RC
        // case 0xD9: INT_RETI(); break; // RETI
        case 0xDA: INT_JC((data2 << 8) | data1); break; // JC
        case 0xDB: 
            INT_IN(); 
            std::cout << "IN" << std::endl;
            break; // IN
        case 0xDC: INT_CC((data2 << 8) | data1); break; // CC
        // case 0xDD: INT_RST(3); break; // RST 3
        case 0xDE: INT_SBI(data1); break; // SBI
        case 0xDF: INT_RST(3); break; // RST 4
        /*   0xE0: - */
        case 0xE0: INT_RPO(); break; // RPO
        case 0xE1: INT_POP(&H, &L); break; // POP HL
        case 0xE2: INT_JPO((data2 << 8) | data1); break; // JPO
        case 0xE3: INT_XTHL(); break; // XTHL
        case 0xE4: INT_CPO((data2 << 8) | data1); break; // CPO
        case 0xE5: INT_PUSH(&H, &L); break; // PUSH HL
        case 0xE6: INT_ANI(data1); break; // ANI
        case 0xE7: INT_RST(4); break; // RST 4
        case 0xE8: INT_RPE(); break; // RPE
        case 0xE9: INT_PCHL(); break; // PCHL
        case 0xEA: INT_JPE((data2 << 8) | data1); break; // JPE
        case 0xEB: INT_XCHG(); break; // XCHG
        case 0xEC: INT_CPE((data2 << 8) | data1); break; // CPE
        // case 0xED: INT_RST(5); break; // RST 5
        case 0xEE: INT_XRI(data1); break; // XRI
        case 0xEF: INT_RST(5); break; // RST 6
        /*   0xF0: - */
        case 0xF0: INT_RP(); break; // RP
        case 0xF1: INT_POP_PSW(); break; // POP AF
        case 0xF2: INT_JP((data2 << 8) | data1); break; // JP
        case 0xF3: INT_DI(); break; // DI
        case 0xF4: INT_CP((data2 << 8) | data1); break; // CP
        case 0xF5: INT_PUSH_PSW(); break; // PUSH AF
        case 0xF6: INT_ORI(data1); break; // ORI
        case 0xF7: INT_RST(6); break; // RST 6
        case 0xF8: INT_RM(); break; // RM
        case 0xF9: INT_SPHL(); break; // SPHL
        case 0xFA: INT_JM((data2 << 8) | data1); break; // JM
        case 0xFB: INT_EI(); break; // EI
        case 0xFC: INT_CM((data2 << 8) | data1); break; // CM
        // case 0xFD: INT_RST(7); break; // RST 7
        case 0xFE: INT_CPI(data1); break; // CPI
        case 0xFF: INT_RST(7); break; // RST 7
    }

}