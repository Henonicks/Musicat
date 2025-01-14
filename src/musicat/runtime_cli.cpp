#include "musicat/runtime_cli.h"
#include "musicat/musicat.h"
#include "musicat/thread_manager.h"
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <thread>

void
_print_pad (size_t len)
{
    if ((long)len < 0L)
        return;

    for (size_t i = 0; i < len; i++)
        {
            fprintf (stderr, " ");
        }
}

namespace musicat
{
namespace runtime_cli
{
bool attached = false;

static inline const std::map<std::pair<const char *, const char *>,
                             const char *>
    commands = {
        { { "command", "alias" }, "description" },
        { { "help", "-h" }, "Print this message" },
        { { "debug", "-d" }, "Toggle debug mode" },
        { { "clear", "-c" }, "Clear console" },
    };

int
attach_listener ()
{
    if (attached == true)
        {
            fprintf (stderr,
                     "[ERROR RUNTIME_CLI] stdin listener already attached!\n");
            return 1;
        }

    fprintf (stderr, "[INFO] Enter `-d` to toggle debug mode\n");

    std::thread stdin_listener ([] () {
        thread_manager::DoneSetter tmds;
        attached = true;

        size_t padding_command = 0;
        size_t padding_alias = 0;

        for (std::pair<std::pair<const char *, const char *>, const char *>
                 desc : commands)
            {
                const size_t len_command = strlen (desc.first.first) + 1U;
                const size_t len_alias = strlen (desc.first.second) + 2U;

                if (len_command > padding_command)
                    padding_command = len_command;

                if (len_alias > padding_alias)
                    padding_alias = len_alias;
            }

        struct pollfd stdinpfds[1];
        stdinpfds[0].events = POLLIN;
        stdinpfds[0].fd = STDIN_FILENO;

        while (get_running_state ())
            {
                std::string cmd;

                // poll for 3 seconds every iteration
                const int read_has_event = poll (stdinpfds, 1, 3000);
                const bool read_ready
                    = (read_has_event > 0) && (stdinpfds[0].revents & POLLIN);

                if (read_ready)
                    std::cin >> cmd;
                else
                    continue;

                if (cmd == "help" || cmd == "-h")
                    {
                        fprintf (stderr,
                                 "Usage: [command] [args] <ENTER>\n\n");

                        for (std::pair<std::pair<const char *, const char *>,
                                       const char *>
                                 desc : commands)
                            {
                                fprintf (stderr, "%s", desc.first.first);

                                _print_pad (padding_command
                                            - strlen (desc.first.first));

                                fprintf (stderr, ":");

                                const size_t pad_a
                                    = padding_alias
                                      - strlen (desc.first.second);

                                _print_pad (pad_a / 2);

                                fprintf (stderr, "%s", desc.first.second);

                                _print_pad ((size_t)ceil ((double)pad_a
                                                          / (double)2.0));

                                fprintf (stderr, ": %s\n", desc.second);
                            }
                    }
                else if (cmd == "debug" || cmd == "-d")
                    {
                        set_debug_state (!get_debug_state ());
                    }
                else if (cmd == "clear" || cmd == "-c")
                    {
                        system ("clear");
                    }
            }
    });

    thread_manager::dispatch (stdin_listener);

    return 0;
}
} // runtime_cli
} // musicat
