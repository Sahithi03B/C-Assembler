#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdbool.h>
#include<stdlib.h>

//converting decimal to hexadecimal
char* decimalToHexadecimal(int n)
{
    // ans string to store hexadecimal number
    char* ans = ""; 
    while (n != 0) {
        // remainder variable to store remainder
        int rem = 0;
          
        // ch variable to store each character
        char ch;
        // storing remainder in rem variable.
        rem = n % 16;
        // check if temp < 10
        if (rem < 10) {
            ch = rem + 48;
        }
        else {
            ch = rem + 55;
        }         
        // updating the ans string with the character variable
        ans += ch;
        n = n / 16;
    }  
    // reversing the ans string to get the final result
    int i = 0, j = strlen(ans) - 1;
    while(i <= j)
    {
      swap(ans[i], ans[j]);
      i++;
      j--;
    }
    return ans;
}

// Structure for Symbol Table
 typedef struct SymTableNode {
// a variable to store symbols in the symbol table
	char* symbol; 
     // to store address of the corresponding symbol of the symbol table   
	int address;    
	struct SymTableNode* next;
}SymTableNode;

// Structure for several operations 
typedef struct SymTable {
	SymTableNode* head;
    //inserting a new symbol into the symbol table
	int (*ist)(struct SymTable, char*);
	//assigning the symbol in symbol table with a address
	int (*assg)(struct SymTable, char*, int);
	//to get the address of a symbol in symbol table
	int (*getaddr)(const struct SymTable, char*);
	//check if any unassigned symbol presents or not without an address
	bool (*anything_unassigned)(const struct SymTable);
	// in case of set mnemonic, updating address of a symbol to a new value
	bool (*update)(struct SymTable, char*, int);
}SymTable;

// Function to create a new node of symbol table for further progress
SymTableNode* makeSymTableNode(char* label, int address) {
    SymTableNode* childNode = (SymTableNode*) malloc(sizeof(SymTableNode));

	childNode -> symbol = (char*) malloc(1024);
	childNode -> address = address;
	childNode -> next = NULL;
	strcpy(childNode -> symbol, label);
    return childNode; //this step returns the newly created childNode of the parentNode
}

// Function for Introducing a label that occurs as an operand
// if the label does not already exist in the table.
int istSymTable(SymTable* symbol_tbl,char* label) {
    if (symbol_tbl -> head == NULL) {
		symbol_tbl -> head = makeSymTableNode(label, -1);;
		return 1;  // returns 1 if we head newly
	}
    SymTableNode* head = symbol_tbl -> head;
   if (strcmp(label, head -> symbol) == 0) {
        return 0; // returns zero is label and the first node are same
   }
       while (head -> next != NULL) {
		if (strcmp(label, (head -> next) -> symbol) == 0) {
                return 0;
		}
		head = head -> next;
	}

	head -> next = makeSymTableNode(label, -1);
	return 1;
}
//assignment function which assigns the PC address to a label if it is already present in the symbol taable
int assgSymTable(SymTable* symbol_tbl,char* label, int PC) {
    (*symbol_tbl).ist(*symbol_tbl, label);
	   SymTableNode* head = symbol_tbl -> head;
     while (strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head -> address != -1) {     //if the nodes Address is already updated that means duplicate label is found
            //so returns 1 so that we can print the error in log file
            return 1;
    }
    head -> address = PC;
	return 0;  // returns zero normally if it assigns the PC value to symbol
}
//Function to return the address of a symbol in the symbol table
// Return -1 if the address does not exist.
int getaddr(const SymTable* symbol_tbl, char* label) {
    SymTableNode* head = symbol_tbl -> head;
    while (head != NULL && strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head == NULL){     // returns -1 if there is nothing updated as the address of the symbol
        return -1;
	}
	return head -> address;
}
// This function checks for the symbols with unassigned address if any
bool anything_unassigned(const SymTable* symbol_tbl) {
    SymTableNode* head = symbol_tbl -> head;
	while (head != NULL) {
		if (head -> address == -1){ // checking for -1 in address
                return 1;            // returns 1 if there is any symbol with unassigned address
		}
		head = head -> next;
	}
	return 0;     // returns 0 if no unassigned symbol presents
}
//function for the case of set-since,set updates the values of variables or address for labels
bool update(SymTable* symbol_tbl, char* label, int address) {
    SymTableNode* head = symbol_tbl -> head;

	while (head != NULL && strcmp(head -> symbol, label) != 0) {
		head = head -> next;
	}
    if (head == NULL){
            return 0;
    }
	head -> address = address;
	return 1;
}
//make symbol tale
SymTable symbol_tbl = {NULL, istSymTable, assgSymTable, getaddr, anything_unassigned, update};

