
#include "../../include/minishell.h"

void assign_roles(t_token **tree)
{
    t_token *curr = *tree;
    int command_found = 0;

    while (curr)
    {
        // Reset if we see a pipe
        if (curr->type == TOKEN_PIPE)
            command_found = 0;

        // If we haven't found a command yet, the first WORD is command
        if (!command_found && curr->type == TOKEN_WORD)
        {
            curr->type = TOKEN_COMMAND;
            command_found = 1;
        }
        // Otherwise, subsequent WORD/VAR/FIELD => arguments
        else if (command_found && 
                (curr->type == TOKEN_WORD ||
                 curr->type == TOKEN_FIELD ||
                 curr->type == TOKEN_EXP_FIELD ||
                 curr->type == TOKEN_VAR))
        {
            curr->type = TOKEN_ARGUMENT;
        }

        // If redirection, next token => file
        if (curr->type == TOKEN_REDIRECT_IN  ||
            curr->type == TOKEN_REDIRECT_OUT ||
            curr->type == TOKEN_REDIRECT_APPEND)
        {
            if (curr->next)
                curr->next->type = TOKEN_FILE;
        }
        // If HERE-DOC, next => marker
        else if (curr->type == TOKEN_HEREDOC)
        {
            if (curr->next)
                curr->next->type = TOKEN_HEREDOC_MARKER;
        }
        curr = curr->next;
    }
}

int check_pipe_errors(t_token *tokens)
{
    // No tokens => no error or depends on your logic
    if (!tokens) return 0;

    // 1) If first token is pipe => error
    if (tokens->type == TOKEN_PIPE)
    {
        fprintf(stderr, "Syntax error: pipe at start\n");
        return 1;
    }

    t_token *curr = tokens;
    while (curr)
    {
        if (curr->type == TOKEN_PIPE)
        {
            // Check next => if no next or next is also pipe => error
            if (!curr->next || curr->next->type == TOKEN_PIPE)
            {
                fprintf(stderr, "Syntax error: pipe misplaced\n");
                return 1;
            }
        }
        curr = curr->next;
    }

    // 2) If last token is pipe => error
    t_token *tail = tokens;
    while (tail->next) tail = tail->next;
    if (tail->type == TOKEN_PIPE)
    {
        fprintf(stderr, "Syntax error: pipe at end\n");
        return 1;
    }
    return 0; // no error
}

int check_redirect_errors(t_token *tokens)
{
    if (!tokens) return 0; // no tokens => no error or your logic

    t_token *curr = tokens;
    int has_command = 0; // to track if we've seen at least one command

    while (curr)
    {
        // If we see a command
        if (curr->type == TOKEN_COMMAND)
            has_command = 1;

        if (curr->type == TOKEN_REDIRECT_IN ||
            curr->type == TOKEN_REDIRECT_OUT ||
            curr->type == TOKEN_REDIRECT_APPEND ||
            curr->type == TOKEN_HEREDOC)
        {
            // Next must be file or here-doc marker
            if (!curr->next ||
                curr->next->type == TOKEN_PIPE ||
                curr->next->type == TOKEN_REDIRECT_IN ||
                curr->next->type == TOKEN_REDIRECT_OUT ||
                curr->next->type == TOKEN_REDIRECT_APPEND ||
                curr->next->type == TOKEN_HEREDOC)
            {
                fprintf(stderr, 
                    "Syntax error: missing file after redirection\n");
                return 1;
            }
        }
        curr = curr->next;
    }

    // Check if we found no command at all (like: "> file" only)
    if (!has_command)
    {
        fprintf(stderr, "Syntax error: no command found\n");
        return 1;
    }
    return 0; // no error
}

int validate_syntax_and_adjust(t_token **tree)
{
    // 1) Assign roles (like old adjusting_token_tree)
    assign_roles(tree);

    // 2) Check pipes
    if (check_pipe_errors(*tree))
        return 1; // error

    // 3) Check redirections
    if (check_redirect_errors(*tree))
        return 1; // error

    // No errors
    return 0;
}