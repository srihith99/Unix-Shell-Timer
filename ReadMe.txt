The program launches a new UNIX shell process with support for all basic UNIX commands .
The program also measures running times of each inputted command.

Compiling the program : gcc shell.c -o time -lrt
Running the program : ./time <write_a_linux_command>
-lrt is used as we are accessing the files in bin.
