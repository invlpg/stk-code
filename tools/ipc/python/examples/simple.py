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

# This example program demonstrates the flow of connecting to a running STK
# instance, then sending some simple player commands to the running game.


from stkremote import STKConnection, DriftDirection

import asyncio


async def player_ai(controller, player):
    print("Player %d added" % player.pid)

    player.disable_controls(True)
    player.start_drifting(DriftDirection.LEFT)
    player.use_nitrous(True)

    async for frame in controller.frames():
        # We don't actually get given real frame data yet lol
        break


async def main():
    # Connect to the game. If the game is not yet running, further execution of
    # this coroutine will be prevented until the game is launched
    async with STKConnection() as controller:
        print("Connected to STK instance")

        # Wait for the game to start (we can't do much whilst in the menu)
        await controller.game_started()
        print("Game started")

        # Start an AI controller for each local player - works with split-screen
        # multiplayer etc.
        await asyncio.wait([
            player_ai(controller, player)
            async for player in controller.players()
        ])


# Start async main from a synchronous context
if __name__ == "__main__":
    loop = asyncio.new_event_loop()
    asyncio.set_event_loop(loop)
    loop.run_until_complete(main())
