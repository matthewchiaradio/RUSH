# RUSH
This program is a simple and efficient Unix shell. Each command is written on a new line. This program must be run in linux (Ubunto was used to develop this program).

# Built in Commands
- "exit" - When the user types exit, the program stops running. It is an error to pass any arguments to exit.

- "cd" - cd always take one argument (0 or >1 args signaled as an error) and is used to change directories. chdir() is used in the code to successfully change directories.

- "path" - The path command takes 0 or more arguments, with each argument separated by whitespace from the others. A typical usage would be like this: rush> path /bin /usr/bin, which would add /bin and /usr/bin to the search path of the shell. The path command always overwrites the old path with the newly specified path.

# Redirection
- ">" - This is used at the end of a command to write the output to a file rather than the standard output. For example, if a user types "ls -la /tmp > output", nothing should be printed on the standard output. Instead, the standard output of the ls program should be rerouted to the file output. If the output file exists already, then the file will be overwritten. The exact format of redirection is a command (and possibly some arguments) followed by the redirection symbol followed by a filename. Multiple redirection operators or multiple arguments to the right of the redirection sign are errors.

# Parallel Commands
This shell allows for multiple commands to be run at the same time. This is accomplished with the ampersand operator. For example, "cmd1 & cmd2 args1 args2 & cmd3 args1". wait() is used in the code to ensure all commands complete at the same time.
