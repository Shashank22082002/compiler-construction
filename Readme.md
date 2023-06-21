## ERPLAG COMPILER ##


<br> </br>


### **Requirements to compile code**
<ul>
<li> GCC VERSION >= 9
<li> NASM COMPILER
<li> LINUX TERMINAL(preferably or download some plugins to run make command on windows)
</ul>

### To Run the compiler, extract the zip and run 
`make`

### This will generate a build of the compiler

### Next run the command 
`./compiler <test_file.txt> <code.asm>`

### This will generate the nasm file
### To run nasm compiler and the executable use the command

`nasm -felf64 code.asm && gcc -no-pie code.o -o code && ./code
` 



### **TODOS**
<UL>
<LI> handling of switch case in code generation .. rest ALL contructs shall work
<LI> handling of dynamic arrays in code gen.
</ul>

### **Contributed by**
<ul>
<li>2020A7PS0073P       Shashank Agrawal
<li>2020A7PS0096P       Akshat Gupta
<li>2020A7PS0129P       Anish Ghule
<li>2020A7PS0134P       Shadan Hussain
<li>2020A7PS1513P       Tarak P V S
</ul> 

Note this is the final repository, some of the pre versions can be seen at -- [AnishGhule-Technogeek/SASTA-Compiler](https://github.com/AnishGhule-Technogeek/SASTA-Compiler)
### This was done as part of course Compiler construction.
