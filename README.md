# BFS_CANAMA_PANAL
Simple "barley-break" task solved by breadth-first search algorithm (code in C)

![Alt text](PC_pzl.png?raw=true "Figure 1. Panama -> Canal barley-break")

Breadth-first search algorithm implemented in C program (***solve.c***) in order to solve the given problem:

```
ENQUEUE start position
while(goal position not found) do begin
    DEQUEUE a position P
    for each position X that is reachable from P by one move, but is not in the hash table
        INSERT X into the hash table and ENQUEUE X
    end
end
```

The code compilation is simple under the Ubuntu OS:
```
gcc -ansi -Wall  solve.c -o solve
```
Compiled code could be run on Ubuntu OS as following:
```
./solve
```

The result of program execution is presented in file "output.txt"
