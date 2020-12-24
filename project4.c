#include "spimcore.h"
#include <math.h>

/* ALU */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
	/*
	1. Implement the operations on input parameters A and B according to ALUControl.
	2. Output the result (Z) to ALUresult.
	3. Assign Zero to 1 if the result is zero; otherwise, assign 0.
	*/

	if (ALUControl == 000)
	{
		//Z = A + B
		*ALUresult = A + B;
	}
	else if(ALUControl == 001)
	{
		//Z = A - B
		*ALUresult = A - B;
	}
	else if (ALUControl == 010)
	{
		//If A < B, Z = 1; otherwise, Z = 0 (A and B are unsigned integers)
		if (A < B)
		{
			*ALUresult = 1;
		}
		else
		{
			*ALUresult = 0;
		}
	}
	else if (ALUControl == 011)
	{
		//Z = A AND B
		*ALUresult = (A & B);
	}
	else if (ALUControl == 101)
	{
		//Z = A OR B
		*ALUresult = (A | B);
	}
	else if (ALUControl == 110)
	{
		//Shift left B by 16 bits
		B = B << 16;
	}
	else if (ALUControl == 111)
	{
		//Z = NOT A
		*ALUresult = ~A;
	}
	else
	{
		printf("\n");
	}

	if (*ALUresult == 0)
	{
		*Zero = 1;
	}
	else
	{
		*Zero = 0;
	}
}


// Each function simulates the operations of a section of the datapath.

/*
	In spimcore.c, the function Step() is the core function of the MySPIM. This function invokes the 9
	functions that you are required to implement to simulate the signals and data passing between
	the components of the datapath.
*/

/* instruction fetch */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{

	/*
	  1. Fetch the instruction addressed by PC from Mem and write it to instruction.
	  2. Return 1 if a halt condition occurs; otherwise, return 0
	*/

	if (PC > 65535 | PC & 0x3) 
	{
		return 1;
	}

	*instruction = Mem[(PC >> 2)];

	return 0;
}


/* instruction partition */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
	//Instruction to Control [26-36]
	*op = (instruction >> 26) & 0x0000003f;

	//Register 1 to Registers [21-25]
	*r1 = (instruction >> 21) & 0x0000001F;

	//Register 2 to Registers [16-20]
	*r2 = (instruction >> 16) & 0x0000001F;

	//Register 3 to Registers [11-15]
	*r3 = (instruction >> 11) & 0x0000001F;

	//Instruction to Sign-Extend [0-5]
	*funct = instruction & 0x0000003F;

	//Instruction [0-15]
	*offset = instruction & 0X0000FFFF;

	//Instruction [0-25]
	*jsec = instruction & 0x03FFFFFF;
}



