/*
Jesus Ugarte
Miguel Dautant
Gianfranco Santinato
project.c
CDA 3103 - Computer Logic and Organization
Professor Angell*/

#include "spimcore.h"

/* ALU */
/* 10 Points */
void ALU(
	unsigned A,
	unsigned B,
	char ALUControl,
	unsigned *ALUresult,
	char *Zero) 
{
	// Used for beq to see if A and B are equal
	if ((A - B) == 0)
		*Zero = 1;
	else
		*Zero = 0;

  // Switch to see which ALU control gets executed
	switch ((int)ALUControl) 
  {
    case 0: // Z = A + B
      *ALUresult = A + B;
      break;

    case 1: // Z = A – B
      *ALUresult = A - B;
      break;

    case 2: // if A < B, Z = 1; otherwise, Z = 0 SIGNED
      if ((int)A < (int)B)
        *ALUresult = 1;
      else
        *ALUresult = 0;
      break;

    case 3: // if A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
        // UNSIGNED
      if (A < B)
        *ALUresult = 1;
      else
        *ALUresult = 0;
      break;

    case 4: // Z = A AND B
      *ALUresult = A & B;
      break;

    case 5: // Z = A OR B
      *ALUresult = A | B;
      break;

    case 6: // Shift left B by 16 bits
      *ALUresult = B << 16;
      break;

    case 7: // Z = NOT A
      *ALUresult = ~A;
      break;
	}
}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC, unsigned *Mem, unsigned *instruction) 
{
	unsigned value;
	// Multiply by 4 to get the proper address in memory
	value = PC >> 2;
	
  // Check number for word alignedness
	if (PC % 4 == 0)
		*instruction = Mem[value]; // We extract the instruction
	else						   
		return 1; // Return 1 if unsigned PC is not word aligned

	if (PC % 4 == 0)
		*instruction = Mem[PC >> 2];
	else
		return 1;

	return 0;
}

/* instruction partition */
/* 10 Points */
void instruction_partition(
	unsigned instruction,
	unsigned *op,
	unsigned *r1,
	unsigned *r2,
	unsigned *r3,
	unsigned *funct,
	unsigned *offset,
	unsigned *jsec) 
{
	//	The areas with 1s correspond to the bits of the words that we extracted from the instruction
	*op = (instruction & 0xfc000000) >> 26; // 1111 1100 0000 0000 0000 0000 0000 0000
	*r1 = (instruction & 0x03e00000) >> 21; // 0000 0011 1110 0000 0000 0000 0000 0000
	*r2 = (instruction & 0x001f0000) >> 16; // 0000 0000 0001 1111 0000 0000 0000 0000
	*r3 = (instruction & 0x0000f800) >> 11; // 0000 0000 0000 0000 1111 1000 0000 0000
	*funct = instruction & 0x0000003f;	// 0000 0000 0000 0000 0000 0000 0011 1111
	*offset = instruction & 0x0000ffff;   // 0000 0000 0000 0000 1111 1111 1111 1111
	*jsec = instruction & 0x03ffffff;	 // 0000 0011 1111 1111 1111 1111 1111 1111
}

/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op, struct_controls *controls) 
{
  //Switch for different operations and find the values that go into the control
	switch (op) 
  {
    case 0x0: // add, sub, and, or, slt, sltu
      controls->RegDst = 1; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 7;
			controls->MemWrite = 0; 
			controls->ALUSrc = 0; 
			controls->RegWrite = 1;
      break;

    case 0x8: // add immediate 
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 0;
			controls->MemWrite = 0; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 1;
      break;

    case 0x23: // load word
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 1; 
			controls->MemtoReg = 1;
			controls->ALUOp = 0;
			controls->MemWrite = 0; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 1;
      break;

    case 0x2b: // store word
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 0;
			controls->MemWrite = 1; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 0;
      break;

    case 0xf: // load unsigned immediate 
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 6;
			controls->MemWrite = 0; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 1;
      break;

    case 0x4: // branch equal
      controls->RegDst = 2; 
      controls->Jump = 0; 
			controls->Branch = 1; 
			controls->MemRead = 0; 
			controls->MemtoReg = 2;
			controls->ALUOp = 1;
			controls->MemWrite = 0; 
			controls->ALUSrc = 2; 
			controls->RegWrite = 0;
      break;

    case 0xa: // slti
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 2;
			controls->MemWrite = 0; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 1;
      break;

    case 0xb: // sltiu
      controls->RegDst = 0; 
      controls->Jump = 0; 
			controls->Branch = 0; 
			controls->MemRead = 0; 
			controls->MemtoReg = 0;
			controls->ALUOp = 3;
			controls->MemWrite = 0; 
			controls->ALUSrc = 1; 
			controls->RegWrite = 1;
      break;

    case 0x2: // jump
      controls->RegDst = 2; 
      controls->Jump = 1; 
			controls->Branch = 2; 
			controls->MemRead = 0; 
			controls->MemtoReg = 2;
			controls->ALUOp = 0;
			controls->MemWrite = 0; 
			controls->ALUSrc = 2; 
			controls->RegWrite = 0;
      break;

    default:
      return 1;
   }
	return 0;
}

