#include <iostream>
#include <fstream>
#include <sstream>
#include "instruction.hpp"
#include <stdlib.h>

using namespace std;
void parseUserInputs(string & filename);
void parse(string filename);


void execCommand();

void findLabel(string label);

void execR1(int type);
void execR2(int type);
void execI1(int type);
void execI2(int type);
void execB(int type);
void execCB(int type);
void execD1(int type);
void execD2(int type);
void execBL(int type);
void execBR(int type);

int mode = 0;

vector<Instruction> PGM;
long long int RFILE[32];
unsigned char *MEM = new unsigned char[1000];
unsigned char *STACK= new unsigned char[1000];

int N = 0;

int currentSFlagInstr = -1;
int ConditionReg1 = 0;
int ConditionReg2 = 0;

//int BLcounter = 0;

int Zero = 0, Overflow = 0, Negative = 0, Carry = 0;

int main(int argc, char * argv[]){

	for(int i = 0; i < 1000; i++){
		MEM[i] = 0;
		STACK[i] = 0;
	}

	//cout << "Enter the Legv8 file name: ";
	//string filename = "legv8.txt";
	string filename;// = "sample.txt";
	//cin >> filename;

	parseUserInputs(filename);
	parse(filename);
	findLabel("MAIN");
	execCommand();

	return 0;
}
void parseUserInputs(string & filename){
	//cout << "Do you wish to initialize the memory? (y/n): ";
	//cin >> init;

	ifstream input("info.txt");
	getline(input, filename);

	string init;
	getline(input, init);
	//cout << init << endl;	
	if(init == "y"){
		string initInputs;
		getline(input, initInputs);
		cout << initInputs << endl;
		stringstream ss(initInputs);
		int memIdx = 0;
		int memVal = 0;
		while(ss >> memIdx){
			ss >> memVal;
			MEM[memIdx] = memVal;
		}
	}
	/*
	cout << "Memory output:" << endl;
	for(int i = 0; i < 10; i++){
		cout << "MEM[" << i << "]" << MEM[i] - '0' << endl;
	}
	*/
	/*
	   cout << "Select execution modes: (1) Single step\t(2) Run to completion\n";
	   cin >> mode;
	   while(mode != 1 && mode != 2){
	   cout << "Error in input. Please try again.\nSelect execution modes: (1) Single step\t(2) Run to completion\n";
	   cin >> mode;
	   }
	 */


}

void parse(string filename){
	ifstream inputFile(filename);
	string line;

	Instruction filler;
	PGM.push_back(filler);

	int number = 1;
	while( getline(inputFile,line) ){
		//cout << line << endl;
		Instruction in;
		in.parse(line, number);
		PGM.push_back(in);
		//cout << endl;
		number++;
	}
	//cout << "Total lines: " << PGM.size() << endl;
}

void execCommand(){
	ofstream outs ("output.txt", ofstream::out);
	while( N > 0 && N < PGM.size() ){
		int type = PGM[N].type;
		/*
		   if(mode == 1){
		   cout << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl; 
		   }
		 */
		outs << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl; 
		if( type < 7 ){
			//R format
			execR1(type);
		}
		else if( type < 10 ){
			//R format
			execR2(type);
		}
		else if(type < 15){
			//I format
			execI1(type);
		}
		else if(type < 18){
			//I flags format
			execI2(type);
		}
		else if(type < 19){
			//B format
			execB(type);
		}
		else if(type < 31){
			//CB format
			execCB(type);
		}
		else if(type < 33){
			//D format
			execD1(type);
		}
		else if(type < 39){
			//D variant format
			execD2(type);
		}
		else if(type < 40){
			//BL statement
			execBL(type);
		}
		else if(type < 41){
			//BR statement
			execBR(type);
		}
		else{
			//cout << "Error executing line: " << PGM[N].fullLine << endl;
			return;
		}
		RFILE[31] = 0;
		/*
		   if( mode == 1 ){
		   int printMode = 0;
		   int regToPrint = -1;
		   cout << "Select print options: (1) Specific register\t(2) All registers\n";
		   cin >> printMode;
		   while( printMode != 1 && printMode != 2 ){
		   cout << "Error in input. Please try again.\nSelect print options: (1) Specific register\t(2) All registers\n";
		   cin >> printMode;
		   }
		   switch(printMode){
		   case 1:
		   cout << "Please enter the desired register number between [0,31].\n";
		   cin >> regToPrint;
		   while( regToPrint < 0 || regToPrint > 31 ){
		   cout << "Error in input. Please enter the desired register number between [0,31].\n";
		   cin >> regToPrint;
		   }
		   cout << "REG[" << regToPrint << "] = " << RFILE[regToPrint] << endl;
		   break;
		   case 2:
		   cout << "Printing all registers...\n";
		   for(int i = 0; i < 32; i++){
		   cout << "REG[" << i << "] = " << RFILE[i] << endl;
		   }
		   break;
		   default:
		   cout << "Error in print input\n";
		   break;
		   }
		   cout << endl;
		   cout << endl;
		   }
		 */
		for(int i = 0; i < 32; i++){
			outs << RFILE[i] << " ";
		}
		outs << endl;
	}
	/*
	   if( mode == 2 ){
	   cout << "Finished executing program. Printing all registers...\n";
	   for(int i = 0; i < 32; i++){
	   cout << "REG[" << i << "] = " << RFILE[i] << endl;
	   }
	   }
	 */


}

