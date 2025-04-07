#include <iostream>
#include "include/I8080.h"
#include <cassert>

void testSetFlags() {
    I8080 cpu;
    
    cpu.setFlagZeroArith(0x00);
    cpu.setFlagSignArith(0x8A);
    cpu.setFlagParityArith(0xAA);
    cpu.setFlagAuxCarryArith(0x12, 0x0F, 0);
    cpu.setFlagCarryArith(0x134);
    
    std::cout << "Flags after setting with 0x00, 0x8A, 0xAA, 0x12+0x0F, 0x134:" << std::endl;
    std::cout << "Zero: " << cpu.zero << ", Sign: " << cpu.sign << ", Parity: " << cpu.parity << ", Aux Carry: " << cpu.aux_carry << ", Carry: " << cpu.carry << std::endl;

    assert(cpu.zero == true);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);

    cpu.setFlagZeroArith(0x01);
    cpu.setFlagSignArith(0x2A);
    cpu.setFlagParityArith(0x2A);
    cpu.setFlagAuxCarryArith(0x12, 0x01, 0);
    cpu.setFlagCarryArith(0x34);

    std::cout << "Flags after setting with 0x01, 0x2A, 0x2A, 0x12+0x01, 0x34:" << std::endl;
    std::cout << "Zero: " << cpu.zero << ", Sign: " << cpu.sign << ", Parity: " << cpu.parity << ", Aux Carry: " << cpu.aux_carry << ", Carry: " << cpu.carry << std::endl;

    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


    cpu.setFlagZeroArith(-0x01);
    cpu.setFlagSignArith(-0x01);
    cpu.setFlagParityArith(-0x01);
    cpu.setFlagAuxCarryArith(0x00, 0x01, 2);
    cpu.setFlagCarryArith(-0x01);

    std::cout << "Flags after setting with 0x01, 0x2A, 0x2A, 0x12+0x01, 0x34:" << std::endl;
    std::cout << "Zero: " << cpu.zero << ", Sign: " << cpu.sign << ", Parity: " << cpu.parity << ", Aux Carry: " << cpu.aux_carry << ", Carry: " << cpu.carry << std::endl;

    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);
}

void testINT_CMC() {
    I8080 cpu;
    cpu.carry = false;
    cpu.INT_CMC();
    std::cout << "After INT_CMC (carry=false): Carry: " << cpu.carry << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.carry == true);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    cpu.INT_CMC();
    std::cout << "After INT_CMC (carry=true): Carry: " << cpu.carry << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.carry == false);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

}

void testINT_STC() {
    I8080 cpu;
    cpu.carry = false;
    cpu.INT_STC();

    std::cout << "PC after INT_STC: " << cpu.PC << std::endl; // Output the PC value
    std::cout << "Cycles after INT_STC: " << cpu.cycles << std::endl; // Output the cycles value


    assert(cpu.carry == true);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    
}

void testINT_INR() {
    I8080 cpu;
    cpu.A = 0x00;
    cpu.INT_INR(&cpu.A);
    
    std::cout << "After INT_INR (A=0x00): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x01);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    // assert(cpu.carry == false);

    cpu.A = 0xFF; 
    cpu.INT_INR(&cpu.A);
    
    std::cout << "After INT_INR (A=0xFF): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x00);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

    // Test with overflow
    assert(cpu.zero == true);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    // assert(cpu.carry == true);

}

void testINT_INR_MEM(){
    I8080 cpu;

    // Test with memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x01;
    cpu.INT_INR_MEM(&cpu.memory[cpu.HL]);
    
    std::cout << "After INT_INR (memory[cpu.HL]=0x01): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.memory[cpu.HL] == 0x02);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 10);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    // assert(cpu.carry == false);


    // Test with memory
    cpu.INT_INR_MEM(&cpu.memory[cpu.HL]);
    
    std::cout << "After INT_INR (memory[cpu.HL]=0x03): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.memory[cpu.HL] == 0x03);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 20);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    // assert(cpu.carry == false);
}

void testINT_DCR() {
    I8080 cpu;
    cpu.A = 0x02;
    cpu.INT_DCR(&cpu.A);
    
    std::cout << "After INT_DCR (A=0x02): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x01);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    // assert(cpu.carry == false);

    cpu.A = 0x00; 
    cpu.INT_DCR(&cpu.A);
    
    std::cout << "After INT_INR (A=0x00): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0xFFu);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    // assert(cpu.carry == true);

}

void testINT_DCR_MEM(){
    I8080 cpu;

    // Test with memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x01;
    cpu.INT_DCR_MEM(&cpu.memory[cpu.HL]);
    
    std::cout << "After INT_DCR (memory[cpu.HL]=0x00): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.memory[cpu.HL] == 0x00);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 10);

    // Test with overflow
    assert(cpu.zero == true);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    // assert(cpu.carry == false);


    // Test with memory
    cpu.INT_DCR_MEM(&cpu.memory[cpu.HL]);
    
    std::cout << "After INT_DCR (memory[cpu.HL]=0xFF): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.memory[cpu.HL] == 0xFF);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 20);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    // assert(cpu.carry == false);
}

