# IPC client for controlling a SuperTuxKart player / observing its environment
# Copyright (C) 2020 Carl Albrecht <carl.albrecht@griffithuni.edu.au>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import asyncio
from typing import AsyncGenerator

from .awaitable import future_call
from .stk_player import STKPlayer


class STKController:
    """ Collection of RPC methods which operate on the STK game in general.

        These methods can be used to gather information on the current game
        state, as well as control the game. """

    def __init__(self, connection: "STKConnection"):
        """ Composes a "controller" atop the STK RPC socket, so that connection
            manipulation methods aren't as readily exposed to external code. """
        self.connection = connection

    async def frames(self) -> AsyncGenerator[int]:
        """ Generator which yields the latest game frame.

            This can be used as an input source for CV-based driving
            algorithms. """
        for i in range(10):
            yield i

    async def players(self) -> AsyncGenerator[STKPlayer]:
        """ Generator which yields all local players (controlled by the STK
            instance running on this computer) in the current game.

            The iterator waits for the player count to have not increased in a
            while before finalising, as PlayerController instances can take a
            while to materialise. """
        last_value = 0

        while await future_call(self.connection.client, "game_running"):
            value = await future_call(self.connection.client, "player_count")

            if value == last_value:
                return

            for i in range(last_value, value):
                yield STKPlayer(self.connection, self, i)

            await asyncio.sleep(0.05)
            last_value = value

    async def game_started(self) -> None:
        """ Sleeps until a game has started. This can be used to easily make a
            coroutine skip the user navigating the main menu, etc.

            i.e. await controller.game_started() returns as soon as the player
            has entered the game world. """
        while True:
            if await future_call(self.connection.client, "game_running"):
                return

            await asyncio.sleep(0.25)