/* instruction decode */
int instruction_decode(unsigned op,struct_controls *controls)
{
	/*
      Control Signals:
		-->>MemRead, MemWrite or RegWrite: 1 enabled, 0 disabled, 2 �don�t care�
		-->>RegDst, Jump, Branch, MemtoReg or ALUSrc: 0/1 indicates the selected path of the multiplexer; 2 �don�t care�

	  ALUOp
	  ===============================================================
	  000 (op) ALU will do addition or "don't care"
	  001 (op 0x00) ALU will do subtraction
	  010 (op 0x00) ALU will do "set less than" operation
	  011 (op 0x00) ALU will do "set less than unsigned operation"
	  100 (op 0x00) ALU will do "AND" operation
	  101 (op 0x00) ALU will do "OR" operation
	  110 (op) ALU will shift left extended_value by 16 bits
	  111 (op 0x00) The instruction is an R-type instruction

	  R-types:
	  -->>add, sub, and, or, slt, slt unsinged -->> 0

	  I-type:
	  -->>addi 001000
	  -->>load word 100011
	  -->>store word 101011
	  -->>load upper immediate 001111
	  -->>branch on equal 000100
	  -->>slt immediate 001010
	  -->>slt immediate unsigned 001011

	  J-types:
	  -->>Jump 000010

	*/

	//Used for halting on unused opcode
	int flag = 0;

	switch (op)
	{
		//ASK the other ALUop with R-TYPE TO PROFESSOR
		case 0x00:
			/*
			  R-Type Instructions -->> AND, ORR, ADD, SUBSTRACT, MULTIPLY, DIVICE NOR, XOR, SLT, LSL, LSR
			  ALUop = 0111 (7) for all R-types
			  RegDst = 1 for all R-types
			  RegWrite = 1 for all R-types
			  ALUSrc = 0 for all R-types
			*/
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

		case 0x08:
			/*
			  ADD immediate (addi)
			  I-type instruction
			  ALUop = 0000 (0)
			*/
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

		case 0x0c:
			/*
			  AND immediate (andi)
			  I-type instruction
			  ALUop = 0100 (4)
			*/
			controls->RegDst = 0;
			controls->Jump = 0;
			controls->Branch = 0;
			controls->MemRead = 0;
			controls->MemtoReg = 0;
			controls->ALUOp = 4;
			controls->MemWrite = 0;
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			break;

		case 0x0d:
			/*
			  OR immediate (ori)
			  I-type instruction
			  ALUop = 0101 (5)
			*/
			controls->RegDst = 0;
			controls->Jump = 0;
			controls->Branch = 0;
			controls->MemRead = 0;
			controls->MemtoReg = 0;
			controls->ALUOp = 5;
			controls->MemWrite = 0;
			controls->ALUSrc = 1;
			controls->RegWrite = 1;
			break;

		case 0x0a:
			/*
			  Set less than immediate (slti)
			  I-type instruction
			  ALUop = 0010 (2)
			*/
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

		case 0x0b:
			/*
			Set less than immediate unsigned(sltiu)
			I-type instruction
			ALUop = 0011 (2)
			*/
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

		case 0x04:
			/*
			  Branch if Equal (beq)
			  I-type instruction
			  ALUop = 0101 (5)
		    */
			controls->RegDst = 2;
			controls->Jump = 0;
			controls->Branch = 1;
			controls->MemRead = 0;
			controls->MemtoReg = 2;
			controls->ALUOp = 0;
			controls->MemWrite = 0;
			controls->ALUSrc = 1;
			controls->RegWrite = 0;
			break;

		case 0x23:
			/*
			  Load Word (lw)
			  I-type instruction
			  ALUop = 0000 (0)
		    */
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

		case 0x0f:
			/*
				Load Upper Immediate(lui)
				I-type instruction
				ALUop = 0110 (6)
			*/
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

		case 0x2b:
			/*
			  Store Word (sw)
			  I-type instruction
			  ALUop = 0000 (0)
			*/
			controls->RegDst = 2;
			controls->Jump = 0;
			controls->Branch = 0;
			controls->MemRead = 0;
			controls->MemtoReg = 2;
			controls->ALUOp = 0;
			controls->MemWrite = 1;
			controls->ALUSrc = 1;
			controls->RegWrite = 0;
			break;	

		case 0x01:
			/*
			  Branch if Equal or Greater (bge)
			  I-type instruction
			  ALUop = 0110(6)
			*/
			controls->RegDst = 0;
			controls->Jump = 0;
			controls->Branch = 1;
			controls->MemRead = 0;
			controls->MemtoReg = 0;
			controls->ALUOp = 6;
			controls->MemWrite = 0;
			controls->ALUSrc = 1;
			controls->RegWrite = 0;
			break;

		case 0x02:
			/*
			  Jump (j)
			  I-type instruction
			  ALUop = 0000 (0)
			*/
			controls->RegDst = 2;
			controls->Jump = 1;
			controls->Branch = 2;
			controls->MemRead = 2;
			controls->MemtoReg = 2;
			controls->ALUOp = 0;
			controls->MemWrite = 0;
			controls->ALUSrc = 2;
			controls->RegWrite = 0;
			break;

		default:
			flag = 1;
			break;
	}

	return flag;
}

/* Read Register */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	/*
	  Read the registers addressed by r1 and r2 from Reg, and write the read values to
      data1 and data2 respectively.
	*/

	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