void testINT_CMA() {
    std::cout << "Testing INT_CMC" << std::endl;

    I8080 cpu;

    cpu.A = 0x5Fu;

    cpu.INT_CMA();
    
    std::cout << "After INT_CMA A: " << std::hex  << (int)cpu.A  << std::endl;
    std::cout << "Cycles after INT_CMA: " << cpu.cycles << std::endl;
    std::cout << "PC after INT_CMA: " << cpu.PC << std::endl;

    assert(cpu.A == 0xA0u);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    std::cout << "Test passed! INT_CMA" << std::endl;
    
}


void testINT_DAA(){

    I8080 cpu;

    cpu.A = 0x45;
    cpu.aux_carry = false;
    cpu.carry = false;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x45): A: " << std::hex << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x45);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


    // Only least 4-bit carry
    cpu.A = 0x4A;
    cpu.aux_carry = false;
    cpu.carry = false;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x4A): A: " << std::hex  << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x50);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    // Only most 4-bit carry
    cpu.A = 0xA4;
    cpu.aux_carry = false;
    cpu.carry = false;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0xA4): A: " << std::hex  << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x04);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 12);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);


    // Least 4 bit carry and make most 4 bit carry
    cpu.A = 0x9B;
    cpu.aux_carry = false;
    cpu.carry = false;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x9B): A: " << std::hex  << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x01);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 16);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);


    // carry and aux_carry is true
    cpu.A = 0x45;
    cpu.aux_carry = true;
    cpu.carry = true;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x45): A: " << std::hex << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0xAB);
    assert(cpu.PC == 5);
    assert(cpu.cycles == 20);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


}


void testINT_DAA_Flag(){

    I8080 cpu;


    // Least 4 bit carry and make most 4 bit carry
    cpu.A = 0x9B;
    cpu.aux_carry = false;
    cpu.carry = false;
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x9B): A: " << std::hex  << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x01);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);

    // carry and aux_carry is true from step 1
    cpu.INT_DAA();
    
    std::cout << "After INT_DAA (A=0x01): A: " << std::hex << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x67);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


}


void testINT_MOV_REG(){
    I8080 cpu;

    cpu.A = 0x12;
    cpu.D = 0x34;
    cpu.C = 0x56;

    // Before INT_MOV_REG
    std::cout << "Before INT_MOV_REG (A=0x12, D=0x34): A: " << (int)cpu.A << ", D: " << (int)cpu.D << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.D == 0x34);
    assert(cpu.C == 0x56);

    cpu.INT_MOV_REG(&cpu.A, &cpu.D);
    
    // After INT_MOV_REG
    std::cout << "After INT_MOV_REG (A=0x12, D=0x34): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x34);
    assert(cpu.D == 0x34);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 5);

    cpu.INT_MOV_REG(&cpu.D, &cpu.C);
    
    std::cout << "After INT_MOV_REG (A=0x34, D=0x34): D: " << (int)cpu.B << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.D == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 10);
}

void testINT_MOV_MEM(){

    I8080 cpu;

    cpu.A = 0x12;
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x34;

    // Before INT_MOV_MEM
    std::cout << "Before INT_MOV_MEM (A=0x12, memory[HL]=0x34): A: " << (int)cpu.A << ", memory[HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.memory[cpu.HL] == 0x34);

    cpu.INT_MOV_MEM(&cpu.A, true);
    
    // After INT_MOV_MEM
    std::cout << "After INT_MOV_MEM (A=0x12, memory[HL]=0x34): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x34);
    assert(cpu.memory[cpu.HL] == 0x34);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 7);

    cpu.A = 0x56;
    cpu.INT_MOV_MEM(&cpu.A, false);
    
    std::cout << "After INT_MOV_MEM (A=0x56, memory[HL]=0x34): memory[HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x56);
    assert(cpu.memory[cpu.HL] == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 14);

}

void testINT_STAX(){
    I8080 cpu;

    cpu.A = 0x12;
    cpu.B = 0x31;
    cpu.C = 0x12;
    cpu.BC = cpu.B << 8 | cpu.C;
    cpu.memory[cpu.BC] = 0x34;

    // Before INT_STAX
    std::cout << "Before INT_STAX (A=0x12, BC=0x3112): A: " << (int)cpu.A << ", BC: " << (int)cpu.BC << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.BC == 0x3112);
    assert(cpu.memory[cpu.BC] == 0x34);

    cpu.INT_STAX(true);
    
    // After INT_STAX
    std::cout << "After INT_STAX (A=0x12, BC=0x3112): memory[BC]: " << (int)cpu.memory[cpu.BC] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.memory[cpu.BC] == 0x12);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 7);

    cpu.A = 0x56;
    cpu.D = 0x40;
    cpu.E = 0x45;
    cpu.DE = cpu.D << 8 | cpu.E;
    cpu.memory[cpu.DE] = 0x52;

    // Before INT_STAX
    std::cout << "Before INT_STAX (A=0x56, DE=0x4045): A: " << (int)cpu.A << ", DE: " << (int)cpu.DE << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x56);
    assert(cpu.DE == 0x4045);
    assert(cpu.memory[cpu.DE] == 0x52);

    cpu.INT_STAX(false);
    // After INT_STAX
    
    std::cout << "After INT_STAX (A=0x56, BC=0x3112): memory[BC]: " << (int)cpu.memory[cpu.BC] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x56);
    assert(cpu.memory[cpu.DE] == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 14);



}


