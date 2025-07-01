# NSHELL
NSHELL is a basic project written in C language written by me, in order to learn the use of system calls provided by UNIX operating system.

## Features
- offers dual modes, interactive mode (./nsh) with prompt, and batch mode (./nsh <file>) for reading commands from a file.
- implements built-in commands cd, help, exit internally without forking.
- offers the option for simple I/O redirection ('<' for input redirection, '>' and '>>' for output redirection).
- allows chaining commands with ';' and can execute any executable in PATH environment variable or via an absolute/relative path.

## Features not implemented in this shell
- No piping.
- No quoting and backslash escaping.
- No globbing.
- No other methods of chaining and running in background like &.

## System calls used
- fork() to create a child process for execution of commands.
- waitpid() to let the parent wait for child process till it is terminated or killed.
- execvp() to execute executable given in arguments.
- open() and close() to open and close file descriptors to handle input and output.

## References
- [link](https://brennan.io/2015/01/16/write-a-shell-in-c/) - for basic structure of code of shell.
- [link](https://pages.cs.wisc.edu/~remzi/OSTEP/) - for understanding process API and key concepts.
- [link](https://linux.die.net/man/) - for Linux man pages of various system calls.
