# Google_girl_hackthon_2k23_Ideathon
This repository contains the implementation of the an Automatic Test Generation Algorithm for 4 Primary inputs, 1 Primary output and 4 gates (AND, OR, NOT, XOR). The Circuit netlist, the Fault_location and the fault_type are given (stuck-at-0 or stuck-at-1 faults). This algorithm will generate an input test vector for thr Primary Inputs to identify the location of the fault.

## Instructions for Execution of the Code
1. Create a input_netlist.txt file having the circuit netlist, fault_location and fault_type in the same manner as given in [input_netlist](https://github.com/Asmita-Zjigyasu/Google_girl_hackthon_2k23_Ideathon/blob/main/input_netlist.txt).
2. Run the [circuit_to_netlist.py](https://github.com/Asmita-Zjigyasu/Google_girl_hackthon_2k23_Ideathon/blob/main/circuit_to_netlist.py) file using python3. This will generate 2 files, a [final_netlist.txt](https://github.com/Asmita-Zjigyasu/Google_girl_hackthon_2k23_Ideathon/blob/main/final_netlist.txt) and [fault.txt](https://github.com/Asmita-Zjigyasu/Google_girl_hackthon_2k23_Ideathon/blob/main/fault.txt).
3. Run the podem.cpp file. Enter the name of the netlist file and the fault file in the respective questions without .txt.
4. The output.txt generated will have the required Input Test vector for the Primary Inputs to detect the Given fault.
5. 'X' in the Input vector means that the PI value can be 1 or 0, it doesn't affect the fault detection.
