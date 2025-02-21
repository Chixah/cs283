1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  _start here_ I think we use `fork/execvp` instead of just calling `execvp` directly because fork allows use to create child processes that runs independently from the parent process, also, when the child process fails, it does not affect the parent process. Furthermore, something to do with error handling? It got mentioned a lot throughout the hw and lecture.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  _start here_ When the fork() system call fails, it returns a -1, where my rc reports there is an error and return rc.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  _start here_ execvp() find the command to execute through the path after it was given the argv from the cmd. The system environment variable that plays a role in this process is the PATH.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  _start here_ The purpose of calling wait() in the parent process after forking is to ensure nothing went wrong in the child process, if we didn't call it, i assume we might encounter multiple errors at the same time or something terminated itself.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  _start here_ It provides the exit status of the child process and it is important for error handling by the parent process.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  _start here_ My implementation of build_cmd_buff() follows the criteria mentioned in README.md:
    On each line-of-input parsing, you should populate cmd_buff using these rules:

trim ALL leading and trailing spaces
eliminate duplicate spaces UNLESS they are in a quoted string
account for quoted strings in input; treat a quoted string with spaces as a single argument
for example, given echo    " hello,    world"  you would parse this as: ["echo", " hello,    world"]; note that spaces inside the double quotes were preserved,
this is important because it enables the separation of the arguments effectively and remove potential errors before hand, like trailing and leading spaces.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  _start here_ Instead of manually using an array I swapped over to using malloc, which causes a lot of segmentation faults that was not easy to spot. Furthermore, it was not parsing by pipes anymore but parsing by quotation marks. I feel like my code is still jam packed and not as refactored as possible if that makes sense.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  _start here_ Purpose of signals in a Linux system is to determine how a process behaves when it is delivered a signal, the default action of receiving such a signal is to terminate the process. This is different from other forms of interprocess communication in ways of blocking execution until the signal is delivered, at which point the kernal returns info about the signal to the caller. Furthermore, signals can be asynchronous as well.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  _start here_
    sigwaitinfo(2), sigtimedwait(2), and sigwait(3) suspend execution until one of the signals in a specified set is delivered. Each of these calls returns information about the delivered signal.

    signalfd(2) returns a file descriptor that can be used to read information about signals that are delivered to the caller. Each read(2) from this file descriptor blocks until one of the signals in the set specified in the signalfd(2) call is delivered to the caller. The buffer returned by read(2) contains a structure describing the signal.

    SIGEMT is not specified in POSIX.1-2001, but nevertheless appears on most other UNIX systems, where its default action is typically to terminate the process with a core dump.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  _start here_ The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored. This is built into Linux system in such a way to control processes with an 100% stop rate when evoked. 
