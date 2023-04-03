#include <string.h>
#include <musicat/cmds.h>
#include <string>

namespace musicat
{
namespace command
{
namespace seek
{
dpp::slashcommand
get_register_obj (const dpp::snowflake &sha_id)
{
    return dpp::slashcommand ("seek",
                              "Seek [currently playing] track",
                              sha_id)
        .add_option (
            dpp::command_option (dpp::co_string, "to",
                                 "Timestamp [to seek to]. Format: Absolute `<[hour:][minute:]second>`. Example: 4:20",
                                 true));
}

char
_valid_number (std::string &numstr)
{
    if (!numstr.length ())
        return -1;

    static const char *numbers = "1234567890";

    for (const char c : numstr)
        {
            bool valid = false;
            for (size_t i = 0; i < strlen(numbers); i++)
                {
                    const char n = numbers[i];
                    if (c == n)
                        {
                            valid = true;
                            break;
                        }
                }

            if (!valid)
                return c;
        }

    return 0;
}

bool
_reply_invalid_number (const dpp::interaction_create_t &event, char rechar)
{
    if (rechar != 0)
        {
            if (rechar == -1)
                event.reply ("Empty number in argument");
            else
                event.reply (std::string ("Invalid number: `") + rechar + '`');
            return true;
        }

    return false;
}

bool
_reply_invalid_60 (const dpp::interaction_create_t &event, int &num, const char *name)
{
    if (num > 59 || num < 0)
        {
            event.reply (std::string ("Invalid ") + name + ": `" + std::to_string (num) + '`');
            return true;
        }

    return false;
}

bool
_reply_invalid_minute (const dpp::interaction_create_t &event, int &minute)
{
    return _reply_invalid_60 (event, minute, "minute");
}

bool
_reply_invalid_second (const dpp::interaction_create_t &event, int &second)
{
    return _reply_invalid_60 (event, second, "second");
}

bool
_reply_invalid_hour (const dpp::interaction_create_t &event, int &hour)
{
    if (hour > 23 || hour < 0)
        {
            event.reply (std::string ("Invalid hour: `") + std::to_string (hour) + '`');
            return true;
        }

    return false;
}

std::string
_pad0 (std::string str)
{
    if (str.length () < 2)
        str = '0' + str;
    return str;
}

void
slash_run (const dpp::interaction_create_t &event,
           player::player_manager_ptr player_manager)
{
    std::string arg_to = "";
    get_inter_param (event, "to", &arg_to);

    int64_t seek_byte = 0;

    auto player = player_manager->get_player (event.command.guild_id);

    if (!player || player->current_track.raw.is_null () || !player->queue.size ())
        {
            event.reply ("Not playing anything");
            return;
        }

    // !TODO: probably add a mutex for safety just in case?
    player::MCTrack &track = player->current_track;
    const uint64_t duration
        = track.info.raw.is_null () ? 0 : track.info.duration ();

    if (!track.seekable || !duration)
        {
            event.reply ("I'm sorry but the current track is unseek-able. "
                         "Might be missing metadata or unsupported format");
            return;
        }

    const size_t max_index = arg_to.length () - 1;
    size_t index = arg_to.find (":", 0);

    int hour = 0;
    int minute = 0;
    int second = -1;

    if (index == std::string::npos)
        {
            const char rechar = _valid_number (arg_to);

            if (_reply_invalid_number (event, rechar))
                return;

            second = atoi (arg_to.c_str ());

            if (_reply_invalid_second (event, second))
                return;
        }
    else
        {
            std::string first_n
                = index == max_index ? "" : arg_to.substr (0, index);

            const char rechar1 = _valid_number (first_n);

            if (_reply_invalid_number (event, rechar1))
                return;

            index = arg_to.find (":", index + 1);

            if (index == std::string::npos)
                {
                    std::string second_n
                        = index == max_index
                              ? ""
                              : arg_to.substr (first_n.length () + 1);

                    const char rechar2 = _valid_number (second_n);

                    if (_reply_invalid_number (event, rechar2))
                        return;

                    minute = atoi (first_n.c_str ());

                    if (_reply_invalid_minute (event, minute))
                        return;

                    second = atoi (second_n.c_str ());

                    if (_reply_invalid_second (event, second))
                        return;
                }
            else
                {
                    const size_t first_n_len = first_n.length ();
                    std::string second_n
                        = index == max_index
                              ? ""
                              : arg_to.substr (first_n_len + 1,
                                               index - first_n_len - 1);

                    const char rechar2 = _valid_number (second_n);

                    if (_reply_invalid_number (event, rechar2))
                        return;

                    std::string third_n
                        = index == max_index ? "" : arg_to.substr (index + 1);

                    const char rechar3 = _valid_number (third_n);

                    if (_reply_invalid_number (event, rechar3))
                        return;

                    hour = atoi (first_n.c_str ());

                    if (_reply_invalid_hour (event, hour))
                        return;

                    minute = atoi (second_n.c_str ());

                    if (_reply_invalid_minute (event, minute))
                        return;

                    second = atoi (third_n.c_str ());

                    if (_reply_invalid_second (event, second))
                        return;
                }
        }

    const bool debug = get_debug_state ();

    if (debug)
        {
            printf ("[seek::slash_run] [arg_to] [hour] [minute] [second]: "
                    "'%s' %d %d %d\n",
                    arg_to.c_str (), hour, minute, second);
        }

    if (hour == 0 && minute == 0 && second == 0)
        seek_byte = 1;
    else
        {
            static const constexpr uint64_t second_ms = 1000;
            static const constexpr uint64_t minute_ms = second_ms * 60;
            static const constexpr uint64_t hour_ms = 60 * minute_ms;

            const uint64_t total_ms = (hour * hour_ms) + (minute * minute_ms) + (second * second_ms);
            if (debug)
                printf ("[seek::slash_run] [total_ms] [duration]: %ld %ld\n", total_ms, duration);

            if (total_ms > duration)
                {
                    event.reply ("Can't seek, seek target exceeding track duration");
                    return;
                }

            float byte_per_ms = (float)track.filesize / (float)duration;

            seek_byte = (int64_t)(byte_per_ms * total_ms);

            if (debug)
                {
                    printf ("[seek::slash_run] [filesize] [duration] [byte_per_ms] [seek_byte]: "
                    "%f %f %f %ld\n",
                    (float)track.filesize, (float)duration,
                    byte_per_ms, seek_byte);
                }
        }

    if (second == -1)
        second = 0;

    std::string ts_str = _pad0 (std::to_string (hour)) + ':'
                         + _pad0 (std::to_string (minute)) + ':'
                         + _pad0 (std::to_string (second));

    if (seek_byte < 1)
        {
            event.reply ("Invalid timestamp: `" + ts_str + '`');
            return;
        }

    track.seek_to = seek_byte;

    event.reply ("Seeking to " + ts_str);
}

} // seek
} // command
} // musicat