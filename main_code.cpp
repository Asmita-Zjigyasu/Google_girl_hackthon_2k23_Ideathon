#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
using namespace std;

struct NodenValue
{
	int nodeNum;
	int nodeVal;
};

struct Gates
{
	string	gateType;
	int		gateNumber;
	int		gateInput1, gateInput2;
	int		gateOutput;
	int		gateFlag;
};

struct Nodes
{
	int nodeVal;
	int nodeFlag;
	int faultFlag;
	vector <Gates>	listofGates;
};

// Global variables declaration
NodenValue Fault;
NodenValue PIObjective;
NodenValue PIBacktrace;
NodenValue PISet;
Gates G;
Nodes N;

int faultActFlag = 0;
int status = 0;

vector <Gates> gate;
vector <Nodes> node;
vector <int> PINodes;
vector <int> PONodes;

class PODEM_functions
{
public:
	PODEM_functions();
	~PODEM_functions();

	// Reads the file
	int readFile()
	{
		int faultNode = 0;
		int faultValue = 0;
		int i = 0;
		int no_spaces1 = 0;
		int lineCount = 0;

		ostringstream s1;
		ostringstream s2;

		string outputFileName = "";
		string inputFileName = "";
		string line = "";

		cout << "Enter the filename here: ";			// Name of the input file
		cin  >> inputFileName;

		inputFileName += ".txt";

		ifstream inputFile(inputFileName);

		// Displays the contents of the input file
		if(inputFile.is_open())
		{
			while(getline(inputFile, line))
				cout << endl << line;
			inputFile.close();
		}

		else
		{
			cout << endl << "Unable to open file" << endl;
			return 0;
		}

		inputFile.open(inputFileName);

		if (inputFile.is_open())
		{
			while (getline(inputFile, line))
			{	
				int no_spaces = std::count(line.begin(), line.end(), ' ');				// Number of spaces in each line

				string *subString = new string[no_spaces + 1];							// Sub string in each line
				istringstream iss(line);

				i = 0;

			    // making each line into sub strings
				while (iss && i <= no_spaces)
				{
					string sub;
					iss >> sub;
					subString[i] = sub;
					i++;
				}

				G.gateType = subString[0];
				G.gateNumber = lineCount;

				if (G.gateType != "INPUT" && G.gateType != "OUTPUT")
				{
					G.gateInput1 = atoi(subString[1].c_str());	// Input 1 of the gate

				// Total number of input nodes till gate input 1
					while (node.size() <= G.gateInput1)
						node.push_back(N);

					node.at(G.gateInput1).listofGates.push_back(G); // List of gates at each node using gate input 1

					// Reading all lines except INV and BUF
					if (G.gateType != "INV" && G.gateType != "BUF")
					{
						G.gateInput2 = atoi(subString[2].c_str());	// Input 2 of the gate
						G.gateOutput = atoi(subString[3].c_str());	// Output of the double-input gate

						// Total number of input nodes till gate input 2
						while (node.size() <= G.gateInput2)
							node.push_back(N);

						node.at(G.gateInput2).listofGates.push_back(G); // List of gates at each node using gate input 2
					}

					else
						G.gateOutput = atoi(subString[2].c_str()); // Output of the single-input gate

					gate.push_back(G);
				}

				else
				{
					// Reading the INPUTS line
					if (G.gateType == "INPUT")
						for (i = 1 ; i < (no_spaces - 1) ; i++)		                    // Starts from 1 because the 0th subString is the word INPUTS
							PINodes.push_back(atoi(subString[i].c_str()));			    // List of PI nodes

					// Reading the OUTPUTS line
					if (G.gateType == "OUTPUT")
					{
						for (i = 1; i < (no_spaces - 1) ; i++)
						{
							PONodes.push_back(atoi(subString[i].c_str()));				// List of PO nodes
							while (node.size() <= atoi(subString[i].c_str()))
								node.push_back(N);
						}
						break;
					}
				}

				no_spaces = 0;
				lineCount++;
			}

			inputFile.close();
		}
		return 1;
	}

	// Sets all nodes to x
	void setX()
	{
		for (int i = 1; i < node.size(); i++)
		{
			node.at(i).nodeVal = -1;
			node.at(i).nodeFlag  = 0;					// denotes that the value at the corresponding node is x
		}
	}

