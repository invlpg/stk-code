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

#include "karts/controller/rpc_controller.hpp"

#include "rpc/rpc_controller_manager.hpp"

RPCController::RPCController(AbstractKart* kart, int local_player_id)
    : LocalPlayerController(kart, local_player_id, HANDICAP_NONE)
    , m_user_controls_enabled(true)
{
    Log::info("RPCController", "Using RPC controller for this race");

    // Register the controller's existence, so RPC remotes know we exist
    assert( rpc::rpc_controller_manager != NULL );
    rpc::rpc_controller_manager->addController(*this);
}

//------------------------------------------------------------------------------
RPCController::~RPCController()
{
    // Unregister the controller's existence, so RPC remotes don't try to use us
    if (rpc::rpc_controller_manager != NULL)
    {
        rpc::rpc_controller_manager->removeController(*this);
    }
}

//------------------------------------------------------------------------------
/** Updates the player kart, called once each timestep.
 */
void RPCController::update(int ticks)
{
    LocalPlayerController::update(ticks);
}

//------------------------------------------------------------------------------
/** Interprets kart actions and sets corresponding values in the kart control
 *  data structure. We mostly defer to LocalPlayerController, so that a human
 *  can control the kart simultaneously, but we can use this to filter certain
 *  actions that we want to prevent the user from executing, by returning before
 *  calling LocalPlayerController::action.
 * \param action  The action to be executed.
 * \param value   If 32768, it indicates a digital value of 'fully set'
 *                if between 1 and 32767, it indicates an analog value,
 *                and if it's 0 it indicates that the corresponding button
 *                was released.
 * \param dry_run If set it will return if this action will trigger a
 *                state change or not.
 * \return        True if dry_run==true and a state change would be triggered.
 *                If dry_run==false, it returns true.
 * \see           LocalPlayerController::action
 */
bool RPCController::action(PlayerAction action, int value, bool dry_run)
{
    if (m_user_controls_enabled)
    {
        return LocalPlayerController::action(action, value, dry_run);
    }
    else
    {
        // Since the user's controls are disabled, a state change can never be
        // triggered
        return !dry_run;
    }
}

//------------------------------------------------------------------------------
void RPCController::disable_user_controls()
{
    m_user_controls_enabled = false;
}

//------------------------------------------------------------------------------
void RPCController::enable_user_controls()
{
    m_user_controls_enabled = true;
}
