/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:00:00 by admin             #+#    #+#             */
/*   Updated: 2025/01/05 18:59:18 by vmamoten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

void disable_echoctl(void)
{
    struct termios term;
    if (tcgetattr(STDIN_FILENO, &term) == -1)
        return;
    term.c_lflag &= ~(ECHOCTL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term);
}

// Это handler для режима "ждём ввод":
static void sigint_handler_readline(int signo)
{
    (void)signo;
    // Очистить текущую строку и вывести prompt
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}

// Отключаем SIGQUIT (Ctrl+\)
static void ignore_sigquit(int signo)
{
    (void)signo;
}

// Установка «обычных» сигналов в родителе
void init_signals(void)
{
    struct sigaction sa_int;
    struct sigaction sa_quit;

    // Игнорируем SIGQUIT
    sa_quit.sa_handler = ignore_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    // SIGINT => стираем строку
    sa_int.sa_handler = sigint_handler_readline;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    disable_echoctl(); // не показывать ^C
}

// Сбросить в дефолт (в дочернем процессе)
void reset_signals_to_default(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
