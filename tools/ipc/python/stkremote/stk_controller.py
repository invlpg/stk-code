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

from .awaitable import future_call
from .stk_player import STKPlayer


class STKController:
    def __init__(self, connection: "STKConnection"):
        self.connection = connection

    async def frames(self):
        for i in range(10):
            yield i

    async def players(self):
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
        while True:
            if await future_call(self.connection.client, "game_running"):
                return

            await asyncio.sleep(0.25)