void testINT_LDAX(){

    I8080 cpu;

    cpu.A = 0x12;
    cpu.B = 0x31;
    cpu.C = 0x12;
    cpu.BC = cpu.B << 8 | cpu.C;
    cpu.memory[cpu.BC] = 0x34;

    // Before INT_LDAX
    std::cout << "Before INT_LDAX (A=0x12, memory[BC]=0x34): A: " << (int)cpu.A << ", memory[BC]: " << (int)cpu.memory[cpu.BC] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.memory[cpu.BC] == 0x34);

    cpu.INT_LDAX(true);
    
    // After INT_LDAX
    std::cout << "After INT_LDAX (A=0x12, memory[BC]=0x34): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x34);
    assert(cpu.memory[cpu.BC] == 0x34);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 7);

    // cpu.A = 0x56;
    cpu.D = 0x40;
    cpu.E = 0x45;
    cpu.DE = cpu.D << 8 | cpu.E;
    cpu.memory[cpu.DE] = 0x56;
    

    // Before INT_LDAX
    std::cout << "Before INT_LDAX (A=0x56, DE=0x4045): A: " << (int)cpu.A << ", DE: " << (int)cpu.DE << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x34);
    assert(cpu.memory[cpu.DE] == 0x56);

    cpu.INT_LDAX(false);

    // After INT_LDAX
    
    std::cout << "After INT_LDAX (A=0x56, DE=0x4045): A: " << (int)cpu.A << ", memory[DE]: " << (int)cpu.memory[cpu.DE] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    
    assert(cpu.A == 0x56);
    assert(cpu.memory[cpu.DE] == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 14);

}


void testINT_ADD(){

    I8080 cpu;

    cpu.A = 0x12;
    cpu.C = 0x1E;

    // Before INT_ADD
    std::cout << "Before INT_ADD (A=0x12): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x12);
    assert(cpu.C == 0x1E);

    cpu.INT_ADD(&cpu.C );
    
    // After INT_ADD
    std::cout << "After INT_ADD (A=0x12): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x30);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;

    // Before INT_ADD_MEM
    std::cout << "Before INT_ADD_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);


    cpu.INT_ADD_MEM();

    // After INT_ADD_MEM
    std::cout << "After INT_ADD (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x48);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);


    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
 
}


void testINT_ADC(){
    I8080 cpu;

    cpu.A = 0x43;
    cpu.C = 0x3D;
    cpu.carry = false;
    // Before INT_ADC
    std::cout << "Before INT_ADC (A=0x42): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x43);
    assert(cpu.C == 0x3D);
    assert(cpu.carry == false);

    cpu.INT_ADC(&cpu.C );
    
    // After INT_ADC
    std::cout << "After INT_ADC (A=0x42): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x80);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;
    cpu.carry = true;

    // Before INT_ADC_MEM
    std::cout << "Before INT_ADC_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);


    cpu.INT_ADC_MEM();
    

    // After INT_ADC_MEM
    std::cout << "After INT_ADC (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;   

    assert(cpu.A == 0x99);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


}

void testINT_SUB(){
    I8080 cpu;

    cpu.A = 0x43;
    cpu.C = 0x3D;
    // Before INT_SUB
    std::cout << "Before INT_SUB (A=0x42): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x43);
    assert(cpu.C == 0x3D);

    cpu.INT_SUB(&cpu.C );
    
    // After INT_SUB
    std::cout << "After INT_SUB (A=0x43): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x06);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


    cpu.A = 0x43;
    cpu.C = 0x4F;
    // Before INT_SUB
    std::cout << "Before INT_SUB (A=0x43): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x43);
    assert(cpu.C == 0x4F);

    cpu.INT_SUB(&cpu.C );
    
    // After INT_SUB
    std::cout << "After INT_SUB (A=0x43): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0xF4);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 8);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);


    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;

    // Before INT_SUB_MEM
    std::cout << "Before INT_SUB_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);


    cpu.INT_SUB_MEM();
    
    // After INT_ADC_MEM
    std::cout << "After INT_SUB (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;   

    assert(cpu.A == 0xDC);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 15);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);

}


void testINT_SBB(){

    I8080 cpu;

    cpu.A = 0x23;
    cpu.C = 0x3D;
    cpu.carry = false;
    // Before INT_SBB
    std::cout << "Before INT_SBB (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x3D);
    assert(cpu.carry == false);

    cpu.INT_SBB(&cpu.C );
    
    // After INT_SBB
    std::cout << "After INT_SBB (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0xE6);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);


    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;


    // Before INT_SBB_MEM
    std::cout << "Before INT_SBB_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);

    cpu.INT_SBB_MEM();
    
    // After INT_SBB_MEM
    std::cout << "After INT_SBB (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;   

    assert(cpu.A == 0xCD);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);

}

void testINT_ANA(){

    I8080 cpu;

    cpu.A = 0x23;
    cpu.C = 0x3D;
    // Before INT_ANA
    std::cout << "Before INT_ANA (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x3D);

    cpu.INT_ANA(&cpu.C );
    
    // After INT_ANA
    std::cout << "After INT_ANA (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x21);
    assert(cpu.C == 0x3D);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    // assert(cpu.aux_carry == true);
    assert(cpu.carry == false);


    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;


    // Before INT_ANA_MEM
    std::cout << "Before INT_ANA_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);

    cpu.INT_ANA_MEM();
    
    // After INT_ANA_MEM
    std::cout << "After INT_ANA_MEM (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.A == 0x00);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == true);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    // assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

}

