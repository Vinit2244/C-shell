# Description

# Assumptions

## General
- '&' and ';' always occur in special sense.
- Absolute file paths always begin with a '/' for example "/Users/vinitmehta/desktop/abcd"
- In the warp command if multiple commands are given separated by space ex: "warp .. test" then the complete command is saved in pastevents.

## Warp
- File path does not contain any spaces.

## Peek
- "peek -" gives error

## Background processes

## Seek
- If flag -e is provided and only one file is found then a maximum of 100000 characters will be read and printed as output even if the file is bigger than that

## Echo
- Handling echo by myself and not using execvp()

## Proclore
- I am considering all the processes spawned by my shell background process only as background process all other processes spawned by the PC terminal is considered as foreground only