	// Says whether the PODEM was successful or not!
	int PODEM()
	{
		// Checking if the error is at the Primary Outputs
		ofstream MyFile("output.txt");
		for (int i = 0; i < PONodes.size(); i++)
			if (node.at(PONodes.at(i)).faultFlag == 1)
			{
				MyFile << "SUCCESS";
				MyFile << endl<< "PI nodes: " << endl;

				for (int i = 0; i < PINodes.size(); i++) // display PI number
				{
					if(PINodes.at(i) == 1)
						MyFile << "[A, ";
					else if(PINodes.at(i) == 2)
						MyFile << "B, ";
					else if(PINodes.at(i) == 3)
						MyFile << "C, ";
					else if(PINodes.at(i) == 4)
						MyFile << "D] = ";
				}

                MyFile << "[";

				for (int i = 0; i < PINodes.size(); i++)
				{
					if (PINodes.at(i)<4)
					{
						if (node.at(PINodes.at(i)).nodeVal == -1)
							MyFile << "x, ";
						else
							MyFile << node.at(PINodes.at(i)).nodeVal << ", ";
					}
					else
					{
						if (node.at(PINodes.at(i)).nodeVal == -1)
							MyFile << "x]";
						else
							MyFile << node.at(PINodes.at(i)).nodeVal << "]";
					}
				}
				MyFile.close();
				return 1;
			}

		PIBacktrace = Objective();

		// Checking if the D-frontier fails
		if (PIBacktrace.nodeNum == 0 || PIBacktrace.nodeNum == -1)
			return 0;

		PISet = Backtrace(PIBacktrace);

		// Checking if the D-frontier fails
		if (PISet.nodeNum == 0 || PISet.nodeNum == -1)
			return 0;

		Imply(PISet);
		
		if (PODEM() == 1)
		{
			return 1;
		}

		// Reversing the decision
		PISet.nodeVal = !PISet.nodeVal;
		Imply(PISet);
		
		if (PODEM() == 1)
		{
			MyFile << "SUCCESS";
			MyFile << endl<< "PI nodes: " << endl;

			for (int i = 0; i < PINodes.size(); i++) // display PI number
			{
				if(PINodes.at(i) == 1)
					MyFile << "[A, ";
				else if(PINodes.at(i) == 2)
					MyFile << "B, ";
				else if(PINodes.at(i) == 3)
					MyFile << "C, ";
				else if(PINodes.at(i) == 4)
					MyFile << "D] = ";
			}

			MyFile << "[";			

			for (int i = 0; i < PINodes.size(); i++)
			{
				if (PINodes.at(i)<4)
				{
					if (node.at(PINodes.at(i)).nodeVal == -1)
						MyFile << "x, ";
					else
						MyFile << node.at(PINodes.at(i)).nodeVal << ", ";
				}
				else
				{
					if (node.at(PINodes.at(i)).nodeVal == -1)
						MyFile << "x]";
					else
						MyFile << node.at(PINodes.at(i)).nodeVal << "]";
				}
			}
			MyFile.close();
			return 1;
		}

		PISet.nodeVal = -1;
		Imply(PISet);
		status = 1;

		return 0;
	}