void testINT_XRA(){

    I8080 cpu;

    cpu.A = 0x23;
    cpu.C = 0x3D;
    // Before INT_XRA
    std::cout << "Before INT_XRA (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x3D);

    cpu.INT_XRA(&cpu.C);
    
    // After INT_XRA
    std::cout << "After INT_XRA (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x1E);
    assert(cpu.C == 0x3D);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);

    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;


    // Before INT_XRA_MEM
    std::cout << "Before INT_XRA_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);

    cpu.INT_XRA_MEM();
    
    // After INT_XRA_MEM
    std::cout << "After INT_XRA_MEM (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.A == 0x06);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);

}

void testINT_ORA(){

    I8080 cpu;

    cpu.A = 0x23;
    cpu.C = 0x3D;
    // Before INT_ORA
    std::cout << "Before INT_ORA (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x3D);

    cpu.INT_ORA(&cpu.C);
    
    // After INT_ORA
    std::cout << "After INT_ORA (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x3F);
    assert(cpu.C == 0x3D);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);

    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;


    // Before INT_ORA_MEM
    std::cout << "Before INT_ORA_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);

    cpu.INT_ORA_MEM();
    
    // After INT_ORA_MEM
    std::cout << "After INT_ORA_MEM (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.A == 0x3F);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
}


void testINT_CMP(){
    I8080 cpu;

    cpu.A = 0x23;
    cpu.C = 0x3D;
    cpu.carry = false;
    // Before INT_CMP
    std::cout << "Before INT_CMP (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x3D);
    assert(cpu.carry == false);

    cpu.INT_CMP(&cpu.C);
    
    // After INT_CMP
    std::cout << "After INT_CMP (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 4);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);

    // Test Memory
    cpu.H = 0x31;
    cpu.L = 0x12;
    cpu.HL = cpu.H << 8 | cpu.L;
    cpu.memory[cpu.HL] = 0x18;


    // Before INT_CMP_MEM
    std::cout << "Before INT_CMP_MEM (memory[cpu.HL]=0x18): memory[cpu.HL]: " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.memory[cpu.HL] == 0x18);

    cpu.INT_CMP_MEM();
    
    // After INT_CMP_MEM
    std::cout << "After INT_CMP_MEM (memory[cpu.HL]=0x18): " << (int)cpu.memory[cpu.HL] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.A == 0x23);
    assert(cpu.memory[cpu.HL] == 0x18);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 11);

    // Test with overflow
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);


    // Test With zero
    cpu.A = 0x23;
    cpu.C = 0x23;
    cpu.carry = false;
    // Before INT_CMP
    std::cout << "Before INT_CMP (A=0x23): A: " << (int)cpu.A   << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.C == 0x23);
    assert(cpu.carry == false);

    cpu.INT_CMP(&cpu.C);
    
    // After INT_CMP    
    std::cout << "After INT_CMP (A=0x23): A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;
    assert(cpu.A == 0x23);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 15);

    // Test with overflow
    assert(cpu.zero == true);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);
}

void testRotate() {
    I8080 cpu;
    cpu.A = 0b10101010;
    cpu.INT_RLC();
    assert(cpu.A == 0b01010101);
    assert(cpu.carry == true);
    
    cpu.INT_RRC();
    assert(cpu.A == 0b10101010);
    assert(cpu.carry == true);

    cpu.INT_RRC();
    assert(cpu.A == 0b01010101);
    assert(cpu.carry == false);

    cpu.INT_RAL();
    assert(cpu.A == 0b10101010);
    assert(cpu.carry == false);

    cpu.INT_RAL();
    assert(cpu.A == 0b01010100);
    assert(cpu.carry == true);

    cpu.INT_RAR();
    assert(cpu.A == 0b10101010);
    assert(cpu.carry == false);

    cpu.INT_RAR();
    assert(cpu.A == 0b01010101);
    assert(cpu.carry == false);
}

void testINT_PUSH() {
    I8080 cpu;
    cpu.SP = 0xFFFE;
    cpu.B = 0x12;
    cpu.C = 0x34;

    cpu.INT_PUSH(&cpu.B, &cpu.C);
    std::cout << "After INT_PUSH (BC=0x1234): SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << std::endl;

    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x12);
    assert(cpu.memory[0xFFFC] == 0x34);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 11);
}

void testINT_PUSH_PSW() {
    I8080 cpu;
    cpu.SP = 0xFFFE;
    cpu.A = 0x56;
    cpu.sign = true;
    cpu.zero = false;
    cpu.aux_carry = true;
    cpu.parity = false;
    cpu.carry = true;

    cpu.INT_PUSH_PSW();
    std::cout << "After INT_PUSH_PSW: SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << std::endl;

    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x56);
    assert(cpu.memory[0xFFFC] == 0x93); // 10010011 in binary
    assert(cpu.PC == 1);
    assert(cpu.cycles == 11);
}

void testINT_POP() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;

    cpu.INT_POP(&cpu.B, &cpu.C);
    std::cout << "After INT_POP: SP: " << cpu.SP << ", B: " << (int)cpu.B << ", C: " << (int)cpu.C << std::endl;

    assert(cpu.SP == 0xFFFE);
    assert(cpu.B == 0x12);
    assert(cpu.C == 0x34);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 10);
}

