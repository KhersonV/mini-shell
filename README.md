# Minishell

![C](https://img.shields.io/badge/language-C-blue)
![Unix](https://img.shields.io/badge/platform-Unix-lightgrey)
![42 School](https://img.shields.io/badge/project-42%20School-black)
![Status](https://img.shields.io/badge/status-learning%20project-green)

A small Unix shell written in **C**, inspired by **Bash**.  
This project was developed as part of the **42 School curriculum** and focuses on understanding how shells work internally: parsing commands, managing processes, pipes, redirections, and environment variables.

---

# Table of Contents

- [About](#about)
- [Features](#features)
- [Project Structure](#project-structure)
- [Installation](#installation)
- [Compilation](#compilation)
- [Usage](#usage)
- [Examples](#examples)
- [Dependencies](#dependencies)
- [Learning Goals](#learning-goals)
- [Author](#author)

---

# About

**Minishell** is a simplified implementation of a Unix shell.

The goal of the project is to recreate core shell behavior:

- read commands from the user
- parse them
- handle pipes and redirections
- execute built-in commands
- execute external programs
- manage environment variables

It helps understand how shells like **bash** work internally.

---

# Features

### Interactive shell
- command prompt
- command history using `readline`

### Built-in commands
- `echo`
- `cd`
- `pwd`
- `export`
- `unset`
- `env`
- `exit`

### Pipes
Execute commands connected by pipes:

```bash
ls | grep minishell
### Redirections

Supported redirections:

>   output redirect
>>  append redirect
<   input redirect
<<  heredoc

### Example:

echo hello > file.txt
cat < file.txt
Environment variables

### Supports expansion of variables:

export NAME=Vlad
echo $NAME

### Also supports the special variable:

$?

which returns the exit status of the last command.

### Signal handling

Correct handling of signals:

Ctrl + C

Ctrl + D

Ctrl + \

# Project Structure
mini-shell/
│
├── include/            # header files
│
├── src/
│   ├── builtins/       # built-in commands
│   ├── env/            # environment variables
│   ├── executor/       # command execution
│   ├── lexer/          # tokenization
│   ├── parser/         # command parsing
│   ├── signals/        # signal handling
│   ├── utils/          # helper functions
│   └── main.c
│
├── libft/              # custom C library
├── get_next_line/      # line reader
│
└── Makefile

# Installation

### Clone the repository:

git clone https://github.com/KhersonV/mini-shell.git
cd mini-shell
Compilation

### Compile the project using:

make

### Available commands:

make        # build minishell
make clean  # remove object files
make fclean # remove executable and object files
make re     # rebuild project
Usage

### Run the shell:

./minishell

### Example prompt:

minishell>

Now you can run commands like in a normal shell.

### Examples
Simple command
ls
Command with arguments
echo Hello minishell
Pipe
cat file.txt | grep hello
Redirection
echo Hello > file.txt
Environment variables
export USER=Vlad
echo $USER
Dependencies

### The project requires:

gcc

make

readline

libft

get_next_line

Install readline if needed:

Ubuntu / Debian:

sudo apt install libreadline-dev

### MacOS:

brew install readline
# Learning Goals

This project helps understand:

how shells work internally

process creation (fork)

program execution (execve)

pipes and file descriptors

command parsing

environment variables

signal handling

# Author

Vladyslav Mamotenko
