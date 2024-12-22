/***************************************************
 * mini_exec_test.c
 *
 * This file demonstrates how to build a “fake”
 * linked-list of commands with redirections
 * for debugging an executor in a minishell project.
 *
 ***************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REDIR_IN       0
#define REDIR_OUT      1
#define REDIR_APPEND   2
#define REDIR_HEREDOC  3

typedef struct s_redirection
{
    char                    *filename;
    int                     fd;
    int                     is_append;
    int                     is_heredoc;
    char                    *heredoc_marker;
    int                     type;
    struct s_redirection    *next;
}   t_redirection;

typedef struct s_exec_command
{
    char                    *cmd_name;
    char                    *exec_path;
    char                    **args;
    int                     pipe_fds[2];
    t_redirection           *redirects;
    int                     exit_status;
    struct s_exec_command   *next_cmd;
    struct s_exec_command   *prev_cmd;
}   t_exec_command;

/**
 * Utility function to create and zero-initialize a new command struct.
 */
static t_exec_command *create_new_command(void)
{
    t_exec_command *cmd = (t_exec_command *)calloc(1, sizeof(t_exec_command));
    if (!cmd)
        return NULL;
    // Initialize pipe_fds to something (e.g., -1) to indicate uninitialized
    cmd->pipe_fds[0] = -1;
    cmd->pipe_fds[1] = -1;
    return cmd;
}

/**
 * Utility function to create a new redirection struct.
 */
static t_redirection *create_new_redir(const char *filename,
                                       int fd,
                                       int is_append,
                                       int is_heredoc,
                                       const char *heredoc_marker,
                                       int type)
{
    t_redirection *redir = (t_redirection *)calloc(1, sizeof(t_redirection));
    if (!redir)
        return NULL;

    if (filename)
        redir->filename = strdup(filename);

    redir->fd = fd;
    redir->is_append = is_append;
    redir->is_heredoc = is_heredoc;

    if (heredoc_marker)
        redir->heredoc_marker = strdup(heredoc_marker);

    redir->type = type;

    return redir;
}

/**
 * Build a fake linked list of commands for testing.
 * Returns the head of the list (Command #1).
 *
 * This example pipeline is:
 *   (1) echo "Hello from command1"
 *    |
 *   (2) grep Hello < my_input_file >> my_output_file
 *    |
 *   (3) wc -l << EOF
 */t_exec_command *build_fake_command_list(void)
{
    /*
     * --------------------------------
     * Command #1: cat
     * --------------------------------
     */
    t_exec_command *cmd1 = create_new_command();
    cmd1->cmd_name = strdup("cat");
    cmd1->exec_path = NULL;
    // Build args
    char **args1 = (char **)calloc(2, sizeof(char *));
    args1[0] = strdup("cat");
    args1[1] = NULL;
    cmd1->args = args1;
    cmd1->redirects = NULL;
    cmd1->exit_status = 0;

    /*
     * --------------------------------
     * Command #2: cat
     * --------------------------------
     */
    t_exec_command *cmd2 = create_new_command();
    cmd2->cmd_name = strdup("cat");
    cmd2->exec_path = NULL;
    // Build args
    char **args2 = (char **)calloc(2, sizeof(char *));
    args2[0] = strdup("cat");
    args2[1] = NULL;
    cmd2->args = args2;
    cmd2->redirects = NULL;
    cmd2->exit_status = 0;

    /*
     * --------------------------------
     * Command #3: ls
     * --------------------------------
     */
    t_exec_command *cmd3 = create_new_command();
    cmd3->cmd_name = strdup("ls");
    cmd3->exec_path = NULL;
    // Build args
    char **args3 = (char **)calloc(2, sizeof(char *));
    args3[0] = strdup("ls");
    args3[1] = NULL;
    cmd3->args = args3;
    cmd3->redirects = NULL;
    cmd3->exit_status = 0;

    /*
     * Link the commands together: cmd1 -> cmd2 -> cmd3
     */
    cmd1->next_cmd = cmd2;
    cmd2->prev_cmd = cmd1;

    cmd2->next_cmd = cmd3;
    cmd3->prev_cmd = cmd2;

    // Return the head of this chain
    return cmd1;
}

// int main(void)
// {
//     // Build the fake command list
//     t_exec_command *cmd_list = build_fake_command_list();
    
//     // Pass cmd_list to your teammate’s executor here, or
//     // just print to verify the structure is correct:
//     t_exec_command *current = cmd_list;
//     while (current)
//     {
//         printf("Command: %s\n", current->cmd_name);

//         // Print arguments
//         if (current->args)
//         {
//             int i = 0;
//             while (current->args[i])
//             {
//                 printf("  arg[%d]: %s\n", i, current->args[i]);
//                 i++;
//             }
//         }

//         // Print redirections
//         t_redirection *r = current->redirects;
//         while (r)
//         {
//             printf("  Redirection: type=%d, file=%s, "
//                    "is_heredoc=%d, is_append=%d\n",
//                    r->type,
//                    (r->filename ? r->filename : "(NULL)"),
//                    r->is_heredoc,
//                    r->is_append);
//             if (r->heredoc_marker)
//                 printf("    heredoc marker: %s\n", r->heredoc_marker);
//             r = r->next;
//         }
//         printf("\n");

//         current = current->next_cmd;
//     }

//     // In a real project, you would free all memory here:
//     // free(cmd_list->args[..]), free(cmd_list->redirects, ...), etc.

//     return 0;
// }
