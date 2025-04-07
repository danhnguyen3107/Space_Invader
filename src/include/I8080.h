#ifndef I8080_H
#define I8080_H

#include <cstdint>


// uint16_t ROMAddress = 0x0000;
// uint16_t RAMAddress = 0x2000;
// uint16_t VRAMAddress = 0x2400;
// uint16_t RAMMirrorAddress = 0x4000;

const uint32_t MEMORY_SIZE = 0x10000; // 64KB of memory for the I8080

// invader file start memory address
const uint16_t INVADER_START_ADDRESS_H = 0x0000; // Start address for the invader ROM
const uint16_t INVADER_START_ADDRESS_G = 0x0800;
const uint16_t INVADER_START_ADDRESS_F = 0x1000;
const uint16_t INVADER_START_ADDRESS_E = 0x1800;


// invader file end memory address
const uint16_t INVADER_END_ADDRESS_H = 0x07FF; // End address for the invader ROM
const uint16_t INVADER_END_ADDRESS_G = 0x0FFF;
const uint16_t INVADER_END_ADDRESS_F = 0x17FF;
const uint16_t INVADER_END_ADDRESS_E = 0x1FFF;

class I8080 {
    public:
        I8080();
        ~I8080();

        // 8-bit Registers
        uint8_t A; // Accumulator

        uint16_t BC; // BC Register
        uint8_t B; // B Register
        uint8_t C; // C Register

        uint16_t DE; // DE Register
        uint8_t D; // D Register
        uint8_t E; // E Register

        uint16_t HL; // HL Register
        uint8_t H; // H Register
        uint8_t L; // L Register

        // 16-bit Registers
        uint16_t PC; // Program Counter
        uint16_t SP; // Stack Pointer

        // Flags
        bool carry;      // Carry Flag
        bool zero;       // Zero Flag
        bool sign;       // Sign Flag
        bool parity;     // Parity Flag
        bool aux_carry;  // Auxiliary Carry Flag

        // memory
        uint8_t memory[MEMORY_SIZE];

        // Video

		// I/O
		uint8_t InPort[4];
		uint8_t OutPort[7];

        // Cycles
        uint32_t cycles = 0;

        // Helper stuff
        bool exit; // Signals main to exit loop
        bool halt; // Signals CPU to halt until interrupt (not functional)

        // Interrupts
        bool interruptsEnabled = false;
        
        // Shift Register
        uint16_t shift_register = 0x0000;  // For port3
        uint8_t shift_offset = 0x00;       // For port3 handling
        
        // Instructions
        void executeInstruction(uint8_t opcode);
        void RegularInstruction();

        // LoadRom
        void loadROM(const char* path, uint16_t startAddress);

        // Interrupts
        void Interrupts(uint8_t opcode);

        // Cycles
        void Cycle(uint32_t num_cycles);

        // Instructions Carry Bits
        void INT_CMC();
        void INT_STC();

        // SINGLE REGISTER INSTRUCTIONS
        void INT_INR(uint8_t* addr);
        void INT_INR_MEM(uint8_t* addr);
        void INT_DCR(uint8_t* addr);
        void INT_DCR_MEM(uint8_t* addr);
        void INT_CMA();
        void INT_DAA();

        // NOP INSTRUCTIONS
        void INT_NOP();

        // DATA TRANSFER INSTRUCTIONS
        void INT_MOV_REG(uint8_t* dest, uint8_t* src);
        void INT_MOV_MEM(uint8_t* addr, bool isSrc);
        void INT_STAX( bool isBC);
        void INT_LDAX(bool isBC);

        // REGISTER OR MEMORY TO ACCUMULATOR INSTRUCTIONS
        void INT_ADD(uint8_t* addr);
        void INT_ADD_MEM();
        void INT_ADC(uint8_t* addr);
        void INT_ADC_MEM();
        void INT_SUB(uint8_t* addr);
        void INT_SUB_MEM();
        void INT_SBB(uint8_t* addr);
        void INT_SBB_MEM();
        void INT_ANA(uint8_t* addr);
        void INT_ANA_MEM();
        void INT_XRA(uint8_t* addr);
        void INT_XRA_MEM();
        void INT_ORA(uint8_t* addr);
        void INT_ORA_MEM();
        void INT_CMP(uint8_t* addr);
        void INT_CMP_MEM();

