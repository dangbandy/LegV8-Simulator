#include <map>
#include "instruction.hpp"

int main(){
    /*
	string test = "main: ADD X10, X9, X2";
	Instruction in;
	cout << test << endl;
	in.parse(test,1);
	cout << endl;

	string test2 = "main2: CBZ X10, LABEL";
	Instruction in2; 
	cout << test2 << endl;
	in2.parse(test2,2);
	cout << endl;
	
	string test3 = "main3: B LABEL";
	Instruction in3;
	cout << test3 << endl;
	in3.parse(test3,3);
	cout << endl;
	
	string test4 = "main4: B.EQ LABEL";
	Instruction in4;
	cout << test4 << endl;
	in3.parse(test4,4);
	*/

    long long int x = 123456789;
    unsigned char MEM[8];
    for(int i = 0; i < 8; i++){
        MEM[i] = (x >> (8*i)) & 0xff;
    }

    for(int i = 0; i < 8; i++){
        cout << '|' << MEM[i] - 0 << '|' << '\t';
    }
    cout << endl;

    long long int y = 0;
    for(int i = 0; i < 8; i++){
        y = y | (MEM[i] << (8*i));
    }
    cout << y;


    return 0;
}
