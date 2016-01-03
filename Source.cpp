/*  TITLE
	DEDUCTIVE FAULT SIMULATOR
*/

/*  PERSONAL DETAILS
	Krishna Prasad Suresh
	MS in ECE at Georgia Institute of Technology
	Specialization: VLSI Systems and Logic Design
	skprasad.22@gmail.com
	+1 (470) 263-0867
*/

/*  DESCRIPTION
	This Circuit Simulator simulates the circuit for any input vector and displays the outputs at each node.
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

using namespace std;

struct Gates
{
    string  gateType;
    int		gateNumber;
	int		gateInput1, gateInput2;
	int		gateOutput;
	int		gateFlag;
}G;

struct Nodes
{
    int			   nodeValue;
    int			   nodeFlag;
    vector <Gates> listofGates;
}N;

// Declaration and initialization of all global variables
ofstream outputFile;

int no_spaces1 = 0;

string inputValues = "";

vector <Gates> Gate;
vector <Nodes> Node;
vector <Gates> Temp;
vector <int>   PINodes;				// List of primary input nodes
vector <int>   PONodes;				// List of primary output nodes
vector <int>   GONodes;				// List of gate output nodes

class Simulator
{
public:
	Simulator();
	~Simulator();

	// Reads the circuit file
	int readCircuitFile()
	{
		int no_spaces	= 0;												// Number of spaces in each line
		int lineCount	= 0;												// Number of the line being read
		int i			= 0;												// Control variable

		string fileName	   = "";											// Name of the file containing the interconnection
		string line		   = "";											// Each line in the file

		// Reading the file containing the interconnection
		cout << endl << "Enter the filename here (without extension): ";
		cin  >> fileName;

		fileName += ".txt";

		ifstream inputFile(fileName);										// Opens the file containing the interconnection

		// Displaying all the contents of the file
		if(inputFile.is_open())
		{
			while(getline(inputFile, line))
				cout << endl << line;
			inputFile.close();
		}
		else
			return 0;

		inputFile.open(fileName);

		// Tokenizing and storing all the contents of the file
		if (inputFile.is_open())
		{
			while (getline(inputFile,line))
			{
				no_spaces = std::count(line.begin(), line.end(), ' ');

				string *subString = new string[no_spaces + 1];							// Sub string in each line
				istringstream iss(line);

				i = 0;

				// Tokenizing each line into sub strings
				while (iss && i <= no_spaces)
				{
					string sub;
					iss >> sub;
					subString[i] = sub;
					i++;
				}

				G.gateType		= subString[0];							// Name of the gate
				G.gateNumber	= lineCount;							// Number of the gate

				// Reading all lines except inputs and outputs
				if (G.gateType != "INPUT" && G.gateType != "OUTPUT")
				{
					G.gateInput1 = atoi(subString[1].c_str());							// Input 1 of the gate
					
					// Total number of input nodes till gate input 1
					while (Node.size() <= G.gateInput1)
						Node.push_back(N);

					Node.at(G.gateInput1).listofGates.push_back(G);						// List of gates at each node using gate input 1

					// Reading all lines except INV and BUF
					if (G.gateType != "INV" && G.gateType != "BUF")
					{
						G.gateInput2 = atoi(subString[2].c_str());						// Input 2 of the gate
						G.gateOutput = atoi(subString[3].c_str());						// Output of the double-input gate

						GONodes.push_back(G.gateOutput);

						// Total number of input nodes till gate input 2
						while (Node.size() <= G.gateInput2)
							Node.push_back(N);

						Node.at(G.gateInput2).listofGates.push_back(G);					// List of gates at each node using gate input 2
					}
					else
					{
						G.gateOutput = atoi(subString[2].c_str());						// Output of the single-input gate

						GONodes.push_back(G.gateOutput);
					}

					Gate.push_back(G);
				}
				else
				{
					// Reading the INPUTS line
					if (G.gateType == "INPUT")
					{
						no_spaces1 = no_spaces;
						cout << endl << endl;

						cout << "Input Vector (" << (no_spaces - 2) << " inputs): ";
						cin  >> inputValues;

						if (inputValues.length() != (no_spaces - 2))
							return 2;

						// Assigning values to Input nodes
						for (int i = 1; i < (no_spaces - 1); i++)
						{
							Node.at(atoi(subString[i].c_str())).nodeValue = (int) (inputValues[i-1] - 48);
							Node.at(atoi(subString[i].c_str())).nodeFlag = 1;			                        // Flag indicating that the node has been assigned a value

							PINodes.push_back(atoi(subString[i].c_str()));										// List of PI nodes
						}
					}

					// Reading the OUTPUTS line
					if (G.gateType == "OUTPUT")
					{
						for (int i = 1; i < (no_spaces - 1) ; i++)
							PONodes.push_back(atoi(subString[i].c_str()));			// List of PO nodes

						for (int i = 0; i < PONodes.size(); i++)
							while (Node.size() <= PONodes.at(i))
								Node.push_back(N);
						break;
					}
				}

				no_spaces = 0;
				lineCount++;
			}

			inputFile.close();							// Close the file containing the interconnection
		}

		return 1;
	}

	// Creates the output file 
	int createOutputFile()
	{
		char ans = 'y';

		outputFile.open("Outputs.txt");                         // Creating the output file

		while (ans == 'y')
		{
			outputFile << endl << "The input vector is : " << inputValues << endl;

			output();

			cout << endl << endl;
			cout << "Do you want to give more input vectors (y/n): ";
			cin  >> ans;

			if (ans == 'y')
			{
				for (int i = 1;i < Node.size();i++)
					Node.at(i).nodeFlag = 0;

				for (int i = 0;i < Gate.size();i++)
					Gate.at(i).gateFlag = 0;
		
				cout << endl << "Input Vector (" << (no_spaces1 - 2) << " inputs): ";
				cin >> inputValues;

				if (inputValues.length() != (no_spaces1 - 2))
					return 0;

				for (int i = 0 ; i < (no_spaces1 - 2) ; i++)
				{
					Node.at(PINodes.at(i)).nodeValue = (int)(inputValues[i] - 48);
					Node.at(PINodes.at(i)).nodeFlag = 1;
				}
			}
		}
	}

	// Calculates outputs
	void output()
	{
		int i = 0, j = 0;

		while (checkOutputValue(PONodes) != 1)
			for (i = 0 ; i < Gate.size() ; i++)
				if (Gate.at(i).gateFlag != 1)
					checkOutput(i);

		for (i = 0 ; i < PONodes.size() ; i++)
			cout << endl << "The value at the output node " << PONodes.at(i) << " is " << Node.at(PONodes.at(i)).nodeValue;
	}

	// Propagates the values at every node
	void checkNode(int a)
	{
		for (int i = 0 ; i < Node.at(a).listofGates.size() ; i++)
		{
			Temp = Node.at(a).listofGates;
			if (Gate.at(Temp.at(i).gateNumber).gateFlag != 1)
				checkOutput(Temp.at(i).gateNumber);
		}
	}

	// Evaluates every gate
	void checkOutput(int a)
	{
		if (Gate.at(a).gateType == "INV" || Gate.at(a).gateType == "BUF")
		{
			if (Node.at(Gate.at(a).gateInput1).nodeFlag == 1 && Gate.at(a).gateFlag != 1)
			{
				Gate.at(a).gateFlag = 1;

				if (Gate.at(a).gateType == "INV")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				else
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				checkNode(Gate.at(a).gateOutput);
			}
		}

		else
		{
			if (Node.at(Gate.at(a).gateInput1).nodeFlag == 1 && Node.at(Gate.at(a).gateInput2).nodeFlag == 1 && Gate.at(a).gateFlag != 1)
			{
				Gate.at(a).gateFlag = 1;

				if (Gate.at(a).gateType == "AND")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue & Node.at(Gate.at(a).gateInput2).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				else if (Gate.at(a).gateType == "NAND")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue & Node.at(Gate.at(a).gateInput2).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				else if (Gate.at(a).gateType == "OR")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = Node.at(Gate.at(a).gateInput1).nodeValue | Node.at(Gate.at(a).gateInput2).nodeValue;
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				else if (Gate.at(a).gateType == "NOR")
				{
					Node.at(Gate.at(a).gateOutput).nodeValue = !(Node.at(Gate.at(a).gateInput1).nodeValue | Node.at(Gate.at(a).gateInput2).nodeValue);
					Node.at(Gate.at(a).gateOutput).nodeFlag = 1;
				}

				checkNode(Gate.at(a).gateOutput);
			}
		}
	}

	// Checks if the POs have values assigned
	int checkOutputValue(vector <int> outputNode)
	{
		int a = 0;

		for (int i = 0; i < outputNode.size(); i++)
		{
			a = outputNode.at(i);
			if (Node.at(a).nodeFlag != 1)
				return 0;
		}
		return 1;
	}

private:

}S_func;

Simulator::Simulator()
{
}

Simulator::~Simulator()
{
}

int main()
{
	int flag1 = 0, flag2 = 1;

	flag1 = S_func.readCircuitFile();

	if (flag1 == 0)
	{
		cout << endl << "File not found!";
		cout << endl << endl;
		return 0;
	}

	else if (flag1 == 2)
	{
		cout << endl << "Incorrect no. of inputs";
		cout << endl << endl;
		return 0;
	}

	flag2 = S_func.createOutputFile();
	
	if (flag2 == 0)
	{
		cout << endl << "Incorrect no. of inputs";
		cout << endl << endl;
		return 0;
	}
	cout << endl << endl;
	return 0;
}