void testINT_POP_PSW() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x93; // 10010011 in binary
    cpu.memory[0xFFFD] = 0x56;
    // cpu.sign = true;
    // cpu.zero = false;
    // cpu.aux_carry = true;
    // cpu.parity = false;
    // cpu.carry = true;

    cpu.INT_POP_PSW();
    std::cout << "After INT_POP_PSW: SP: " << cpu.SP << ", A: " << (int)cpu.A << ", Flags: S=" << cpu.sign << " Z=" << cpu.zero << " AC=" << cpu.aux_carry << " P=" << cpu.parity << " C=" << cpu.carry << std::endl;

    assert(cpu.SP == 0xFFFE);
    assert(cpu.A == 0x56);
    assert(cpu.sign == true);
    assert(cpu.zero == false);
    assert(cpu.aux_carry == true);
    assert(cpu.parity == false);
    assert(cpu.carry == true);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 10);
}

void testINT_DAD() {
    I8080 cpu;
    cpu.H = 0x12;
    cpu.L = 0x34;
    cpu.D = 0x56;
    cpu.E = 0x78;
    cpu.DE = cpu.D << 8 | cpu.E;

    uint16_t DE = cpu.DE;

    cpu.INT_DAD(&DE);
    std::cout << "After INT_DAD (HL=0x1234, DE=0x5678): HL: " << (cpu.H << 8 | cpu.L) << ", Carry: " << cpu.carry << std::endl;

    assert((cpu.H << 8 | cpu.L) == 0x68AC);
    assert(cpu.carry == false);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 10);


    // Carry is set
    cpu.H = 0xFF;
    cpu.L = 0xFF;
    cpu.D = 0x00;
    cpu.E = 0x01;
    cpu.DE = cpu.D << 8 | cpu.E;

    DE = cpu.DE;

    cpu.INT_DAD(&DE);
    std::cout << "After INT_DAD (HL=0xFFFF, DE=0x0001): HL: " << (cpu.H << 8 | cpu.L) << ", Carry: " << cpu.carry << std::endl;

    assert((cpu.H << 8 | cpu.L) == 0x0000);
    assert(cpu.carry == true);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 20);
}

void testINT_INX() {
    I8080 cpu;
    cpu.B = 0x12;
    cpu.C = 0x34;

    cpu.INT_INX(&cpu.B, &cpu.C);
    std::cout << "After INT_INX (BC=0x1234): BC: " << (cpu.B << 8 | cpu.C) << std::endl;

    assert((cpu.B << 8 | cpu.C) == 0x1235);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 5);

    // Test stack pointer
    cpu.SP = 0x1234;
    cpu.INT_INX_SP();
    std::cout << "After INT_INX_SP (SP=0x1234): SP: " << cpu.SP << std::endl;

    assert(cpu.SP == 0x1235);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 10);


}

void testINT_DCX() {
    I8080 cpu;
    cpu.B = 0x12;
    cpu.C = 0x34;

    cpu.INT_DCX(&cpu.B, &cpu.C);
    std::cout << "After INT_DCX (BC=0x1234): BC: " << (cpu.B << 8 | cpu.C) << std::endl;

    assert((cpu.B << 8 | cpu.C) == 0x1233);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 5);

    // Test stack pointer
    cpu.SP = 0x1234;
    cpu.INT_DCX_SP();
    std::cout << "After INT_DCX_SP (SP=0x1234): SP: " << cpu.SP << std::endl;

    assert(cpu.SP == 0x1233);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 10);
}

void testINT_XCHG() {
    I8080 cpu;
    cpu.H = 0x12;
    cpu.L = 0x34;
    cpu.D = 0x56;
    cpu.E = 0x78;

    cpu.INT_XCHG();
    std::cout << "After INT_XCHG: HL: " << (cpu.H << 8 | cpu.L) << ", DE: " << (cpu.D << 8 | cpu.E) << std::endl;

    assert((cpu.H << 8 | cpu.L) == 0x5678);
    assert((cpu.D << 8 | cpu.E) == 0x1234);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 5);
}


void testINT_XTHL() {
    I8080 cpu;
    cpu.SP = 0x1000;
    cpu.memory[cpu.SP] = 0x12;
    cpu.memory[cpu.SP + 1] = 0x34;
    cpu.H = 0x56;
    cpu.L = 0x78;

    cpu.INT_XTHL();
    std::cout << "After INT_XTHL: H: " << (int)cpu.H << ", L: " << (int)cpu.L << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << std::endl;

    assert(cpu.H == 0x34);
    assert(cpu.L == 0x12);
    assert(cpu.memory[cpu.SP] == 0x78);
    assert(cpu.memory[cpu.SP + 1] == 0x56);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 18);
}

void testINT_SPHL() {
    I8080 cpu;
    cpu.H = 0x12;
    cpu.L = 0x34;

    cpu.INT_SPHL();
    std::cout << "After INT_SPHL: SP: " << cpu.SP << std::endl;

    assert(cpu.SP == 0x1234);
    assert(cpu.PC == 1);
    assert(cpu.cycles == 5);
}

void testINT_MVI() {
    I8080 cpu;
    cpu.A = 0x00;

    cpu.INT_MVI(&cpu.A, 0x56);
    std::cout << "After INT_MVI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);
}