int Scan(char** line, char* nxtptr) {
    int new;
    int result = sscanf(*line, "%s%n", nxtptr, &new);
    *line += (result != -1 ? new : 0);
    return result;
}
bool isNumberValid(char* nxtptr) {
	char* endptr = NULL;
	errno = 0;
	int num = strtol(nxtptr, &endptr, 0);
	if (num == 0 && (nxtptr == endptr || errno != 0|| (errno == 0 && nxtptr && *endptr != 0))) {
		return 0;   // returns 0 if it is not a valid number
	}
	return 1; // returns 1 if this is a valid number
}
int isValidLabel(char* label) {
    if( !((label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z')) ) return 0;

    for(int i = 0; i < strlen(label); i++) {
        if( !( isdigit(label[i]) || (label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z') ) ) {
            return 0;
        }
    }
    return 1;
}

int getNumeric(char* nxtptr) {
	char* endptr = NULL;
	return strtol(nxtptr, &endptr, 0);
}
typedef struct Mnemonic {
	char* mnemonic; //This is the name of the Mnemonic
	int opcode;   //This is the opcode of the Mnemonic
	int operand_type; //This is the code to distinguish which type of Mnemonic is required
} Mnemonic;
//This array stores the struct details of all Mnemonics
Mnemonic mnemonics[] = {{"ldc", 0, 1},
                       {"adc", 1, 1},
                       {"ldl", 2, 2},
                       {"stl", 3, 2},
                        {"ldnl",4, 2},
	                    {"stnl", 5, 2},
	                    {"add", 6, 0},
	                    {"sub", 7, 0},
	                    {"shl", 8, 0},
	                    {"shr", 9, 0},
	                    {"adj", 10, 1},
	                    {"a2sp",11, 0},
	                    {"sp2a",12, 0},
	                    {"call",13, 2},
	                    {"return",14, 0},
                        {"brz", 15, 2},
                        {"brlz",16, 2},
                        {"br", 17, 2},
                        {"HALT",18, 0},
                        {"data", NULL, -1},
                        {"SET", NULL, -2}};
int isMnemonicValid(char* mnemonic){//function to check the valid mnemonic
    int i;
    int var;
    for(int i=0;i<21;i++){
        if(strcmp(mnemonics[i].mnemonic,mnemonic)==0){
            var = mnemonics[i].operand_type;
        }//comparing the given word with the mnemonics given if it is present return 1 else return 0
    }
    if(var == -2 || var == -1 || var == 0 || var == 1 || var == 2){
        return var;
    }else{
    return -5;
}}

     bool reqOffset(char* mnemonic) {
	char* offsetMnemonics[] = {"ldl", "stl", "ldnl", "stnl", "call", "brz", "brlz", "br"};//these mnemonics requires offset
	for (int i = 0; i < 8; i++) {
		if (strcmp(offsetMnemonics[i], mnemonic) == 0) { // here it returns 1 if the mnemonic matches the mnemonics which requires offset
                return 1;
		}
	}
	return 0; // else return 0
}
char* decToBinary24bit(int n) {
    char* binaryNum = (char*)malloc(25 * sizeof(char));
    int i = 23;

    while (n > 0) {
        binaryNum[i] = (n % 2) + '0'; // convert digit to character
        n = n / 2;
        i--;
    }

    while (i >= 0) {
        binaryNum[i] = '0'; // add leading zeros
        i--;
    }

    binaryNum[24] = '\0'; // add null terminator

    return binaryNum;
}
char* decToBinary32bit(int n) {
    char* binaryNum = (char*)malloc(33 * sizeof(char));
    int i = 31;

    while (n > 0) {
        binaryNum[i] = (n % 2) + '0'; // convert digit to character
        n = n / 2;
        i--;
    }

    while (i >= 0) {
        binaryNum[i] = '0'; // add leading zeros
        i--;
    }

    binaryNum[32] = '\0'; // add null terminator

    return binaryNum;
}

char* decToBinary(int n) {
    char* binaryNum = (char*)malloc(9 * sizeof(char));
    int i = 7;

    while (n > 0) {
        binaryNum[i] = (n % 2) + '0'; // convert digit to character
        n = n / 2;
        i--;
    }

    while (i >= 0) {
        binaryNum[i] = '0'; // add leading zeros
        i--;
    }
    binaryNum[8] = '\0'; // add null terminator

    return binaryNum;
}
int get_oprcode(char** opr_value, char* nxtptr, char* mnemonic,int mnemonic_type, int pc) {
	int num = 0;

	if (mnemonic_type == -1) { // for mnemonic data in which mnemonic_type=-1;
		*opr_value = decToBinary32bit(getNumeric(nxtptr)); // for data we a 32 bit code because of the absence of the 8 bit opcode
		return 1;
	}
    //if the word is a valid number then convert it into a number and store it in num
	if (isNumberValid(nxtptr)){
        num = getNumeric(nxtptr);}
	else if (isValidLabel(nxtptr)) {//if the word is a valid label,then get the address assigned to this label from symbol table
            num = symbol_tbl.getaddr(&symbol_tbl, nxtptr);}
	else {
         return 0;
	}
    //if it is the offset required mnemonic case then subtract the present PC address from the address assigned to the label in symbol table
	if (reqOffset(mnemonic) && isValidLabel(nxtptr)) {
            num -= pc+ 1;
	}
	*opr_value = decToBinary24bit(num);//convert it into 24 bit opr code
	return 1;
}


// Function to Return the 8-bit opcode of the given mnemonic
int get_opcode(char** opcode, char* mnemonic){
    int r;
	for (int i = 0; i < 20; i++) {
		if (strcmp(mnemonics[i].mnemonic, mnemonic) == 0) { // checks the mnemonic array for the required mnemonic
			r = mnemonics[i].opcode; // assigns the required opcode here
           *opcode = decToBinary(r);
           return 1;
		}
	}
	return 0; // returns 0 if the function cant find the opcode for this mnemonic
}
char* Hexa_decimal_from_binary(const char* bin, int mode) { // function is used at converting 8 bit opcode to 
//2 byte to write in the listing file
    int n = 0;
    const char* a = bin;
    while (*a) {
        n = (n << 1) | (*a++ - '0');
    }
    char* hex = (char*) malloc(9 + mode);
    if (hex != NULL) {
        if (mode) {
            sprintf(hex, "%08x", n);
        } else {
            sprintf(hex, "%x", n);
        }
    }
    return hex;
}
//left padding in case of use 
char *StringPadRight(char *string, int padded_len, char *pad) {
    int len = (int) strlen(string);
    if (len >= padded_len) {
        return string;
    }
    int i;
    for (i = 0; i < padded_len - len; i++) {
        strcat(string, pad);
    }
    return string;
}

/*// 2nd pass
int secondPass(FILE *input, FILE *object, FILE *listfile, FILE *log) {
    //declaring pc counter, error count, ln(line count)
	int pc = 0; // pc addr
    int error = 0;           // to store error count in a variable
    int ln = 0;     // to store current line number in input file
    char* linep2 = (char*) malloc(1024);
    //It is used to move or change the position of the file pointer which is being used to read the file,
    //to a specified offset position
	fseek(input, 0, SEEK_SET);
	//running a while loop to read all lines int the input file
	while (fgets(linep2, 1024, input)) {
		ln++;// To go to next line in the input file
    // Erasing comments if found any;
    char* comment_if_any = strchr(linep2, ';'); //presence of ';' indicates the presence of a comment
	if (comment_if_any != NULL)
	{*comment_if_any = '\0';} // erasing the comment
	//Dynamic memory allocation of some variables
    
	char* label = (char*) malloc(1024); // creation of a variable to read a label into it
    char* mnemonic = (char*) malloc(10);// creation of a variable to read a mnemonic into it
    char* nxtptr = (char*) malloc(1024);
    //creation of a variable to read the operand into it if any operand is present after mnemonics
    char* opcode = (char*) malloc(10);  // creation of a variable to store the respective opcode of a mnemonic
    char* opr_value = (char*) malloc(26); //creation of a variable to read the operands code into it
    char* mchn_code = (char*) malloc(36); //creation of a variable to read the final machine code output created for every line
    //initiation of the array machine code with null value
    mchn_code[0] = '\0';

    //see if any label is present by finding colon
    char* colon_if_any = strchr(linep2, ':');
	if (colon_if_any != NULL) { // presence of ':' indicates presence of label
		*colon_if_any = ' ';    // replace label by ' '
    //Since it is pass2 then the labels present in the symbol table should be
    //valid because it has passed pass1
    (Scan(&linep2, label));// to change the address to next word
     // print pc addr, label in the listing file
     fprintf(listfile, "%08x\t\t\t\t%s:\n", pc, label);
}
    if (Scan(&linep2, mnemonic) == -1){ // if there is nothing, continue
        continue;}
     //To store mnemonic type in a variable
    int mnemonic_type = isMnemonicValid(mnemonic);
    get_opcode(&opcode, mnemonic);// to store the opcode of the encountered mnemonic

    if (mnemonic_type == -2){//if mnemonic is set type then there is no need to write in the machine code(object file)
        continue;}//we just continue if it is set
       //if the type of mnemonic is greater than 1 that means that the mnemonic requires an operand
		if (mnemonic_type==1||mnemonic_type==2) { //if the operand is required after mnemonic
		//pointing to the next word
		    Scan(&linep2,nxtptr);
		    get_oprcode(&opr_value, nxtptr, mnemonic, mnemonic_type, pc);//to set opr code to the required operands
		}else{
		    get_oprcode(&opr_value,"0", mnemonic, mnemonic_type, pc);}//setting the opr code to zero instead of word if mnemonic doesnt require an operand
		strcat(mchn_code, opr_value);//concatenating 24 bit opr value to machine code in the object file
		if (mnemonic_type != -1) {
                strcat(mchn_code, opcode);} // catenation of opcode with machine code if mnemonic is not data to make 32 bit machine code
		fprintf(object, "%s\n", mchn_code);//printing the 32 bit machine code in object file
		// printing in listing file
		fprintf(listfile, "%08x\t%s\t%s\t0x%s\n", pc, Hexa_decimal_from_binary(mchn_code, 1), mnemonic, Hexa_decimal_from_binary(opr_value, 0));
		pc++; // increasing pc counter to go to next linep2
	}
	fprintf(log, "output files generated\n");
	return 1;
}*/
void write_listingfile(FILE *fp, char *str,int pc,char mnemonic,char* opr_value){
    char* s = Hexa_decimal_from_binary(str, 1);
    char* s1 = Hexa_decimal_from_binary(opr_value, 0);

fprintf(fp, "%08x\t%s\t%s\t0x%s\n", pc,s, mnemonic, s1);

}
void write_object(FILE *fp, char *str)
{
    fprintf(fp, "%s\n", str);
}

// 2nd pass
int secondPass(FILE *input, FILE *object, FILE *listfile, FILE *log) {
    //declaring pc counter, error count, ln(line count)
	int pc = 0; // pc addr
    int error = 0;           // to store error count in a variable
    int ln = 0;     // to store current line number in input file
    char* linep2 = (char*) malloc(1024);
    //It is used to move or change the position of the file pointer which is being used to read the file,
    //to a specified offset position
	fseek(input, 0, SEEK_SET);
	//running a while loop to read all lines int the input file
	while (fgets(linep2, 1024, input)) {
		ln++;// To go to next line in the input file
    // Erasing comments if found any;
    char* comment_if_any = strchr(linep2, ';'); //presence of ';' indicates the presence of a comment
	if (comment_if_any != NULL)
	{*comment_if_any = '\0';} // erasing the comment
	//Dynamic memory allocation of some variables

	char* label = (char*) malloc(1024); // creation of a variable to read a label into it
    char* mnemonic = (char*) malloc(10);// creation of a variable to read a mnemonic into it
    char* nxtptr = (char*) malloc(1024);
    //creation of a variable to read the operand into it if any operand is present after mnemonics
    char* opcode = (char*) malloc(10);  // creation of a variable to store the respective opcode of a mnemonic
    char* opr_value = (char*) malloc(26); //creation of a variable to read the operands code into it
    char* mchn_code = (char*) malloc(36); //creation of a variable to read the final machine code output created for every line
    //initiation of the array machine code with null value
    mchn_code[0] = '\0';

    //see if any label is present by finding colon
    char* colon_if_any = strchr(linep2, ':');
	if (colon_if_any != NULL) { // presence of ':' indicates presence of label
		*colon_if_any = ' ';    // replace label by ' '
    //Since it is pass2 then the labels present in the symbol table should be
    //valid because it has passed pass1
    (Scan(&linep2, label));// to change the address to next word
     // print pc addr, label in the listing file
     fprintf(listfile, "%08x\t\t\t\t%s:\n", pc, label);
}
    if (Scan(&linep2, mnemonic) == -1){ // if there is nothing, continue
        continue;}
     //To store mnemonic type in a variable
    int mnemonic_type = isMnemonicValid(mnemonic);
    get_opcode(&opcode, mnemonic);// to store the opcode of the encountered mnemonic

    if (mnemonic_type == -2){//if mnemonic is set type then there is no need to write in the machine code(object file)
        continue;}//we just continue if it is set
       //if the type of mnemonic is greater than 1 that means that the mnemonic requires an operand
		if (mnemonic_type==1||mnemonic_type==2) { //if the operand is required after mnemonic
		//pointing to the next word
		    Scan(&linep2,nxtptr);
		    get_oprcode(&opr_value, nxtptr, mnemonic, mnemonic_type, pc);//to set opr code to the required operands
		}else{
		    get_oprcode(&opr_value,"0", mnemonic, mnemonic_type, pc);}//setting the opr code to zero instead of word if mnemonic doesnt require an operand
		strcat(mchn_code, opr_value);//concatenating 24 bit opr value to machine code in the object file
		if (mnemonic_type != -1) {
                strcat(mchn_code, opcode);} // catenation of opcode with machine code if mnemonic is not data to make 32 bit machine code
		write_object(object, mchn_code);//printing the 32 bit machine code in object file
		// printing in listing file
		write_listingfile(*listfile,mchn_code,pc,mnemonic,opr_value);
		pc++; // increasing pc counter to go to next linep2
	}
	fprintf(log, "output files generated\n");
	return 1;
}

int main(int argc,char* argv[]){ // argc is the number of arguments and argv is the arguments in string format
FILE *input, *object, *listfile, *log;
//dynamic memory allocation to store file names
char* file_name = (char*) malloc(1024);
char* object_file_name = (char*) malloc(1024);
char* listfile_file_name = (char*) malloc(1024);
char* log_file_name = (char*) malloc(1024);

strcpy(file_name, argv[1]);//extraction of name from argv
char* something = strchr(file_name, '.');
if (something != NULL) *something = '\0';
input = fopen(argv[1], "r");//opening assembly file in read mode
//assigning name to object file
object_file_name[0] = '\0';
strcat(object_file_name, file_name);
strcat(object_file_name, ".o");
object = fopen(object_file_name, "wb");//opening object file in write binary mode

//assigning name to listing file
listfile_file_name[0] = '\0';
strcat(listfile_file_name, file_name);
strcat(listfile_file_name, ".lst");
listfile = fopen(listfile_file_name, "w");//opening list file in write mode

//assigning name to log file
log_file_name[0] = '\0';
strcat(log_file_name, file_name);
strcat(log_file_name, ".log");
log = fopen(log_file_name, "w");//opening log file in write mode


	//pass1
int pc=0; //current PC address
int error=0;//error count
int ln=0;//current line number
char* line = (char*) malloc(1024);
//loop for reading all the lines in input file
while(fgets(line,1024,input)){
    ln++;//line by line
    //Erasing comments if there any
    char* comment_if_any=strchr(line,';');
    if(comment_if_any!=NULL){*comment_if_any='\0';}//erasing comment
//Dynamic allocation to store labels and mnemonics
char* label = (char*) malloc(1024);
char* nxtptr = (char*) malloc(1024);
//Checking if there is any label
//check if there is any colon(:)
char* colon_if_any=strchr(line,':');
if(colon_if_any!=NULL){*colon_if_any=' ';//replacing colon by ' '
  if(Scan(&line, label)==-1){
      //if it is empty print error
      fprintf(log,"line '%d'-> It's not a valid label",ln);
      error++;
      continue;
  }
  //checking if it is a valid label or not
			if (isValidLabel(label)) {
                //if it is continue,
                //assign pc addr to it in the Symbol Table
                // if it is already present in the table, print error
				if (symbol_tbl.assg(&symbol_tbl, label, pc)==1) { // returns 1 if it is already assigned
                // To print the required error in the log file
				fprintf(log, "line '%d' -> It's a duplicate label\n", ln);
                    error++;
					continue;
				}
			}else{fprintf(log,"line '%d' -> It's not a valid label\n",ln);
			error++;continue;}
			//if no next word present, then continue
			if (Scan(&line, nxtptr) == -1) {continue;}

			int mnemonic_type = isMnemonicValid(nxtptr);

			if(mnemonic_type==-5){
			   fprintf(log,"line '%d' -> It's not a valid mnemonic",ln);
			   error++;continue;}
			if(mnemonic_type==-1||mnemonic_type==-2||mnemonic_type==1||mnemonic_type==2){
			    if(Scan(&line,nxtptr)==-1){
			        fprintf(log,"line '%d' -> Operand missing\n",ln);
			        error++;
			        continue;}
			     if (!isValidLabel(nxtptr) && !isNumberValid(nxtptr)) {
			    fprintf(log, "line '%d' ->  operand is not a number or label\n", ln);
			    error++;continue;}
			if (isValidLabel(nxtptr)){//check if its valid label
				symbol_tbl.ist(&symbol_tbl, nxtptr);}//if yes, insert this into the symbol table
			//check if a number is valid or not
			if(isNumberValid==1){
			    //if yes, then continue
			    if(mnemonic_type==-2){
			        symbol_tbl.update(&symbol_tbl, label, getNumeric(nxtptr));
					continue;}
			}else if(Scan(&line,nxtptr)!=-1){
			    fprintf(log,"line '%d' -> extra operand appeared\n",ln);
			    error++;continue;
			}
			if(Scan(&line,nxtptr)!=-1){
			    fprintf(log,"line '%d' -> extra words at the end of the line\n",ln);
			    error++;continue;
			}
			  pc++;//incrementing the pc counter by 1
			}//end of assembly(input) file reading

			//checking for any errors found in 1st pass
            //if found , then stop proceeding to 2nd pass
			if(error){
			    return 0;
			    }else{
			        secondPass(input,object,listfile,log);
			    }
			    fclose(log); fclose(listfile);
	           fclose(object);  fclose(input);
			}
}
}