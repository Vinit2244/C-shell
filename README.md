[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/76mHqLr5)
# Description

# Assumptions

## General
- '&' and ';' always occur in special sense.
- Absolute file paths always begin with a '/' for example "/Users/vinitmehta/desktop/abcd"
- In the warp command if multiple commands are given separated by space ex: "warp .. test" then the complete command is saved in pastevents.

## Warp
- File path does not contain any spaces.
- When the output of warp is redirected into some file then the file gets created into the final path of the folder.

## Peek
- "peek -" gives error

## Background processes

## Seek
- Assuming that if the base directory provided in relative to the current folder starts with "." always
- If flag -e is provided and only one file is found then a maximum of 100000 characters will be read and printed as output even if the file is bigger than that
- Only file/folder name can be provided throught the input of '<' (redirection input)

## Proclore
- I am considering all the processes spawned by my shell background process only as background process all other processes spawned by the PC terminal is considered as foreground only

## IO Redirection
- In input output redirection the errors which are printed in color on terminal is not printed in color in the text file it is just written normally without color coding
- I/O and piping are not provided as background process

## Ping
- Assuming the process can only be blocked by SIGSTOP = 19 signal through ping command and can be resumed only through SIGCONT = 18 and killed only using SIGKILL = 9

## System Commands
- If some invalid command is passed to be run as background process then it's pid will get stored and it would print the error but won't execute