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

#include <karts/controller/kart_control.hpp>
#include "rpc/server.hpp"

#include "rpc/rpc_controller_manager.hpp"
#include "utils/log.hpp"

namespace rpc {


std::unique_ptr<Server> server = nullptr;

//------------------------------------------------------------------------------
void Server::run()
{
    Log::info("rpc::Server", "run() method called");

    try
    {
        class rpc::server rpc_server(42069);
        rpc_server.suppress_exceptions(true);

        register_methods(rpc_server);

        rpc_server.async_run();

        cancellation_token.wait();

        rpc_server.close_sessions();
        rpc_server.stop();
    }
    catch (std::exception& e)
    {
        Log::error("rpc::Server",
                   "Exception reached top level: %s", e.what());
    }
    catch (...)
    {
        Log::error("rpc::Server",
                   "Non-exception thrown object reached top level");
    }
}

//------------------------------------------------------------------------------
void Server::register_methods(class rpc::server& rpc_server)
{
    rpc_server.bind("game_running", game_running);
    rpc_server.bind("hello", hello);
    rpc_server.bind("disable_player_controls", disable_player_controls);
    rpc_server.bind("player_count", player_count);
    rpc_server.bind("set_firing", set_firing);
    rpc_server.bind("start_drifting", start_drifting);
    rpc_server.bind("stop_drifting", stop_drifting);
    rpc_server.bind("use_nitrous", use_nitrous);

    /* Example instance binding (we don't have any instance methods currently)
    rpc_server.bind(
            "use_nitrous",
            [this](bool enable) { use_nitrous(enable); });*/
}

//------------------------------------------------------------------------------
bool Server::game_running()
{
    // TODO This should be redone to use the actual game state, but for now, we
    //      assume that if there are extant controllers, a game is in progress
    return rpc_controller_manager->getNumberOfControllers() > 0;
}

//------------------------------------------------------------------------------
std::string Server::hello(const std::string& echo)
{
    return echo;
}

//------------------------------------------------------------------------------
void Server::disable_player_controls(player_id_t pid, bool disable)
{
    RPCController* controller = rpc_controller_manager->getController(pid);

    if (disable)
    {
        controller->disable_user_controls();
    }
    else
    {
        controller->enable_user_controls();
    }
}

//------------------------------------------------------------------------------
Server::player_id_t Server::player_count()
{
    return rpc_controller_manager->getNumberOfControllers();
}

//------------------------------------------------------------------------------
void Server::set_firing(player_id_t pid, bool firing)
{

}

//------------------------------------------------------------------------------
void Server::start_drifting(player_id_t pid, DriftDirection direction)
{
    assert( direction < DriftDirection::_COUNT );

    RPCController* controller = rpc_controller_manager->getController(pid);

    switch (direction)
    {
    case DriftDirection::LEFT:
        controller->set_skid_direction(KartControl::SC_LEFT);
        break;

    case DriftDirection::RIGHT:
        controller->set_skid_direction(KartControl::SC_RIGHT);
        break;

    default:
        assert( false );
    }
}

//------------------------------------------------------------------------------
void Server::stop_drifting(player_id_t pid)
{
    RPCController* controller = rpc_controller_manager->getController(pid);
    controller->set_skid_direction(KartControl::SC_NONE);
}

//------------------------------------------------------------------------------
void Server::use_nitrous(player_id_t pid, bool enable)
{
    RPCController* controller = rpc_controller_manager->getController(pid);

    if (enable)
    {
        controller->enable_nitrous();
    }
    else
    {
        controller->disable_nitrous();
    }
}


}
