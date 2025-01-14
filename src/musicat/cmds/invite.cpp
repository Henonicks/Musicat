#include "musicat/cmds.h"
#include "musicat/musicat.h"

namespace musicat
{
namespace command
{
namespace invite
{
dpp::slashcommand
get_register_obj (const dpp::snowflake &sha_id)
{
    return dpp::slashcommand ("invite", "My invite link", sha_id);
}

void
slash_run (const dpp::slashcommand_t &event)
{
    std::string invite = get_invite_link ();
    event.reply (!invite.empty () ? "** [ ❤️ "
                                    "](" + invite
                                        + ") **"
                                  : "I don't have invite link configured");
}
} // invite
} // command
} // musicat
