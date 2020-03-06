//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2020 Carl Albrecht <carl.albrecht@griffithuni.edu.au>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_RPC_SERVER_HPP
#define HEADER_RPC_SERVER_HPP

#include <memory>
#include <string>

#include <rpc/server.h>

#include "rpc/background_task.hpp"
#include "utils/log.hpp"

namespace rpc {


class Server : public BackgroundTask<Server>
{
    friend BackgroundTaskCRTP;

    using player_id_t = unsigned int;

public:
    enum DriftDirection { LEFT, RIGHT, _COUNT };

private:
    Server() = default;

protected:
            void run();
    virtual void register_methods(class rpc::server& rpc_server);

public:
    static void        disable_player_controls(player_id_t pid, bool disable);
    static bool        game_running();
    static std::string hello(const std::string& echo);
    static player_id_t player_count();
    static void        set_firing(player_id_t pid, bool firing);
    static void        start_drifting(player_id_t pid, DriftDirection direction);
    static void        stop_drifting(player_id_t pid);
    static void        use_nitrous(player_id_t pid, bool enable);
};


extern std::unique_ptr<Server> server;


}

#endif // HEADER_RPC_SERVER_HPP