void sign_extend(unsigned offset,unsigned *extended_value)
{
	/*
	  Assign the sign-extended value of offset to extended_value
	*/

    unsigned negative = offset >> 15;
    //if the number is negative (MSB = 1), extend with 1s
    if(negative == 1)
        *extended_value = offset | 0xFFFF0000;
    //otherwise extend with 0's
    else
        *extended_value = offset & 0x0000FFFF;
}

/* ALU operations */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
	/*
	  1. Apply ALU operations on data1, and data2 or extended_value (determined by ALUSrc).
	  2. The operation performed is based on ALUOp and funct.
	  3. Apply the function ALU(�).
	  4. Output the result to ALUresult.
	  5. Return 1 if a halt condition occurs; otherwise, return 0.
	*/
      
	if (ALUSrc == 1) 
	{
		// if the ALUSrc is 1, set data2 to extended_value
		data2 = extended_value;
	}

	// ALUop 7 is R-type instruction
    if (ALUOp == 7) 
	{
    	// set the correct ALUOp for each R-type instruction
    	
		switch(funct) 
		{
	    	case 32:
				// add
	    		ALUOp = 0;
	    		break;
	   
	    	case 34:
				// subtract
	    		ALUOp = 1;
	    		break;
	    	
	    	case 42:
				// set less than signed
	    		ALUOp = 2;
	    		break;
	    	
	    	case 43:
				// set less than unsigned
	    		ALUOp = 3;
	    		break;
	    
	    	case 36:
				// and
	    		ALUOp = 4;
	    		break;
				
	    	case 37:
				// or
	    		ALUOp = 5;
	    		break;
	    	
	    	case 4:
				// shift left right variable
	    		ALUOp = 6;
	    		break;
	    	
	    	case 39:
				// nor
	    		ALUOp = 7;
	    		break;
	    	
	    	default:
	    		return 1;
	    }

	    // call ALU function
	    ALU(data1, data2, ALUOp, ALUresult, Zero);
    } 
	else
	{
    	// call ALU for non-functions
    	ALU(data1, data2, ALUOp, ALUresult, Zero);
    }

    return 0;
}

/* Read / Write Memory */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{
	/*
	  1. Base on the value of MemWrite or MemRead to determine memory write operation or memory read operation.
	  2. Read the content of the memory location addressed by ALUresult to memdata.
	  3. Write the value of data2 to the memory location addressed by ALUresult.
	  4. Return 1 if a halt condition occurs; otherwise, return 0.
	*/
    
	//if we are reading,
    //Read data from ALUresult * 4 index in Memory.
    if(MemRead == 1)
	{
        if((ALUresult % 4) == 0)
		{
            *memdata = Mem[ALUresult >> 2];
        }
        else
            return 1;
    }

    //if we are writing,
    //Write data to ALUresult * 4 index in Memory
    //Word aligned
    if(MemWrite == 1)
	{
        if((ALUresult % 4) == 0)
		{
            Mem[ALUresult >> 2] = data2;
        }
        else
            return 1;
    }

    return 0;
}


/* Write Register */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	/*
	  Write the data (ALUresult or memdata) to a register (Reg) addressed by r2 or r3
	*/

	if (RegWrite == 1)
	{
		//Memory to register
		if (MemtoReg == 1)
		{
			if (RegDst == 0)
			{
				Reg[r2] = memdata;
			}
			else //if (RegDst == 1)
			{
				Reg[r3] = memdata;
			}
		}
		else //if (MemtoReg == 0)
		{
			if (RegDst == 0)
			{
				Reg[r2] = ALUresult;
			}
			else //if (RegDst == 1)
			{
				Reg[r3] = ALUresult;
			}
		}
	}
}

/* PC update */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	/*
	 *Update the program counter (PC)
	*/
    
	//always increment by 4 no matter what.
		*PC += 4;
	
    //if branching, and we got a zero, add the extended_value, bitshifted (multiplied by 4)
	if (Zero && Branch)
	{
		*PC += extended_value << 2;
	}

    //if jumping, jump to the Jump register, and multiply by 4 (word align)
	if (Jump)
	{
		*PC = (jsec << 2) | (*PC & 0xf0000000);
	}
}

