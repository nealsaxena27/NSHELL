# NSHELL
NSHELL is a basic project written in C language written by me, in order to learn the use of system calls provided by UNIX-based operating systems.

## How to run
- Build the code with `make` command.
- Run the shell with `./nsh`.

## Features
- offers dual modes, interactive mode (./nsh) with prompt, and batch mode (./nsh <file>) for reading commands from a file.
- implements built-in commands cd, help, exit internally without forking.
- offers the option for simple I/O redirection ('<' for input redirection, '>' and '>>' for output redirection).
- offers the option to pipe various commands. ('|' can be used to pipe any number of commands).
- allows chaining commands with ';' and can execute any executable in PATH environment variable or via an absolute/relative path.

## Features not implemented in this shell
- No quoting and backslash escaping.
- No globbing.
- No other methods of chaining and running in background like &.

## System calls used
- fork() to create a child process for execution of commands.
- waitpid() to let the parent wait for child process till it is terminated or killed.
- execvp() to execute executable given in arguments.
- open() and close() to open and close file descriptors to handle input and output.
- read() and write() to read and write into files and for input/output.
- chdir() to implement built-in command cd for changing current directory.
- pipe() to create pipes for inter-process communication and implement piping of commands.

## References
- [Stephen Brennan's Tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/) - as a start point for basic structure of code of shell.
- [OSTEP Book](https://pages.cs.wisc.edu/~remzi/OSTEP/) - for understanding process API and key concepts.
- [Linux Man Pages (Web)](https://linux.die.net/man/) - for Linux man pages of various system calls.
