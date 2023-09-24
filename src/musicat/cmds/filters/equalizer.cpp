/*
Apply 18 band equalizer.

The filter accepts the following options:

1b

    Set 65Hz band gain.
2b

    Set 92Hz band gain.
3b

    Set 131Hz band gain.
4b

    Set 185Hz band gain.
5b

    Set 262Hz band gain.
6b

    Set 370Hz band gain.
7b

    Set 523Hz band gain.
8b

    Set 740Hz band gain.
9b

    Set 1047Hz band gain.
10b

    Set 1480Hz band gain.
11b

    Set 2093Hz band gain.
12b

    Set 2960Hz band gain.
13b

    Set 4186Hz band gain.
14b

    Set 5920Hz band gain.
15b

    Set 8372Hz band gain.
16b

    Set 11840Hz band gain.
17b

    Set 16744Hz band gain.
18b

    Set 20000Hz band gain.

action: set balance reset
up the volume threshold to 500
*/

#include "appcommand.h"
#include "musicat/cmds.h"
#include "musicat/musicat.h"
#include "musicat/util.h"

namespace musicat
{
namespace command
{
namespace filters
{
namespace equalizer
{

static inline constexpr const char *eq_options[][2] = {
    { "band-1", "Set 65Hz band gain" },
    { "band-2", "Set 92Hz band gain" },
    { "band-3", "Set 131Hz band gain" },
    { "band-4", "Set 185Hz band gain" },
    { "band-5", "Set 262Hz band gain" },
    { "band-6", "Set 370Hz band gain" },
    { "band-7", "Set 523Hz band gain" },
    { "band-8", "Set 740Hz band gain" },
    { "band-9", "Set 1047Hz band gain" },
    { "band-10", "Set 1480Hz band gain" },
    { "band-11", "Set 2093Hz band gain" },
    { "band-12", "Set 2960Hz band gain" },
    { "band-13", "Set 4186Hz band gain" },
    { "band-14", "Set 5920Hz band gain" },
    { "band-15", "Set 8372Hz band gain" },
    { "band-16", "Set 11840Hz band gain" },
    { "band-17", "Set 16744Hz band gain" },
    { "band-18", "Set 20000Hz band gain" },
};

void
setup_subcommand (dpp::slashcommand &slash)
{
    constexpr size_t arg_size = (sizeof (eq_options) / sizeof (*eq_options));
    constexpr int argpc = 18;

    for (size_t i = 0; i < (arg_size / argpc); i++)
        {
            dpp::command_option eqsubcmd (dpp::co_sub_command, "equalizer",
                                          "Apply 18 band equalizer");

            eqsubcmd.add_option (
                dpp::command_option (dpp::co_string, "action",
                                     "What you wanna do?", false)
                    .add_choice (dpp::command_option_choice ("Set", "set"))
                    .add_choice (dpp::command_option_choice ("Balance", "balance"))
                    .add_choice (dpp::command_option_choice ("Reset", "reset")));

            for (size_t j = i * argpc; j < ((i+1) * argpc) && j < arg_size; j++)
                {
                    eqsubcmd.add_option (
                        dpp::command_option (dpp::co_integer, eq_options[j][0],
                                             eq_options[j][1], false)
                            .set_min_value (1)
                            .set_max_value (150));
                }

            slash.add_option (eqsubcmd);
        }
}

static inline constexpr const command_handlers_map_t action_handlers
    = { { "", show },
        { "set", set },
        { "balance", balance },
        { "reset", reset },
        { NULL, NULL } };

void
show (const dpp::slashcommand_t &event)
{
    event.reply ("show");
}

void
set (const dpp::slashcommand_t &event)
{
    event.reply ("set");
}

void
balance (const dpp::slashcommand_t &event)
{
    event.reply ("balance");
}

void
reset (const dpp::slashcommand_t &event)
{
    event.reply ("reset");
}

void
slash_run (const dpp::slashcommand_t &event)
{
    // argument
    std::string arg_action = "";
    get_inter_param (event, "action", &arg_action);

    handle_command ({ arg_action, action_handlers, event });
}

} // equalizer

} // filters
} // command
} // musicat
