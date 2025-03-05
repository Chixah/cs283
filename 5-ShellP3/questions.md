1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_answer here_I had to use waitpid() in order to ensure that all child processes complete before the shell continues accepting user input. If i forgot to call waitpid() on all child processes, it could potentially cause zombie processes, potential descritor leaks, and further signal processing.

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_answer here_ I believe the main reason why it is necessary to close unused pipe ends after calling dup2() was to ensure efficient system resource management. Potential things that could go wrong if I were to leave pipes open is data leakage from unintended file descriptors.

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_answer here_ I think cd is implemented as a built-in rather than an external command because cd have to change the current working directory of the shell process, if it were to be implemented as an external process, then things like environment is going to be a problem with child and parent.

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_answer here_ I say I attempt to fix the unknown bugs not covered by my tests first, then I would free memory of the already executed commands and reuse the following space in order to allow an arbitrary number of piped commands while still handling memory allocation efficiently.
