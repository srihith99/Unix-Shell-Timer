Spawns a new UNIX shell process with support for all standard UNIX commands.
The execution time for each command will also be displayed to the shell user ( The mmap API in C language was used for communication of time between processes ). 

Compiling the program : gcc shell.c -o time -lrt
Running the program : ./time <write_a_linux_command>
-lrt is used as we are accessing the files in bin.