void testINT_MVI_MEM() {
    I8080 cpu;

    // Move to register H
    cpu.INT_MVI(&cpu.H, 0x12);
    std::cout << "After INT_MVI: H: " << (int)cpu.H << std::endl;

    assert(cpu.H == 0x12);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Move to register C
    cpu.INT_MVI(&cpu.C, 0x34);
    std::cout << "After INT_MVI: C: " << (int)cpu.C << std::endl;

    assert(cpu.C == 0x34);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);

    // Move to memory
    cpu.HL = cpu.H << 8 | cpu.L;

    cpu.INT_MVI_MEM(&cpu.memory[cpu.HL], 0x56);
    std::cout << "After INT_MVI_MEM: memory[HL]: " << (int)cpu.memory[cpu.HL] << std::endl;

    assert(cpu.memory[cpu.HL] == 0x56);
    assert(cpu.PC == 6);
    assert(cpu.cycles == 24);
}

void testINT_ADI() {
    I8080 cpu;
    cpu.A = 0x12;

    cpu.INT_ADI(0x34);
    std::cout << "After INT_ADI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x46);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test with overflow

    cpu.INT_ADI(0xEB);
    std::cout << "After INT_ADI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x31);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);
    
    // Test bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);
}

void testINT_ACI() {
    I8080 cpu;
    cpu.A = 0x46;
    cpu.carry = false;

    cpu.INT_ACI(0xEB);
    std::cout << "After INT_ACI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x31);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == true);

    cpu.INT_ACI(0x12);
    std::cout << "After INT_ACI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x44);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
    
}

void testINT_SUI() {
    I8080 cpu;
    cpu.A = 0x56;

    cpu.INT_SUI(0x34);
    std::cout << "After INT_SUI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x22);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    cpu.INT_SUI(0x23);
    std::cout << "After INT_SUI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0xFF);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);
}

void testINT_SBI() {
    I8080 cpu;
    cpu.A = 0x56;
    cpu.carry = true;

    cpu.INT_SBI(0x34);
    std::cout << "After INT_SBI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x21);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    cpu.INT_SBI(0x23);
    std::cout << "After INT_SBI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0xFE);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);
}

void testINT_ANI() {
    I8080 cpu;
    cpu.A = 0x56;

    cpu.INT_ANI(0x34);
    std::cout << "After INT_ANI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x14);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
}

void testINT_XRI() {
    I8080 cpu;
    cpu.A = 0x56;

    cpu.INT_XRI(0x34);
    std::cout << "After INT_XRI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x62);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
}

void testINT_ORI() {
    I8080 cpu;
    cpu.A = 0x56;

    cpu.INT_ORI(0x34);
    std::cout << "After INT_ORI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x76);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == false);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == false);
}

void testINT_CPI() {
    I8080 cpu;

    // Test A is greater than immediate value
    cpu.A = 0x56;

    cpu.INT_CPI(0x34);
    std::cout << "After INT_CPI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x56);
    assert(cpu.PC == 2);
    assert(cpu.cycles == 7);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);

    // Test A is less than immediate value
    cpu.A = 0x34;

    cpu.INT_CPI(0x56);
    std::cout << "After INT_CPI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x34);
    assert(cpu.PC == 4);
    assert(cpu.cycles == 14);

    // Test Bit
    assert(cpu.zero == false);
    assert(cpu.sign == true);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == false);
    assert(cpu.carry == true);

    // Test A is equal to immediate value
    cpu.A = 0x34;

    cpu.INT_CPI(0x34);
    std::cout << "After INT_CPI: A: " << (int)cpu.A << std::endl;

    assert(cpu.A == 0x34);
    assert(cpu.PC == 6);
    assert(cpu.cycles == 21);

    // Test Bit
    assert(cpu.zero == true);
    assert(cpu.sign == false);
    assert(cpu.parity == true);
    assert(cpu.aux_carry == true);
    assert(cpu.carry == false);
    
}

void testINT_STA() {
    I8080 cpu;
    cpu.A = 0x56;
    uint16_t addr = 0x1234;

    cpu.INT_STA(addr);
    std::cout << "After INT_STA: memory[addr]: " << (int)cpu.memory[addr] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.memory[addr] == 0x56);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 13);
}

void testINT_LDA() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.memory[addr] = 0x56;

    cpu.INT_LDA(addr);
    std::cout << "After INT_LDA: A: " << (int)cpu.A << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.A == 0x56);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 13);
}

void testINT_SHLD() {
    I8080 cpu;
    cpu.H = 0x12;
    cpu.L = 0x34;
    uint16_t addr = 0x5678;

    cpu.INT_SHLD(addr);
    std::cout << "After INT_SHLD: memory[addr]: " << (int)cpu.memory[addr] << ", memory[addr+1]: " << (int)cpu.memory[addr + 1] << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.memory[addr] == 0x34);
    assert(cpu.memory[addr + 1] == 0x12);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 16);
}

void testINT_LHLD() {
    I8080 cpu;
    uint16_t addr = 0x5678;
    cpu.memory[addr] = 0x34;
    cpu.memory[addr + 1] = 0x12;

    cpu.INT_LHLD(addr);
    std::cout << "After INT_LHLD: L: " << (int)cpu.L << ", H: " << (int)cpu.H << ", PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.L == 0x34);
    assert(cpu.H == 0x12);
    assert(cpu.PC == 3);
    assert(cpu.cycles == 13);
}

void testINT_PCHL() {
    I8080 cpu;
    cpu.H = 0x12;
    cpu.L = 0x34;

    cpu.INT_PCHL();
    std::cout << "After INT_PCHL: PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.cycles == 5);
}

