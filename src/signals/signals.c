/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vmamoten <vmamoten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 20:00:00 by admin             #+#    #+#             */
/*   Updated: 2025/01/06 13:19:44 by vmamoten         ###   ########.fr       */
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
    
	
	if (g_commands && g_commands->cmd_name && g_commands->cmd_name[0] != '\0')
    {
        // Выводим ^C + перенос строки
        write(STDOUT_FILENO, "^C\n", 3);

        // Если хотим совсем «отключить» перерисовку Readline:
        // НЕ вызываем rl_on_new_line() / rl_replace_line() / rl_redisplay()
        // Тогда Readline «зависнет» в старом состоянии ввода,
        // но выведется ^C и строка (prompt) не перерисуется.
        // Если это поведение нужно – всё, выходим из функции.

        return;
    }
    else
    {
        // Стандартное поведение: перенос строки, очистка, перерисовка
        write(STDOUT_FILENO, "\n", 1);
        rl_on_new_line();
        rl_replace_line("", 0);
        rl_redisplay();
    }
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



// static void sigint_handler_execution(int signo)
// {
//     (void)signo;
//     // Обычно bash печатает ^C (если ECHOCTL включен) и делает перевод строки
//     // Показать перевод строки можно, если хотите "разделить" вывод:
//     write(STDOUT_FILENO, "\n", 1);

//     // НЕ вызываем rl_replace_line, потому что мы в режиме выполнения команды
//     // При желании можно убить дочерние процессы: kill(pid, SIGINT) или kill(0, SIGINT).
//     // Но если в дочернем сброшен сигнал по умолчанию, она сама прервётся.
// }

// void init_signals_execution(void)
// {
//     struct sigaction sa_int, sa_quit;

//     // Можно снова игнорировать SIGQUIT, если хотите
//     sa_quit.sa_handler = SIG_IGN;
//     sigemptyset(&sa_quit.sa_mask);
//     sa_quit.sa_flags = 0;
//     sigaction(SIGQUIT, &sa_quit, NULL);

//     // Ctrl+C => печатаем \n, не трём строку
//     sa_int.sa_handler = sigint_handler_execution;
//     sigemptyset(&sa_int.sa_mask);
//     sa_int.sa_flags = SA_RESTART;
//     sigaction(SIGINT, &sa_int, NULL);

//     // Если хотите, чтобы `^C` было видно, можно включить echoctl:
//      enable_echoctl();
// }