void findLabel(string label){
	bool found = false;
	N = 0;
	while(N < PGM.size() && !found){
		if( PGM[N].label == label )
			found = true;
		else
			N++;
	}
	/*
	   if(N < PGM.size()){
	   cout << label << " is at: " << N << endl;
	   }
	   else{
	   cout << "Label not found.\n";
	   N = PGM.size();
	   }
	 */
	if(N < 1 || N > PGM.size()){
		//cout << "Label not found.\n";
		N = PGM.size();
	}
}

void execR1(int type){
	int d = 0,j = 0,k = 0;
	d = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];

	switch(type){
		case 0://add
			RFILE[d] = RFILE[j] + RFILE[k];
			break;
		case 1://and
			RFILE[d] = RFILE[j] & RFILE[k];
			break;
		case 2://orr
			RFILE[d] = RFILE[j] | RFILE[k];
			break;
		case 3://eor
			RFILE[d] = RFILE[j] ^ RFILE[k];
			break;
		case 4://sub
			RFILE[d] = RFILE[j] - RFILE[k];
			break;
		case 5://lsr
			RFILE[d] = RFILE[j] >> k;
			break;
		case 6://lsl
			RFILE[d] = RFILE[j] << k;
			break;
		default:
			cout << "Error\n";
	}
	N++;
}

void execR2(int type){
	int d = 0,j = 0,k = 0;
	d = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];

	currentSFlagInstr = N;
	ConditionReg1 = RFILE[j];
	ConditionReg2 = RFILE[k];

	unsigned long long int t1;
	unsigned long long int t2;
	unsigned long long int t3;
	switch(type){
		case 7://adds
			RFILE[d] = RFILE[j] + RFILE[k];
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			if( (RFILE[j] > 0 && RFILE[k] > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && RFILE[k] < 0 && RFILE[d] > 0) )
				Overflow = 1;
			else
				Overflow = 0;
			t1 = RFILE[d]; 
			t2 = RFILE[j]; 
			t3 = RFILE[k]; 
			if( t1 < t2 || t1 < t3 )
				Carry = 1;
			else
				Carry = 0;
			break;
		case 8://ands
			RFILE[d] = RFILE[j] & RFILE[k];
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			Overflow = 0;//???
			Carry = 0;
			break;
		case 9://subs 
			RFILE[d] = RFILE[j] - RFILE[k];
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			if( (RFILE[j] > 0 && RFILE[k] > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && RFILE[k] < 0 && RFILE[d] > 0) )
				Overflow = 1;
			else
				Overflow = 0;
			t1 = RFILE[d]; 
			t2 = RFILE[j]; 
			t3 = RFILE[k]; 
			if( t1 < t2 || t1 < t3 )
				Carry = 1;
			else
				Carry = 0;
			break;
		default:
			cout << "Error\n";
	}
	N++;
}

void execI1(int type){
	int d = 0, j = 0, k = 0;
	d = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];

	switch(type){
		case 10://orri
			RFILE[d] = RFILE[j] | k;
			break;
		case 11://eori
			RFILE[d] = RFILE[j] ^ k;
			break;
		case 12://addi
			RFILE[d] = RFILE[j] + k;
			break;
		case 13://andi
			RFILE[d] = RFILE[j] & k;
			break;
		case 14://subi
			RFILE[d] = RFILE[j] - k;
			break;
		default:
			cout << "Error\n";
	}
	N++;
}

void execI2(int type){
	int d = 0, j = 0, k = 0;
	d = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];

	currentSFlagInstr = N;
	ConditionReg1 = RFILE[j];
	ConditionReg2 = k;

	unsigned long long int t1;
	unsigned long long int t2;
	unsigned long long int t3;
	switch(type){
		case 15://addis
			RFILE[d] = RFILE[j] + k;
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			if( (RFILE[j] > 0 && k > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && k < 0 && RFILE[d] > 0) )
				Overflow = 1;
			else
				Overflow = 0;
			t1 = RFILE[d]; 
			t2 = RFILE[j]; 
			t3 = k; 
			if( t1 < t2 || t1 < t3 )
				Carry = 1;
			else
				Carry = 0;
			break;
		case 16://subis
			RFILE[d] = RFILE[j] - k;
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			if( (RFILE[j] > 0 && k > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && k < 0 && RFILE[d] > 0) )
				Overflow = 1;
			else
				Overflow = 0;
			t1 = RFILE[d]; 
			t2 = RFILE[j]; 
			t3 = k; 
			if( t1 < t2 || t1 < t3 )
				Carry = 1;
			else
				Carry = 0;
			break;
			break;
		case 17://andis
			RFILE[d] = RFILE[j] & k;
			Negative = (RFILE[d] < 0 ? 1 : 0);
			Zero = (RFILE[d] == 0 ? 1 : 0);
			Overflow = 0;
			Carry = 0;
			break;
		default:
			cout << "Error\n";
	}
	N++;
}

