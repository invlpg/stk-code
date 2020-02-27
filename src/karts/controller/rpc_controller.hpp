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

#ifndef HEADER_RPC_CONTROLLER_HPP
#define HEADER_RPC_CONTROLLER_HPP

#include "karts/controller/local_player_controller.hpp"

class AbstractKart;

/** Special controller which can either be controlled by the player, or by
 *  remote applications over an RPC channel. RPC programs can disable the
 *  player's controls, so that it has exclusive control over the kart.
 *
 * \remarks Each RPCController instance is registered with [some sort of manager
 *          class], allowing multiple players to be controlled simultaneously
 *          via RPC.
 *
 * \ingroup controller
 */
class RPCController : public LocalPlayerController
{
private:
    bool         m_user_controls_enabled;

public:
                 RPCController(AbstractKart *kart, int local_player_id);
    virtual     ~RPCController();

    //-------------------------------------------------------------------------
    // Event callbacks
            void update         (int ticks) OVERRIDE;
            bool action         (PlayerAction action, int value,
                                 bool dry_run=false) OVERRIDE;
};

#endif // HEADER_RPC_CONTROLLER_HPP