        // ROTATE ACCUMULATOR INSTRUCTIONS
        void INT_RLC();
        void INT_RRC();
        void INT_RAL();
        void INT_RAR();

        // REGISTER PAl R INSTRUCTIONS
        void INT_PUSH(uint8_t* highReg, uint8_t* lowReg);
        void INT_PUSH_PSW();
        void INT_POP(uint8_t* highReg, uint8_t* lowReg);
        void INT_POP_PSW();
        void INT_DAD(uint16_t* reg);
        void INT_INX(uint8_t* highReg, uint8_t* lowReg);
        void INT_INX_SP();
        void INT_DCX(uint8_t* highReg, uint8_t* lowReg);
        void INT_DCX_SP();
        void INT_XCHG();
        void INT_XTHL();
        void INT_SPHL();
        
        // IMMEDIATE INSTRUCTIONS
        void INT_LXI(uint8_t* reg1, uint8_t* reg2, uint8_t high, uint8_t low);
        void INT_LXI_SP(uint8_t high, uint8_t low);
        void INT_MVI(uint8_t* dest, uint8_t value);
        void INT_MVI_MEM(uint8_t* addr, uint8_t value);
        void INT_ADI(uint8_t value);
        void INT_ACI(uint8_t value);
        void INT_SUI(uint8_t value);
        void INT_SBI(uint8_t value);
        void INT_ANI(uint8_t value);
        void INT_XRI(uint8_t value);
        void INT_ORI(uint8_t value);
        void INT_CPI(uint8_t value);

        // DIRECT ADDRESSING INSTRUCTIONS
        void INT_STA(uint16_t address);
        void INT_LDA(uint16_t address);
        void INT_SHLD(uint16_t address);
        void INT_LHLD(uint16_t address);

        // JUMP INSTRUCTIONS
        void INT_PCHL();
        void INT_JMP(uint16_t address);
        void INT_JC(uint16_t address);
        void INT_JNC(uint16_t address);
        void INT_JZ(uint16_t address);
        void INT_JNZ(uint16_t address);
        void INT_JM(uint16_t address);
        void INT_JP(uint16_t address);
        void INT_JPE(uint16_t address);
        void INT_JPO(uint16_t address);

        // CALL SUBROUTINE INSTRUCTIONS
        void INT_CALL(uint16_t address, bool cond);
        void INT_CC(uint16_t address);
        void INT_CNC(uint16_t address);
        void INT_CZ(uint16_t address);
        void INT_CNZ(uint16_t address);
        void INT_CM(uint16_t address);
        void INT_CP(uint16_t address);
        void INT_CPE(uint16_t address);
        void INT_CPO(uint16_t address);

        // RETURN FROM SUBROUTINE INSTRUCTIONS
        void INT_RET(bool cond);
        void INT_RC();
        void INT_RNC();
        void INT_RZ();
        void INT_RNZ();
        void INT_RM();
        void INT_RP();
        void INT_RPE();
        void INT_RPO();

        // RST INSTRUCTION
        void INT_RST(int address);

        // INTERRUPT FLIP-FLOP INSTRUCTIONS
        void INT_EI();
        void INT_DI();

        // Input/Output INSTRUCTIONS
        void INT_IN();
        void INT_OUT();

        // HLT INSTRUCTION
        void INT_HLT();

        // private:
            // Set all flag
            void setFlagsArith(uint16_t result );
            void setFlagZeroArith(uint16_t result );
            void setFlagSignArith(uint16_t result );
            void setFlagParityArith(uint16_t result );
            void setFlagAuxCarryArith(uint8_t num1, uint8_t num2, int operation);
            void setFlagCarryArith(uint16_t result );
            void setFlagAuxCarryArithCarry(uint8_t num1, uint8_t num2, uint8_t carry, int operation);

            // stack
            void pushStackCall( );
            void popStackCall( );
};

#endif // I8080_H