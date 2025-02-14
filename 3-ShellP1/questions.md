1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  _start here_ `fgets()` is a good choice for this application because it stops reading after an EOF or a newline is presented.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  I needed to use `malloc()` to allocate memory for `cmd_buff` in `dsh_cli.c` instead of allocating a fixed-size array because cmd_buff has to be dynamic in response to user inputs from stdin.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  It is necessary for the function `build_cmd_list(`)` to trim leading and trailing spaces from each command before storing it because it will later impact parsing through the string and identifying if it is a word or not. I experienced it the hard way when debugging through my code.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  _start here_ According to my research, we should implement redirecting stdout and stderr, then both stdout and stderr, and lastly stdout and stderr to the same file. Some challenges we might, well at least I might have implementing these are file errors, grasping the dos and don'ts with stdout, stderr, and stdin. Source: https://www.howtogeek.com/435903/what-are-stdin-stdout-and-stderr-on-linux/#reacting-to-pipes-and-redirects

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  _start here_ When a command like ls is being piped, it orients its output (stdout) in a single column form where if you redirect ls into a file, it stores the output into the file instead of stdout. Furthermore, pipes allows you to pass the result of a command into another command.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  _start here_ It is important to keep these separate in a shell to one avoid confusion and two allows for accurate and easier debugging.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  _start here_ Our custom shell should handle errors from commands that fail by returning a rc (return code) of anything below a zero and zero when there is no error. If we were to provide a way to merge stdout and stderr, it would be for debugging purposes and will hopefully utilize rc as a starter and other tactics.