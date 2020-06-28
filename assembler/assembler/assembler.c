
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1 
#define FALSE 0

#define MAX_LINES 4096 
#define MAX_LINE_LEN 501 // +1 for '\0'
#define MAX_LABEL_LEN 51 // +1 for '\0'
#define MAX_TOKENS_IN_LINE 6

#define OPCODES { "add", "sub", "and" ,"or", "sll", "sra", "srl", "beq", "bne", "blt", "bgt", "ble", "bge", "jal", "lw", "sw", "reti", "in", "out", "halt", ".word"} 
// all values' indices match their opcode number
#define OPCODES_LEN 21 // the length of the array above

#define REGS {"$zero", "$imm", "$v0", "$a0", "$a1", "$t0", "$t1", "$t2", "$t3", "$s0", "$s1", "$s2", "$gp", "$sp", "$fp", "$ra"}
// all registers' indices match their number
#define REGS_LEN 16 // the length of the array above


typedef	struct label {

	char name[MAX_LABEL_LEN];
	int address;

} Label;


void read_file(FILE* asm_file, int run_num);

void write_to_memin(FILE* out_file);

void correct_line(char* line, char* corrected_line);

int update_PC(char* tokens[]);

void get_tokens(char* line, char* tokens[]);

void update_labels(char* tokens[], int PC);

void write_instruction(char* tokens[], int PC);

int ipow(int base, int exp);

int get_imm(char* str);

int check_value(char* line_start);

int get_reg(char* reg);

int is_equal_str(char* str1, char* str2);

int str2int(char* str);


Label Labels[MAX_LINES]; // stores labels name and addresses 
int label_index = 0; //first empty label index in labels array 

unsigned int Memory[MAX_LINES] = { 0 }; // represents the memory, in the end will be written to memin.txt
// int range isn't large enough to contain the memory output
int mem_index = 0; // current empty slot in memory array 
int mem_end = 0; // store index of the last non-zero slot


int main(int argc, char** argv)
{
	FILE* asm_file = NULL;
	FILE* memin = NULL;

	if (argc < 3)
	{
		printf("Argument amount error\n");
		return 1;
	}

	asm_file = fopen(argv[1], "r");
	memin = fopen(argv[2], "w");

	if (asm_file == NULL || memin == NULL)
	{
		printf("Couldn't open file, terminating process\n");
		return 1;
	}

	// read file, perform first run 
	read_file(asm_file, 1);

	// read file, perform second run 
	read_file(asm_file, 2);

	// write from memory array to memin.txt 
	write_to_memin(memin);
	fclose(asm_file);
	fclose(memin);
}


// performs first run and second run over the code
// run_num = 1 -> first run  -> get labels      (get the labels and their addresses)
// run_num = 2 -> second run -> write to memory (write instructions)
void read_file(FILE* asm_file, int run_num)
{
	char line[MAX_LINE_LEN]; // holds line from file
	char corrected_line[MAX_LINE_LEN + 1]; // holds line after correction, +1 for the additional whitespace

	char* tokens[MAX_TOKENS_IN_LINE]; // tokens, will hold parts of the line

	int PC = 0;				   // current PC 

	while (fgets(line, MAX_LINE_LEN, asm_file) != NULL) // read file line by line 
	{
		correct_line(line, corrected_line);
		get_tokens(corrected_line, tokens);

		// first run - get labels
		if (run_num == 1)
			update_labels(tokens, PC); // first run function, needs to be written

		// second run - get instructions in hex and write in file
		else
			write_instruction(tokens, PC); // second run function needs to be written

		// update Program Counter - needs to be written
		PC += update_PC(tokens);

	}
	rewind(asm_file);
}


// writes to out_file the contents of the memory array 
void write_to_memin(FILE* out_file)
{
	for (int i = 0; i < mem_end - 1; i++)
		fprintf(out_file, "%08X\n", Memory[i]);
	
	fprintf(out_file, "%08X\n", Memory[mem_end - 1]);
}

// corrects line from file if there is no ' ' after ':', so we won't read the label and opcode as one token 
// and removes whitespaces between label name and ':'
void correct_line(char* line, char* corrected_line)
{
	strcpy(corrected_line, line);

	char* colon_index1 = strchr(corrected_line, ':'); // check if there is ':' in line
	if (colon_index1 != NULL) // if there is -> make sure there are no whitespaces before it and at least one whitespace after it 
	{
		while (*(colon_index1 - 1) == ' ' || *(colon_index1 - 1) == '\t') // make sure there are no whitespaces before ':' 
		{
			*(colon_index1 - 1) = ':';
			*(colon_index1) = ' ';
			colon_index1--;
		}

		if (*(colon_index1 + 1) != ' ' || *(colon_index1 + 1) != '\t') // make sure there is at least one whitespaces after ':'  
		{
			*(colon_index1 + 1) = ' ';
			char* colon_index2 = strchr(line, ':');

			strcpy(colon_index1 + 2, colon_index2 + 1);
		}
	}
}


// recieves tokens of the line and returns how much we should add to the PC
int update_PC(char* tokens[])
{
	int first_token = check_value(tokens[0]);
	int opcode;
	int rd_index = 2; // token index of the first register in the instruction 

	if (first_token == -2) return 0; // check if empty line 

	if (first_token == -1)  // check if first token is label
	{
		opcode = check_value(tokens[1]); // first token is label -> second one is opcode if exists

		if (opcode == -2)  // check if second token is NULL 
			return 0;
	}
	else  // first token is opcode
	{
		opcode = first_token;
		rd_index = 1;
	}
	if (opcode == 20)	// if opcode is ".word"
		return 0;

	else
		return 1;
}