	// Returns the Objective to be achieved
	NodenValue Objective()
	{
		int l, v;
		l = Fault.nodeNum;
		v = Fault.nodeVal;

		PIObjective.nodeNum = -1;
		PIObjective.nodeVal = -1;

		// Checking if the fault is activated
		for (int i = 1; i < node.size(); i++)
		{
			if (node.at(i).faultFlag == 1)
			{
				faultActFlag = 1;
			}
		}

		// Checking if the fault node is the Input1 of the gates and if it is x
		if (!faultActFlag)
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateInput1 == l)
					if (node.at(gate.at(i).gateInput1).nodeVal == -1)
					{
						PIObjective.nodeNum = gate.at(i).gateInput1;
						PIObjective.nodeVal = !(v);
						return PIObjective;
					}
			}

		// Checking if the fault node is the Input2 of the gates and if it is x
		if (!faultActFlag)
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateType != "INV" && gate.at(i).gateType != "BUF" && gate.at(i).gateInput2 == l)
					if (node.at(gate.at(i).gateInput2).nodeVal == -1)
					{
						PIObjective.nodeNum = gate.at(i).gateInput2;
						PIObjective.nodeVal = !(v);
						return PIObjective;
					}
			}

		// Checking if the fault node is the PO node and if it is x
		if (!faultActFlag)
			for (int i = 0; i < PONodes.size(); i++)
			{
				if (PONodes.at(i) == l)
					if (node.at(PONodes.at(i)).nodeVal == -1)
					{
						PIObjective.nodeNum = PONodes.at(i);
						PIObjective.nodeVal = !(v);
						return PIObjective;
					}
			}

		// Checking for a gate from the D-frontier
		for (int i = 0; i < gate.size(); i++)
		{
			if (gate.at(i).gateType == "INV" || gate.at(i).gateType == "BUF")
			{
				if (isDFrontier(gate.at(i).gateInput1, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput2).nodeFlag == 0)
					{
						PIObjective.nodeNum = gate.at(i).gateInput2;
						PIObjective.nodeVal = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}
			}
			else if (gate.at(i).gateType == "AND" || gate.at(i).gateType == "OR" || gate.at(i).gateType == "NAND" || gate.at(i).gateType == "NOR")
			{
				if (isDFrontier(gate.at(i).gateInput1, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput2).nodeFlag == 0)
					{
						PIObjective.nodeNum = gate.at(i).gateInput2;
						PIObjective.nodeVal = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}

				if (isDFrontier(gate.at(i).gateInput2, gate.at(i).gateOutput))
					if (node.at(gate.at(i).gateInput1).nodeFlag == 0)
					{
						PIObjective.nodeNum = gate.at(i).gateInput1;
						PIObjective.nodeVal = !(ctrlValue(gate.at(i).gateType));
						return PIObjective;
					}
			}
		}
	
		return PIObjective;
	}

	// Maps Objective into PI assignment
	NodenValue Backtrace(NodenValue PIBacktrace)
	{
		int k = PIBacktrace.nodeNum;
		int vk = PIBacktrace.nodeVal;
		int v = vk;
		int j = 0;
		int inv = 0;
		int flag = 1;

		NodenValue PISetTemp;
		PISetTemp.nodeNum = -1;
		PISetTemp.nodeVal = -1;

		while (flag)
		{
			flag = 0;
			for (int i = 0; i < gate.size(); i++)
			{
				if (gate.at(i).gateOutput == k)
				{
					inv = invValue(gate.at(i).gateType);
					
					if (node.at(gate.at(i).gateInput1).nodeVal == -1)
						j = gate.at(i).gateInput1;
					else if (gate.at(i).gateType != "INV" && gate.at(i).gateType != "BUF" && node.at(gate.at(i).gateInput2).nodeVal == -1)
						j = gate.at(i).gateInput2;
					
					v = XORValue(v, inv);
					k = j;
					flag = 1;
					break;
				}
			}
		}

		PISetTemp.nodeNum = k;
		PISetTemp.nodeVal = v;

		return PISetTemp;
	}

	// Propagates the value
	void Imply(NodenValue PIPropagate)
	{
		int nodeNum = PIPropagate.nodeNum;
		int nodeVal  = PIPropagate.nodeVal;

		int gin1, gin2, gout;
		string gtype;
		int flag = 1;

		// Set the PI node
		for (int i = 0; i < PINodes.size(); i++)
		{
			int PI = PINodes.at(i);

			if (PI == nodeNum)
				if (node.at(PI).nodeFlag == 0)
				{
					node.at(PI).nodeVal = nodeVal;
					node.at(PI).nodeFlag = 1;
					if (PI == Fault.nodeNum)
						node.at(PI).faultFlag = 1;
					break;
				}
		}

		// Evaluate the implications
		while (flag)
		{
			flag = 0;
			for (int i = 0; i < gate.size(); i++)
			{
				gin1  = gate.at(i).gateInput1;
				gin2  = gate.at(i).gateInput2;
				gout  = gate.at(i).gateOutput;
				gtype = gate.at(i).gateType;

				if (node.at(gout).nodeFlag == 0)
				{
					if (node.at(gin1).nodeFlag == 1 && (gtype == "INV" || gtype == "BUF"))
					{
						flag = gate1(gin1, gout, gtype);
					}					
					else if (node.at(gin1).nodeFlag == 1 && node.at(gin1).nodeVal == ctrlValue(gtype) && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
					else if (node.at(gin2).nodeFlag == 1 && node.at(gin2).nodeVal == ctrlValue(gtype) && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
					else if (node.at(gin1).nodeFlag == 1 && node.at(gin2).nodeFlag == 1 && (gtype == "AND" || gtype == "NAND" || gtype == "OR" || gtype == "NOR"))
					{
						flag = gate2(gin1, gin2, gout, gtype);
					}
				}
			}
		}
	}

	// Evaluate single input gate
	int gate1(int input, int output, string type)
	{
		if (type == "INV")
		{
			node.at(output).nodeVal = !(node.at(input).nodeVal);
			node.at(output).nodeFlag = 1;
			node.at(output).faultFlag = node.at(input).faultFlag;
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;
			return 1;
		}
		else if (type == "BUF")
		{
			node.at(output).nodeVal = node.at(input).nodeVal;
			node.at(output).nodeFlag = 1;
			node.at(output).faultFlag = node.at(input).faultFlag;
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;
			return 1;
		}
		else
			return 0;
	}

	// Evaluate double input gate
	int gate2(int input1, int input2, int output, string type)
	{
		if (type == "AND")
		{
			if (node.at(input1).nodeVal == 0 || node.at(input2).nodeVal == 0)
			{
				node.at(output).nodeVal = 0;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeVal = node.at(input1).nodeVal & node.at(input2).nodeVal;
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "OR")
		{
			if (node.at(input1).nodeVal == 1 || node.at(input2).nodeVal == 1)
			{
				node.at(output).nodeVal = 1;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeVal = node.at(input1).nodeVal | node.at(input2).nodeVal;
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "NAND")
		{
			if (node.at(input1).nodeVal == 0 || node.at(input2).nodeVal == 0)
			{
				node.at(output).nodeVal = 1;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeVal = !(node.at(input1).nodeVal & node.at(input2).nodeVal);
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else if (type == "NOR")
		{
			if (node.at(input1).nodeVal == 1 || node.at(input2).nodeVal == 1)
			{
				node.at(output).nodeVal = 0;
				node.at(output).nodeFlag = 1;
			}
			else
			{
				node.at(output).nodeVal = !(node.at(input1).nodeVal | node.at(input2).nodeVal);
				node.at(output).nodeFlag = 1;
			}

			faultPropagate(input1, input2, output, type);
			if (output == Fault.nodeNum && (node.at(output).nodeVal != Fault.nodeVal))
				node.at(output).faultFlag = 1;

			return 1;
		}
		else
			return 0;
	}

	// Propagates the fault
	void faultPropagate(int input1, int input2, int output, string type)
	{
		if (node.at(input1).faultFlag == 1)
			if (node.at(input2).nodeVal == !ctrlValue(type))
				node.at(output).faultFlag = 1;

		if (node.at(input2).faultFlag == 1)
			if (node.at(input1).nodeVal != ctrlValue(type))
				node.at(output).faultFlag = 1;
	}

	// Checks if the gate is a D-frontier
	int isDFrontier(int inputNode, int outputNode)
	{
		if (node.at(inputNode).faultFlag == 1 && node.at(outputNode).nodeFlag == 0)
			return 1;
		return 0;
	}

	// Returns the controlling value of the particular gate
	int ctrlValue(string type)
	{
		if ( type == "AND")
			return 0;
		else if ( type == "NAND")
			return 0;
		else if ( type == "OR")
			return 1;
		else if ( type == "NOR")
			return 1;
		else
			return -1;
	}

	// Returns the inversion value of the particular gate
	int invValue(string type)
	{
		if ( type == "AND")
			return 0;
		else if ( type == "NAND")
			return 1;
		else if ( type == "OR")
			return 0;
		else if ( type == "NOR")
			return 1;
		else if ( type == "INV")
			return 1;
		else if ( type == "BUF")
			return 0;
		else
			return -1;
	}

	// Returns the XOR value
	int XORValue(int a, int b)
	{
		return ((!a & b) + (a & !b));
	}

private:
	
}P_func;

PODEM_functions::PODEM_functions()
{
}

PODEM_functions::~PODEM_functions()
{
}

int main()
{
	int flag = 1;
	int result = 0;
	flag = P_func.readFile();
	if(flag == 0)
		return 0;
    
    string inputfault = "";
    string line = "";

    cout << endl << endl << "Enter the fault filename here: ";			// Name of the input fault file
    cin  >> inputfault;

    inputfault += ".txt";

    ifstream inputFile(inputfault);

		// Displays the contents of the input file
    if(inputFile.is_open())
    {
        while(getline(inputFile, line))
            cout << endl << line;
        inputFile.close();
    }

    else
    {
        cout << endl << "Unable to open fault file" << endl;
        return 0;
    }

    inputFile.open(inputfault);

    
    if (inputFile.is_open())
    {
        while (getline(inputFile, line))
        {	
            int no_spaces = std::count(line.begin(), line.end(), ' ');				// Number of spaces in each line

            string *subString = new string[no_spaces + 1];							// Sub string in each line
            istringstream iss(line);

            int j = 0;

        // Tokenizing each line into sub strings
            while (iss && j <= no_spaces)
            {
                string sub;
                iss >> sub;
                subString[j] = sub;
                j++;
            }

            if(subString[0] == "fault_location")
                Fault.nodeNum = stoi(subString[1]);
            else if(subString[0] == "fault_location")
                Fault.nodeVal = stoi(subString[1]);


            no_spaces = 0;
			
		}

		inputFile.close();

		P_func.setX();
		result = P_func.PODEM();

		if (status == 1)
			cout << endl << endl << "FAILURE";

		status = 0;

		for (int i = 1; i < node.size(); i++)
		{
			node.at(i).nodeFlag = 0;
			node.at(i).faultFlag = 0;
		}

		for (int i = 0; i < gate.size(); i++)
		{
			gate.at(i).gateFlag = 0;
		}

		faultActFlag = 0;
	}

	cout << endl << endl;


	return 0;
}