void testINT_JMP() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.INT_JMP(addr);
    std::cout << "After INT_JMP: PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);
}

void testINT_JC() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.carry = true;
    cpu.INT_JC(addr);
    std::cout << "After INT_JC (carry=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.carry = false;
    cpu.PC = 0x0000;
    cpu.cycles = 0;
    cpu.INT_JC(addr);
    std::cout << "After INT_JC (carry=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.cycles == 10);
}

void testINT_JNC() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.carry = false;
    cpu.INT_JNC(addr);
    std::cout << "After INT_JNC (carry=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.carry = true;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JNC(addr);
    std::cout << "After INT_JNC (carry=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JZ() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.zero = true;
    cpu.INT_JZ(addr);
    std::cout << "After INT_JZ (zero=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.zero = false;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JZ(addr);
    std::cout << "After INT_JZ (zero=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JNZ() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.zero = false;
    cpu.INT_JNZ(addr);
    std::cout << "After INT_JNZ (zero=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.zero = true;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JNZ(addr);
    std::cout << "After INT_JNZ (zero=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JM() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.sign = true;
    cpu.INT_JM(addr);
    std::cout << "After INT_JM (sign=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.sign = false;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JM(addr);
    std::cout << "After INT_JM (sign=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JP() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.sign = false;
    cpu.INT_JP(addr);
    std::cout << "After INT_JP (sign=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.sign = true;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JP(addr);
    std::cout << "After INT_JP (sign=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JPE() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.parity = true;
    cpu.INT_JPE(addr);
    std::cout << "After INT_JPE (parity=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.parity = false;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JPE(addr);
    std::cout << "After INT_JPE (parity=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_JPO() {
    I8080 cpu;
    uint16_t addr = 0x1234;

    cpu.parity = false;
    cpu.INT_JPO(addr);
    std::cout << "After INT_JPO (parity=false): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.cycles == 10);

    cpu.parity = true;
    cpu.PC = 0;
    cpu.cycles = 0;
    cpu.INT_JPO(addr);
    std::cout << "After INT_JPO (parity=true): PC: " << cpu.PC << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 3);
    assert(cpu.cycles == 10);
}

void testINT_CALL() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.INT_CALL(addr, true);
    std::cout << "After INT_CALL (cond=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CALL(addr, false);
    std::cout << "After INT_CALL (cond=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CC() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.carry = true;
    cpu.INT_CC(addr);
    std::cout << "After INT_CC (carry=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.carry = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CC(addr);
    std::cout << "After INT_CC (carry=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CNC() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x3456;

    cpu.carry = false;
    cpu.INT_CNC(addr);
    std::cout << "After INT_CNC (carry=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x34);
    assert(cpu.memory[0xFFFC] == 0x59);
    assert(cpu.cycles == 17);

    cpu.carry = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CNC(addr);
    std::cout << "After INT_CNC (carry=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CZ() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.zero = true;
    cpu.INT_CZ(addr);
    std::cout << "After INT_CZ (zero=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.zero = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CZ(addr);
    std::cout << "After INT_CZ (zero=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CNZ() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.zero = false;
    cpu.INT_CNZ(addr);
    std::cout << "After INT_CNZ (zero=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.zero = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CNZ(addr);
    std::cout << "After INT_CNZ (zero=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CM() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.sign = true;
    cpu.INT_CM(addr);
    std::cout << "After INT_CM (sign=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.sign = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CM(addr);
    std::cout << "After INT_CM (sign=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CP() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x3456;

    cpu.sign = false;
    cpu.INT_CP(addr);
    std::cout << "After INT_CP (sign=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x34);
    assert(cpu.memory[0xFFFC] == 0x59);
    assert(cpu.cycles == 17);

    cpu.sign = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CP(addr);
    std::cout << "After INT_CP (sign=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CPE() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x3456;

    cpu.parity = true;
    cpu.INT_CPE(addr);
    std::cout << "After INT_CPE (parity=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x34);
    assert(cpu.memory[0xFFFC] == 0x59);
    assert(cpu.cycles == 17);

    cpu.parity = false;
    cpu.PC = 0x3456;
    cpu.SP = 0xFFFE;
    cpu.INT_CPE(addr);
    std::cout << "After INT_CPE (parity=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x3459);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_CPO() {
    I8080 cpu;
    uint16_t addr = 0x1234;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x0000;

    cpu.parity = false;
    cpu.INT_CPO(addr);
    std::cout << "After INT_CPO (parity=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == addr);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x00);
    assert(cpu.memory[0xFFFC] == 0x03);
    assert(cpu.cycles == 17);

    cpu.parity = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFE;
    cpu.INT_CPO(addr);
    std::cout << "After INT_CPO (parity=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0003);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 28);
}

