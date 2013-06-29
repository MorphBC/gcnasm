/*
 * AMD GCN ISA Assembler
 *
 * SOP2 instruction parser
 *
 * This software is Copyright 2013, Daniel Bali <balijanosdaniel at gmail.com>,
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "sop2.h"

/**
 * Parses instructions with a SOP2 encoding
 * 
 * MAGIC (2) | OP (7) | SDST (7) | SSRC1 (8) | SSRC0 (8) | [LITERAL (32)]
 */
isa_op_code parseSOP2Instruction(isa_instr instr, char **args)
{
	char *sdst_str, *ssrc1_str, *ssrc0_str;
	isa_operand sdst_op, ssrc1_op, ssrc0_op;	// ISA operand structs
	isa_op_code op_code;					// Generated opcode struct
	
	// Setup arguments
	sdst_str	= args[0];
	ssrc0_str	= args[1];
	ssrc1_str	= args[2];

	// Parse operands
	op_code.code = instr.op_code;
	op_code.literal_set = 0;

	// SDST
	sdst_op = parseSOP2Operand(sdst_str);

	if (sdst_op.op_type.type >= SDST_OPERAND_TRESHOLD)
		ERROR("incorrect value for SDST operand");

	op_code.code |= sdst_op.op_code << 16;

	// SSRC0 - First source operand
	ssrc0_op = parseSOP2Operand(ssrc0_str);

	if (ssrc0_op.op_type.type == LITERAL)
		setLiteralOperand(&op_code, ssrc0_op);

	op_code.code |= ssrc0_op.op_code;	

	// SSRC1 - Second source operand
	ssrc1_op = parseSOP2Operand(ssrc1_str);

	if (ssrc1_op.op_type.type == LITERAL)
		setLiteralOperand(&op_code, ssrc1_op);

	op_code.code |= ssrc1_op.op_code << 8;

	return op_code;
}

/**
 * Parses SOP type operands
 */
isa_operand parseSOP2Operand(char *op_str)
{
	isa_operand result;
	char *end;
	int i;

	// Look up simple (built-in) operand types
	for (i = 0; i < isa_simple_operand_count; ++i)
		if (strncmp(op_str, isa_simple_operand_list[i].name, 
				strlen(isa_simple_operand_list[i].name)) == 0)
			break;

	if (i < isa_simple_operand_count)
	{
		result.op_code = isa_simple_operand_list[i].op_code;
		result.op_type = isa_simple_operand_list[i];
		return result;
	}


	if (tolower(op_str[0]) == 's')
	{
		// Parse SGPR operand
		result.value = strtol((const char*) op_str+1, &end, 10);

		if (*end)
			ERROR("parsing operand (SGPR value)");

		if (result.value < 0 || result.value > 103)
			ERROR("invalid SGPR number (%d)", result.value);

		result.op_code = SGPR_OP.op_code + result.value;
		result.op_type = SGPR_OP;

		return result; 
	}
	else if (tolower(op_str[0]) == 't')
	{
		// Parse TTMP operand
		result.value = strtol((const char*) op_str+1, &end, 10);

		if (*end)
			ERROR("parsing operand (TTMP value)");

		if (result.value < 0 || result.value > 11)
			ERROR("invalid TTMP number (%d)", result.value);

		result.op_code = TTMP_OP.op_code + result.value;
		result.op_type = TTMP_OP;

		return result; 
	}
	else
	{
		// Parse literal constant

		if (strncmp(op_str, "0x", 2) == 0)
		{
			result.value = strtol((const char*) op_str+2, &end, 16);
		}
		else
		{
			result.value = strtol((const char*) op_str, &end, 10);
		}

		if (*end)
			ERROR("parsing operand (literal value)");

		if (result.value == 0)
		{

			result.op_code = ZERO_OP.op_code;
			result.op_type = ZERO_OP;
		}
		else if (result.value > 0 && result.value <= 64)
		{
			result.op_code = INL_POS_OP.op_code + result.value - 1;
			result.op_type = INL_POS_OP;
		}
		else if (result.value >= -16 && result.value <= -1)
		{
			result.op_code = INL_NEG_OP.op_code + (-result.value) - 1;
			result.op_type = INL_NEG_OP;
		}
		else
		{
			result.op_code = LITERAL_OP.op_code;
			result.op_type = LITERAL_OP;
		}

		return result;
	}

	// At this stage this is unreachable code

	ERROR("unsupported operand type (%s", op_str);
	return result;
}