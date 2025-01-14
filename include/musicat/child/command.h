#ifndef MUSICAT_CHILD_COMMAND_H
#define MUSICAT_CHILD_COMMAND_H

#include "musicat/child.h"
#include <string>

namespace musicat
{
namespace child
{
namespace command
{

// update worker::execute and related routines when changing this
inline const struct
{
    const std::string create_audio_processor = "cap";
    const std::string shutdown = "shut";
} command_execute_commands_t;

// update set_option impl in child/command.cpp when changing this
inline const struct
{
    const std::string command = "cmd";  // str
    const std::string file_path = "fp"; // str
    const std::string debug = "dbg";    // bool
    const std::string id = "id";        // str
    const std::string guild_id = "gid"; // str
    const std::string ready = "rdy";    // bool
    const std::string seek = "sk";      // bool
    const std::string volume = "vl";    // bool

    /**
     * Effect chain per helper processor as ffmpeg
     * audio effect command, can be provided more than once
     *
     * Careful as creating too many helper
     * processor will severely add delay buffer
     * to audio playback
     */
    const std::string helper_chain = "ehl"; // str
    const std::string force = "frc";        // bool
} command_options_keys_t;

// update create_command_options impl below when changing this struct
struct command_options_t
{
    std::string command;

    std::string file_path;
    bool debug;
    std::string id;

    pid_t pid;
    int parent_read_fd;
    int parent_write_fd;
    int child_read_fd;
    int child_write_fd;

    std::string audio_stream_fifo_path;
    std::string guild_id;
    /**
     * Ready status
     */
    int ready;
    std::string seek;
    std::string audio_stream_stdin_path;
    std::string audio_stream_stdout_path;
    int volume;
    /**
     * A list with format `@effect_args@` without separator
     * Need to be parsed to processor_options_t helper_chain list
     */
    std::string helper_chain;

    /**
     * Not every command support this flag
     * Some will just ignore it
     */
    bool force;
};

static inline command_options_t
create_command_options ()
{
    return { "", "", false, "", -1, -1, -1,  -1, -1,
             "", "", false, "", "", "", 100, "", false };
}

void command_queue_routine ();

void wait_for_command ();

void run_command_thread ();

int send_command (const std::string &cmd);

void wake ();

// default values are used in the main thread ONLY! You should specify
// write_fd and caller in child processes
void write_command (const std::string &cmd,
                    const int write_fd = *get_parent_write_fd (),
                    const char *caller = "child::command");

// should be called before send_command when setting value
// and use the return string as value
std::string sanitize_command_value (const std::string &value);

// mostly internal use
std::string sanitize_command_key_value (const std::string &key_value);

void parse_command_to_options (const std::string &cmd,
                               command_options_t &options);

int wait_slave_ready (std::string &id, const int timeout);

int mark_slave_ready (std::string &id, const int status = 0);

} // command
} // child
} // musicat

#endif // MUSICAT_CHILD_COMMAND_H
