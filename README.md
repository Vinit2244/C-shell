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
- If flag -e is provided and only one file is found then a maximum of 9999999 characters will be read and printed as output even if the file is bigger than that

## Echo
- Handling echo by myself and not using execvp()