void testINT_RET() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.INT_RET(true);
    std::cout << "After INT_RET (cond=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RET(false);
    std::cout << "After INT_RET (cond=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RC() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.carry = true;
    cpu.INT_RC();
    std::cout << "After INT_RC (carry=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.carry = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RC();
    std::cout << "After INT_RC (carry=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RNC() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.carry = false;
    cpu.INT_RNC();
    std::cout << "After INT_RNC (carry=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.carry = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RNC();
    std::cout << "After INT_RNC (carry=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RZ() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.zero = true;
    cpu.INT_RZ();
    std::cout << "After INT_RZ (zero=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.zero = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RZ();
    std::cout << "After INT_RZ (zero=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RNZ() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.zero = false;
    cpu.INT_RNZ();
    std::cout << "After INT_RNZ (zero=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.zero = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RNZ();
    std::cout << "After INT_RNZ (zero=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RM() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.sign = true;
    cpu.INT_RM();
    std::cout << "After INT_RM (sign=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.sign = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RM();
    std::cout << "After INT_RM (sign=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RP() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.sign = false;
    cpu.INT_RP();
    std::cout << "After INT_RP (sign=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.sign = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RP();
    std::cout << "After INT_RP (sign=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RPE() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.parity = true;
    cpu.INT_RPE();
    std::cout << "After INT_RPE (parity=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.parity = false;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RPE();
    std::cout << "After INT_RPE (parity=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RPO() {
    I8080 cpu;
    cpu.SP = 0xFFFC;
    cpu.memory[0xFFFC] = 0x34;
    cpu.memory[0xFFFD] = 0x12;
    cpu.PC = 0x0000;

    cpu.parity = false;
    cpu.INT_RPO();
    std::cout << "After INT_RPO (parity=false): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x1234);
    assert(cpu.SP == 0xFFFE);
    assert(cpu.cycles == 10);

    cpu.parity = true;
    cpu.PC = 0x0000;
    cpu.SP = 0xFFFC;
    cpu.INT_RPO();
    std::cout << "After INT_RPO (parity=true): PC: " << cpu.PC << ", SP: " << cpu.SP << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x0001);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.cycles == 15);
}

void testINT_RST() {
    I8080 cpu;
    cpu.SP = 0xFFFE;
    cpu.PC = 0x1234;

    // RST 0
    cpu.INT_RST(0);
    std::cout << "After INT_RST(0): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x00);
    assert(cpu.SP == 0xFFFC);
    assert(cpu.memory[0xFFFD] == 0x12);
    assert(cpu.memory[0xFFFC] == 0x35);
    assert(cpu.cycles == 11);

    // RST 1
    cpu.INT_RST(1);
    std::cout << "After INT_RST(1): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x08);
    assert(cpu.SP == 0xFFFA);
    assert(cpu.memory[0xFFFB] == 0x00);
    assert(cpu.memory[0xFFFA] == 0x01);
    assert(cpu.cycles == 22);

    // RST 2
    cpu.INT_RST(2);
    std::cout << "After INT_RST(2): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x10);
    assert(cpu.SP == 0xFFF8);
    assert(cpu.memory[0xFFF9] == 0x00);
    assert(cpu.memory[0xFFF8] == 0x09);
    assert(cpu.cycles == 33);

    // RST 3
    cpu.INT_RST(3);
    std::cout << "After INT_RST(3): PC: " << cpu.PC << ", SP: " << cpu.SP << ", memory[SP]: " << (int)cpu.memory[cpu.SP] << ", memory[SP+1]: " << (int)cpu.memory[cpu.SP + 1] << ", Cycles: " << cpu.cycles << std::endl;

    assert(cpu.PC == 0x18);
    assert(cpu.SP == 0xFFF6);
    assert(cpu.memory[0xFFF7] == 0x00);
    assert(cpu.memory[0xFFF6] == 0x11);
    assert(cpu.cycles == 44);
}


int main() {
    testSetFlags();
    testINT_CMC();
    testINT_STC();
    testINT_INR();
    testINT_INR_MEM();
    testINT_DCR();
    testINT_DCR_MEM();
    testINT_CMA();
    testINT_DAA();
    testINT_DAA_Flag();
    testINT_MOV_REG();
    testINT_MOV_MEM();
    testINT_STAX();
    testINT_LDAX();
    testINT_ADD();
    testINT_ADC();
    testINT_SUB();
    testINT_SBB();
    testINT_ANA();
    testINT_XRA();
    testINT_ORA();
    testINT_CMP();
    testRotate();
    testINT_PUSH();
    testINT_PUSH_PSW();
    testINT_POP();
    testINT_POP_PSW();
    testINT_DAD();
    testINT_INX();
    testINT_DCX();
    testINT_XCHG();
    testINT_XTHL();
    testINT_SPHL();
    testINT_MVI();
    testINT_MVI_MEM();
    testINT_ADI();
    testINT_ACI();
    testINT_SUI();
    testINT_SBI();
    testINT_ANI();
    testINT_XRI();
    testINT_ORI();
    testINT_CPI();
    testINT_STA();
    testINT_LDA();
    testINT_SHLD();
    testINT_LHLD();
    testINT_PCHL();
    testINT_JMP();
    testINT_JC();
    testINT_JNC();
    testINT_JZ();
    testINT_JNZ();
    testINT_JM();
    testINT_JP();
    testINT_JPE();
    testINT_JPO();
    testINT_CALL();
    testINT_CC();
    testINT_CNC();
    testINT_CZ();
    testINT_CNZ();
    testINT_CM();
    testINT_CP();
    testINT_CPE();
    testINT_CPO();
    testINT_RET();
    testINT_RC();
    testINT_RNC();
    testINT_RZ();
    testINT_RNZ();
    testINT_RM();
    testINT_RP();
    testINT_RPE();
    testINT_RPO();
    testINT_RST();


    std::cout << "All tests passed!" << std::endl;
    return 0;
}