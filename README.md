# Minishell

![C](https://img.shields.io/badge/language-C-blue)
![Unix](https://img.shields.io/badge/platform-Unix-lightgrey)
![42 School](https://img.shields.io/badge/project-42%20School-black)

A small Unix shell written in C.

This project is a simplified implementation of a Unix shell inspired by Bash.
It was developed as part of the 42 School curriculum to understand how shells work internally.

The program reads user commands, parses them, handles pipes and redirections,
executes built-in commands and external programs, and manages environment variables.

--------------------------------------------------

# Features

## Interactive shell

- command prompt
- command history using readline

## Built-in commands

Supported built-ins:

- echo
- cd
- pwd
- export
- unset
- env
- exit

## Pipes

Execute commands connected by pipes.

Example:

    ls | grep minishell

## Redirections

Supported redirections:

    >   output redirect
    >>  append redirect
    <   input redirect
    <<  heredoc

Example:

    echo hello > file.txt
    cat < file.txt

## Environment variables

Supports expansion of environment variables.

Example:

    export NAME=Vlad
    echo $NAME

Special variable:

    $?

Returns the exit status of the last executed command.

## Signal handling

Correct handling of signals:

- Ctrl + C
- Ctrl + D
- Ctrl + \

--------------------------------------------------

# Project Structure

    mini-shell/
    │
    ├── include/            header files
    │
    ├── src/
    │   ├── builtins/       built-in commands
    │   ├── env/            environment variables
    │   ├── executor/       command execution
    │   ├── lexer/          tokenization
    │   ├── parser/         command parsing
    │   ├── signals/        signal handling
    │   ├── utils/          helper functions
    │   └── main.c
    │
    ├── libft/              custom C library
    ├── get_next_line/      line reader
    │
    └── Makefile

--------------------------------------------------

# Installation

Clone the repository:

    git clone https://github.com/KhersonV/mini-shell.git
    cd mini-shell

--------------------------------------------------

# Compilation

Compile the project:

    make

Available commands:

    make        build minishell
    make clean  remove object files
    make fclean remove executable and object files
    make re     rebuild project

--------------------------------------------------

# Usage

Run the shell:

    ./minishell

Example prompt:

    minishell>

Now you can run commands like in a normal shell.

--------------------------------------------------

# Examples

Simple command:

    ls

Command with arguments:

    echo Hello minishell

Pipe:

    cat file.txt | grep hello

Redirection:

    echo Hello > file.txt

Environment variables:

    export USER=Vlad
    echo $USER

--------------------------------------------------

# Dependencies

The project requires:

- gcc
- make
- readline
- libft
- get_next_line

Install readline if needed.

Ubuntu / Debian:

    sudo apt install libreadline-dev

MacOS:

    brew install readline

--------------------------------------------------

# Learning Goals

This project helps understand:

- how shells work internally
- process creation (fork)
- program execution (execve)
- pipes and file descriptors
- command parsing
- environment variables
- signal handling

--------------------------------------------------

# Author

Vladyslav Mamotenko
Serhii Nazarov

GitHub:
https://github.com/KhersonV
