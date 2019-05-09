#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <ctype.h>
using namespace std;

map<string,int> commandTypes = {
	//R format
	{"ADD",0},{"AND",1},{"ORR",2},{"EOR",3},{"SUB",4},{"LSR",5},{"LSL",6},
	//R flags format
	{"ADDS",7},{"ANDS",8},{"SUBS",9},
	//I format
	{"ORRI",10},{"EORI",11},{"ADDI",12},{"ANDI",13},{"SUBI",14},
	//I flags format
	{"ADDIS",15},{"SUBIS",16},{"ANDIS",17},
	//B format
	{"B",18},
	//CB format
	{"B.EQ",19},{"B.NE",20},{"B.LT",21},{"B.LE",22},{"B.GT",23},{"B.GE",24},
	{"B.LO",25},{"B.LS",26},{"B.HI",27},{"B.HS",28},
	{"CBZ",29},{"CBNZ",30},
	//D format
	{"STUR",31},{"LDUR",32},
	//D variant format
	{"STURB",33},{"LDURB",34},{"STURH",35},{"LDURH",36},{"STURW",37},{"LDURSW",38},
	//BL statement
	{"BL",39},
	//BR statement
	{"BR",40}
};

struct Instruction{
	string label;//main
    string fullLine;
	string paramLabel;
	string cmd;//ADD
	vector<int> params;
	int lineNumber;
	int type;

	Instruction(){
		label = paramLabel = cmd = fullLine = "";
		lineNumber = type = 0;
	}

	void parse(string instr, int ln){
		lineNumber = ln;
        fullLine = instr;
		stringstream ss(instr);
		size_t scope = instr.find(":");
		if (scope != string::npos){
			ss >> label;
			label = label.substr(0, label.size()-1);
			cout << "Label is " << label << endl;
		}
		else
			cout << "No label found\n";

		ss >> cmd;
		cout << "CMD is: " << cmd << endl;
        if( commandTypes.find(cmd) != commandTypes.end() ){
            type = commandTypes[cmd];
            cout << "Type: " << type << endl;
        }
        else{
            type = -1;
            cout << "Unkown command type" << endl;
        }

        if( (type >= 18 && type <= 30) || type == 39){
            cout << "Banch type\n";
            size_t commaIdx = instr.find(",");
            //2 params
            if (commaIdx != string::npos){
                char p;
                int num = -1;
                while( ss >> p && p != ','){
                    if(isdigit(p) ){
                        if(ss >> num){
                            stringstream numConvert;
                            numConvert << p << num;
                            numConvert >> num;
                            params.push_back( num );
                        }
                        else{
                            params.push_back( p - '0');
                            ss.clear();
                        }
                    }


                }

            }
            ss >> paramLabel;
        }
        else{
            cout << "Not branch type\n";
            char p;
            int num = -1;
            while( ss >> p ){
                if(isdigit(p) ){
                    if(ss >> num){
                        stringstream numConvert;
                        numConvert << p << num;
                        numConvert >> num;
                        params.push_back( num );
                    }
                    else{
                        params.push_back( p - '0');
                        ss.clear();
                    }
                }
            }
        }
        for( int i = 0; i < params.size(); i++)
            cout << "param is num: " << params[i] << endl;

        if(paramLabel.size() != 0){
            cout << "Param Label: " << paramLabel << endl;
        }
        else{
            cout << "There is no param label\n";
        }


    }




};