// splits line into tokens 
void get_tokens(char* line, char* tokens[])
{
	char* delimiters = " \n\t,";    // line delimiters 

	char* number_sign_index = NULL;

	int is_comment_start = FALSE;

	int tokens_index = 0;

	char* token = strtok(line, delimiters);

	while (token != NULL && *token != '#') // check if token is not NULL and doesnt begin with '#' 
	{
		number_sign_index = strchr(token, '#'); // number_sign - '#', holds address of '#' in token if exists

		if (number_sign_index != NULL)  // check if token has '#'
		{
			*number_sign_index = '\0';  // truncate token 
			is_comment_start = TRUE;    // indicates that after current token a comment has started
		}

		tokens[tokens_index] = token;
		tokens_index++;

		if (is_comment_start) // if a comment has started -> no more tokens 
			break;

		token = strtok(NULL, delimiters); // read next token 
	}

	for (int i = tokens_index; i < MAX_TOKENS_IN_LINE; i++) // set rest tokens to NULL 
		tokens[i] = NULL;
}


// updates the labels array 
void update_labels(char* tokens[], int PC)
{
	if (check_value(tokens[0]) != -1) // check if label is present in the line 
		return;

	strcpy(Labels[label_index].name, tokens[0]); //add label to the array 
	Labels[label_index].address = PC;

	int i = 0;
	while (Labels[label_index].name[i] != ':')
		i++;

	Labels[label_index].name[i] = '\0';

	label_index++;
}


// converts instruction to hex and writes to Memory array - second run
void write_instruction(char* tokens[], int PC)
{
	int opcode, rd, rs, rt, imm = 0;
	int first_token, index_offset = 0;

	first_token = check_value(tokens[0]);

	if (first_token == -2) return; // check if empty line 

	if (first_token == -1)  // check if first token is label
	{
		index_offset = 1; // instruction starts after first token

		int second_token = check_value(tokens[1]);

		if (second_token == -2)  // check if second token is NULL 
			return;
	}

	opcode = check_value(tokens[0 + index_offset]);

	if (opcode == 20) // if opcode is ".word"
	{
		int address = str2int(tokens[1 + index_offset]); // convert address to int 
		int data = str2int(tokens[2 + index_offset]);    // convert data to int  

		Memory[address] = data;

		if (address + 1 > mem_end)
			mem_end = address + 1;
		return;
	}

	rd = get_reg(tokens[1 + index_offset]);
	rs = get_reg(tokens[2 + index_offset]);
	rt = get_reg(tokens[3 + index_offset]);

	imm = get_imm(tokens[4 + index_offset]);
	if (imm < 0)
		imm = 4096 + imm;

	Memory[mem_index] = (opcode * ipow(16, 6)) + rd * ipow(16, 5) + rs * ipow(16, 4) + rt * ipow(16, 3) + imm;
	mem_index++;

	if (mem_index > mem_end)
		mem_end = mem_index;
}


// int power of int base and exp
int ipow(int base, int exp)
{
	int result = 1;
	while (exp > 0)
	{
		result *= base;
		exp--;
	}
	return result;
}


// converts imm field in instruction to the appropriate value 
int get_imm(char* str)
{
	// check if imm is label
	for (int i = 0; i < label_index; i++) // check if imm is label, if it is return address
		if (strcmp(str, Labels[i].name) == 0)
			return Labels[i].address;

	return str2int(str);
}


// a line starts with label, an opcode or ".word"
// the function recieves the first word in the line and returns:
// its opcode number if its an opcode, 20 if it's ".word",
// -1 if it's a label and -2 if it's NULL 
int check_value(char* line_start)
{
	if (line_start == NULL) return -2;

	char* has_colon = strchr(line_start, ':'); // checks if token has the ':' at the end of it 

	if (has_colon != NULL) return -1;

	char* opcodes[] = OPCODES;

	for (int i = 0; i < OPCODES_LEN; i++)
		if (is_equal_str(line_start, opcodes[i]))
			return i;

	return -3; // shouldn't get here 
}


// recieves register str, returns register number  
int get_reg(char* reg)
{
	char* regs[] = REGS;

	// check which register reg is
	if (is_equal_str(reg, "$0")) return  0;
	for (int i = 0; i < REGS_LEN; i++)
		if (is_equal_str(reg, regs[i]))
			return i;

	return -1; // shouldn't get here 
}


// compares lower case version of two strings,
// if equal returns TRUE, else FALSE
int is_equal_str(char* str1, char* str2)
{
	int i = 0;
	while (str1[i] != '\0' || str2[i] != '\0')
	{
		if (tolower(str1[i]) != tolower(str2[i])) // convert to lower case and compare
			return FALSE;
		i++;
	}
	if (str1[i] != '\0' || str2[i] != '\0') // if strings are of diffrent length they are not equal 
		return FALSE;

	return TRUE;
}


// gets str of a number returns its integer value 
int str2int(char* str)
{
	if (str[0] == '0' && (str[1] == 'x' || (str[1] == 'X'))) // check if str is hex
		return strtol(str, NULL, 0);
	else // str is decimal 
		return strtol(str, NULL, 10);
}