void execB(int type){
	findLabel(PGM[N].paramLabel);
}

void execCB(int type){
	int d = 0;
	unsigned long long int UnsignedVal1 = ConditionReg1;
	unsigned long long int UnsignedVal2 = ConditionReg2;
	switch(type){
		case 19://EQ
			if(Zero == 1)
				findLabel(PGM[N].paramLabel);
			else
				N++;
			break;
		case 20://NE
			if(Zero == 0)
				findLabel(PGM[N].paramLabel);
			else
				N++;
			break;
		case 21://LT
			if( ConditionReg1 < ConditionReg2){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 22://LE
			if( ConditionReg1 <= ConditionReg2){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 23://GT
			if( ConditionReg1 > ConditionReg2){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 24://GE
			if( ConditionReg1 >= ConditionReg2){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 25://LO
			if( UnsignedVal1 < UnsignedVal2 ){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 26://LS
			if( UnsignedVal1 <= UnsignedVal2 ){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 27://HI
			if( UnsignedVal1 > UnsignedVal2 ){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 28://HS
			if( UnsignedVal1 >= UnsignedVal2 ){
				findLabel(PGM[N].paramLabel);
			}
			else
				N++;
			break;
		case 29://CBZ
			d = PGM[N].params[0];
			if(RFILE[d] == 0)
				findLabel(PGM[N].paramLabel);
			else
				N++;
			break;
		case 30://CBNZ
			d = PGM[N].params[0];
			if(RFILE[d] != 0)
				findLabel(PGM[N].paramLabel);
			else
				N++;
			break;
		default:
			cout << "Error\n";
	}
}

void execD1(int type){
	int i = 0, j = 0, k = 0;
	i = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];

	switch(type){
		case 31://stur
			if( j == 28 ){//stack
				for(int t = 0; t <= 7; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					STACK[ RFILE[j] + t + k ] = b;
				}

			}
			else{
				for(int t = 0; t <= 7; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					MEM[ RFILE[j] + t + k ] = b;
				}
			}
			break;
		case 32://ldur
			if( j == 28 ){//stack
				RFILE[i] = 0;
				for(int t = 0; t <= 7; t++){
					RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
				}
			}
			else{
				RFILE[i] = 0;
				for(int t = 0; t <= 7; t++){
					RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
				}
			}
			break;
		default:
			cout << "Error\n";
	}
	N++;
}

void execD2(int type){
	int i = 0, j = 0, k = 0;
	i = PGM[N].params[0];
	j = PGM[N].params[1];
	k = PGM[N].params[2];
	switch(type){
		case 33://sturb
			if( j == 28 ){//stack
				for(int t = 0; t <= 0; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					STACK[ RFILE[j] + t + k ] = b;
				}

			}
			else{
				for(int t = 0; t <= 0; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					MEM[ RFILE[j] + t + k ] = b;
				}
			}
			break;
		case 34://ldurb
			if( j == 28 ){//stack
				RFILE[i] = 0;
				for(int t = 0; t <= 0; t++){
					RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
				}
			}
			else{
				RFILE[i] = 0;
				for(int t = 0; t <= 0; t++){
					RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
				}
			}
			break;
		case 35://sturh
			if( j == 28 ){//stack
				for(int t = 0; t <= 1; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					STACK[ RFILE[j] + t + k ] = b;
				}

			}
			else{
				for(int t = 0; t <= 1; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					MEM[ RFILE[j] + t + k ] = b;
				}
			}
			break;
		case 36://ldurh
			if( j == 28 ){//stack
				RFILE[i] = 0;
				for(int t = 0; t <= 1; t++){
					RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
				}
			}
			else{
				RFILE[i] = 0;
				for(int t = 0; t <= 1; t++){
					RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
				}
			}
			break;
		case 37://sturw
			if( j == 28 ){//stack
				for(int t = 0; t <= 3; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					STACK[ RFILE[j] + t + k ] = b;
				}

			}
			else{
				for(int t = 0; t <= 3; t++){
					int b = (RFILE[i] >> (8*t)) & 0xff;
					MEM[ RFILE[j] + t + k ] = b;
				}
			}
			break;
		case 38://ldursw
			if( j == 28 ){//stack
				RFILE[i] = 0;
				for(int t = 0; t <= 3; t++){
					RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
				}
			}
			else{
				RFILE[i] = 0;
				for(int t = 0; t <= 3; t++){
					RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
				}
			}
			break;
		default:
			cout << "Error\n";
	}

}

void execBL(int type){
	//BLcounter++;
	RFILE[30] = N+1;
	findLabel(PGM[N].paramLabel);
}

void execBR(int type){
	/* if(BLcounter <= 0){
	   N = PGM.size();
	   }
	   else{
	   cout << BLcounter-- << endl << endl;
	 */    
	N = RFILE[30];
	//}
}