/* Read Register */
/* 5 Points */
void read_register(
	unsigned r1,
	unsigned r2,
	unsigned *Reg,
	unsigned *data1,
	unsigned *data2) 
{
  //Extracting contents from the registers
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}
/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset, unsigned *extended_value) 
{
  // If it is negative then make the last 16 bits
	if ((offset >> 15) == 1)
		*extended_value = offset | 0xffff0000; 

  // If it is postitive then make the last 16
	else
		*extended_value = offset & 0x0000ffff;
}

/* ALU operations */
/* 10 Points */
int ALU_operations(
	unsigned data1,
	unsigned data2,
	unsigned extended_value,
	unsigned funct,
	char ALUOp,
	char ALUSrc,
	unsigned *ALUresult,
	char *Zero) 
{
	unsigned char ALUControl = ALUOp;

	switch (ALUOp) 
  {
    case 0x0:  // Addition
    case 0x1:  // Subtraction
    case 0x2:  // Set it less than signed (slt)
    case 0x3:  // Set it less than unsigned (sltu)
    case 0x4:  // and
    case 0x5:  // or
    case 0x6:  // Shift extended value by 16 bits
      break; // One break needed because all cases go until case 7
    case 0x7:  // R-Type instructions
      switch (funct) 
      {
        case 0x20: // add
          ALUControl = 0x0;
          break;
        case 0x24: // and
          ALUControl = 0x4;
          break;
        case 0x25: // or
          ALUControl = 0x5;
          break;
        case 0x2a: // set less than signed (slt)
          ALUControl = 0x2;
          break;
        case 0x2b: // set less than unsigned (sltu)
          ALUControl = 0x3;
          break;
        default: // Whatever other command used will halt the program
          return 1;
      }
      break;
    default:
      return 1;
	}
	// if ALUsrc is 1, we call ALU with extended value, else call it with data2
	if(ALUSrc == 1)
    ALU(data1, extended_value, ALUControl, ALUresult, Zero);
  else
    ALU(data1, data2, ALUControl, ALUresult, Zero);

	return 0;
}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(
	unsigned ALUresult,
	unsigned data2,
	char MemWrite,
	char MemRead,
	unsigned *memdata,
	unsigned *Mem) 
{
  // If we are reading from the memory
	if (MemRead == 1) 
  {
		if (ALUresult % 4 == 0 && ALUresult < 65536)
			*memdata = Mem[ALUresult >> 2];
		else
			return 1;
	}
  // If we are writing to the memory
	if (MemWrite == 1) 
  {
		if (ALUresult % 4 == 0 && ALUresult < 65536)
			Mem[ALUresult >> 2] = data2;
		else
			return 1;
	}
	return 0;
}

/* Write Register */
/* 10 Points */
void write_register(
	unsigned r2,
	unsigned r3,
	unsigned memdata,
	unsigned ALUresult,
	char RegWrite,
	char RegDst,
	char MemtoReg,
	unsigned *Reg) 
{
  // The information given will be written to a register
	if (RegWrite == 1) 
  {
		if (MemtoReg == 1) 
      // The information will be comming from the memory
			Reg[r2] = memdata;

    // The information will be coming from a register
		else if (MemtoReg == 0)
    { 
			if (RegDst == 1)
				// R-Type format so it can write to register r3
				Reg[r3] = ALUresult;
			else
				// write to the register r2
				Reg[r2] = ALUresult;
		}
	}
}

/* PC update */
/* 10 Points */
// This is to update the instruction the PC has to read.
void PC_update(
	unsigned jsec,
	unsigned extended_value,
	char Branch,
	char Jump,
	char Zero,
	unsigned *PC) 
{
  // Incrementing the PC by 4 ALWAYS
	*PC += 4;
  // If we branching and a Zero, add extended value
	if (Zero == 1 && Branch == 1)
		*PC = *PC + (extended_value << 2);
  // If we jumping, we shift instruction and combine it with PC
	if (Jump == 1)
		*PC = (*PC & 0xf0000000) | (jsec << 2);
}