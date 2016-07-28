# Unix-Shell
GNU shell functionalities implementation

Three Modules(Three Files):
        1. DataBase.h 
        2. Parser.c    
        3. Shell.c
        
1. DataBase.h
Stores the whole data/commands moving around the shell and accessible on including it in file.
Has two structures storing the result of parsing.

2. Parser.c
The whole parsing logic to interpret the incoming command on my shell and is heart of whole program.
Parse commands and stores all given and implied information in the structures.

3. Shell.c
The actual shell implementation works on the incoming command and try to replicate behavior of Bash.

Functionalities Implemented:
1. All non-builtin commands can be executed.
2. Following Builtin commands can be implemented:
        "cd","pwd","export","echo","history","exit"
3. Handling of environment variables to store and print them.
4. Multilevel Piping without I/O redirection
5. History and '!' operator
6. Handling Interrupt Signal: On pressing "Ctrl+C" 

P.S.
My parsing logic is taking care of files for i/o redirection but due to lack of time i am not actually able to redirect contents to corresponding file(s).

Compilation : 
        gcc Parser.c Shell.c
        OR
        gcc Parser.c -c
        gcc Shell.c -c
        gcc Parser.o Shell.o

Execution : ./a.out 

