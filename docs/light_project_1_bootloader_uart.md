# Light Project 1: Custom Linux Shell

## Project Goal
Build a Unix shell in C that can be used in a real terminal.

## What I Will Build

- command parser
- process launcher using fork and exec
- built-in commands
- support for pipes and redirection

## Required Features

1. Basic command execution
- Parse input line into command and arguments.
- Run external programs with fork() + execvp().
- Wait for foreground processes with waitpid().

2. Built-in commands
- cd
- exit
- history
- pwd

3. Redirection
- input redirection with <
- output redirection with >

4. Pipes
- Support at least one pipeline using |.
- Example: ls | wc -l

5. Stability
- Handle empty input and invalid commands gracefully.
- Keep shell running until exit.

## Suggested Structure

- shell/src/manager/main.c
- shell/src/parser/parser.c
- shell/src/executer/executer.c
- shell/src/builtin/builtins.c
- shell/src/history/history.c

## Testing Checklist

- Built-ins work correctly.
- External commands run correctly.
- Redirection creates/reads files correctly.
- Single pipe flow works.
- Invalid command does not crash shell.

## OSTEP Connection

This project maps to process and execution concepts around fork, exec, wait, and basic shell behavior.

## Deliverable

A working shell executable that supports built-ins, pipes, and redirection in terminal.
