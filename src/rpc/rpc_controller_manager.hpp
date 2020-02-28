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

#ifndef HEADER_RPC_CONTROLLER_MANAGER_HPP
#define HEADER_RPC_CONTROLLER_MANAGER_HPP

#include "karts/controller/rpc_controller.hpp"
#include "utils/ptr_vector.hpp"

class RPCControllerManager
{
    PtrVector<RPCController, REF> m_controllers;

public:
                           RPCControllerManager();

    //--------------------------------------------------------------------------
    // Controller collection methods
            void           addController         (RPCController& controller);
    virtual void           removeController      (RPCController& controller);
            RPCController* getController         (int i);
            unsigned int   getNumberOfControllers() const;
};

extern RPCControllerManager* rpc_controller_manager;

#endif // HEADER_RPC_CONTROLLER_MANAGER_HPP
