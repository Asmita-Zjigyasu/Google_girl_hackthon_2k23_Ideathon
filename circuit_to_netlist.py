import sys
import os

h = open('netlist2.txt', 'r')
f = open("final_netlist.txt", 'w')
f1 = open("fault.txt", 'w')  
# Dictionary for mapping operators
Operator = {'&':'AND', '|':'OR', '^':'XOR', '~':'NOT'}
# Assigning node numbers to the primary inputs
Net = {'A':1, 'B':2, 'C':3, 'D':4} 
# Reading from the file
content = h.readlines()
 
# Variable for storing the node count
node_cnt = 5 # start adding the nodes with node_cnt = 5 as earlier 4 numbers altready assigned
 
# Iterating through the content Of the file
for line in content:
    str = line
    if(str != "*****" or str[:6] != "FAULT_"):
        cond = line.split() 
        for member in cond:
            if(member[:4] != '^'):
                if(member[:3] == "net" or member[:3] == "Z"): #assigning node number to the intermediate nodes
                    if member in Net.keys(): # do not want to add the node to the dictionary again
                        continue
                    else:
                        Net[member] = node_cnt # assigning value to Z would also be taken care over here
                        node_cnt = node_cnt+1 # increment the node for the next wire
                

for line in content:
    str = line
    if(str[:6] != "FAULT_"):
        cond1 = line.split()
        print(cond1)
        stat1 = Net.get(cond1[0]).__str__()
        stat2 = Net.get(cond1[1]).__str__() # this is '=' in each line
        stat3 = Net.get(cond1[2]).__str__()
        stat4 = Net.get(cond1[3]).__str__()
        
        
        if(len(cond1) == 4):
            if(cond1[2] == '~'):
                f.write("INV " + stat4 + ' ' + stat1 + "\n") # NOT in out
        elif(len(cond1) == 5):
            stat5 = Net.get(cond1[4]).__str__()
            if(cond1[3] == '&'):
                f.write("AND " + stat3 + ' ' + stat5 + ' ' + stat1 + ' ' + "\n") # AND in1 in2 out
            elif(cond1[3] == '|'):
                f.write("OR " + stat3 + ' '  + stat5 + ' '  + stat1 + ' '  + "\n") # OR in1 in2 out
            elif(cond1[3] == '^'):
                f.write("OR " + stat3 + ' '  + stat5 + ' '  + node_cnt.__str__() + ' '  + "\n") # converting XOR to a series of gates
                node_cnt = node_cnt+1        
                f.write("NAND " + stat3 + ' '  + stat5 + ' '  + node_cnt.__str__() + ' '  + "\n")
                f.write("AND " + (node_cnt-1).__str__() + ' '  + node_cnt.__str__() + ' '  + stat1 + ' '  + "\n") 
                
                
out_net = Net.get('Z').__str__()  
f.write("INPUT  " + "1 2 3 4 -1" + "\n") 
f.write("OUTPUT  " + out_net + " -1" + "\n")
               
f.close()   

print(Net)

for line in content:
    str = line
    if(str[:8] == "FAULT_AT"):
        cond2 = line.split()
        print(cond2)
        key_net = cond2[2]
        if key_net in Net.keys():
            out_fault_loc = Net.get(key_net).__str__() 
            f1.write("fault_location " + out_fault_loc + "\n") 
    elif(str[:10] == "FAULT_TYPE"): 
        cond3 = line.split()
        if (cond3[2] == "SA0"):
            f1.write("fault_type " + "0" + "\n")
        elif (cond3[2] == "SA1"):
            f1.write("fault_type " + "1")
            
f1.close() 