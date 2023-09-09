[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description

# Assumptions
### General:
- '&' and ';' always occur in special sense.
- Absolute file paths always begin with a '/' for example "/Users/vinitmehta/desktop/abcd".
- In the warp command if multiple commands are given separated by space ex: "warp .. test" then the complete command is saved in pastevents.
- assuming both the <filename/path> along with input redirection won't be provided to warp and peek.
- Multiple output redirections are not given in the same command same for multiple input redirections

### Warp:
- File path does not contain any spaces.
- When the output of warp is redirected into some file then the file gets created into the final path of the folder.
- Warp does not support input redirection so whenever some input redirection is provided warp just changes the cwd to home directory

### Peek:
- "peek -" gives error.
- Peek does not accept input using input redirection '<' so we just peek cwd

### Background processes:
- If some invalid command is passed to be run as background process then it's pid will get stored and it would print the error but won't execute and in the next run the terminal will print process exited abnormally.

### Seek:
- Assuming that if the base directory provided in relative to the current folder starts with "." always.
- If flag -e is provided and only one file is found then a maximum of 100000 characters will be read and printed as output even if the file is bigger than that.
- Only path of the file to seek can be provided throught the input of '<' (redirection input).

### Proclore:
- I am considering all the processes spawned by my shell background process only as background process all other processes spawned by the PC terminal is considered as foreground only.

### IO Redirection:
- In input output redirection the errors which are printed in color on terminal is not printed in color in the text file it is just written normally without color coding.
- I/O and piping are not provided as background process.

### Ping:
- Assuming the process can only be blocked by SIGSTOP = 19 signal through ping command and can be resumed only through SIGCONT = 18 and killed only using SIGKILL = 9.

### Pipe:
- When piping '|' is used then the program creates a new temp file to store the output of the pipe 1st command temporarily names "a_a.txt" so it is assumed that the user never creates a custom file names "a_a.txt".


# To-do:
- [ ] Replace all perror with normal bprintf using color coding
- [ ] Implement IO for iMan
- [ ] iMan not working
- [x] Implement input and output redirection when provided both concurrently
- [x] Implement fg for vim and gedit
- [ ] Implement piping
- [ ] Ctrl C, Z, D implementation remaining