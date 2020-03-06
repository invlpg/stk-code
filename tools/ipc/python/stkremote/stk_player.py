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
import enum


class DriftDirection(enum.Enum):
    LEFT = 0
    RIGHT = 1


class STKPlayer:
    """ Collection of RPC methods which operate on an individual player / kart.

        Instances of this class should be obtained by iterating over
        STKController.players(). """

    def __init__(
            self,
            connection: "STKConnection", controller: "STKController",
            pid: int):
        """ Uses object composition to associate a connection and controller with
            a game player id. """
        self.connection = connection
        self.controller = controller
        self.pid = pid

    def disable_controls(self, disable: bool) -> None:
        """ Disables user input, so that this program has exclusive control over
            the kart.

            Leaving user input enabled when programmatic kart control is desired
            usually leads to the programmed commands being ignored. However, it
            may be desirable to leave user controls enabled (and not control the
            kart at all programmatically), and just monitor what the user does,
            in order to e.g. gather training data for a neural network.

            :param disable: whether or not to disable user controls
            :returns: nothing """
        self.connection.client.notify(
            "disable_player_controls", self.pid, disable)

    async def fire(self) -> None:
        """ Automatically sets, then clears the fire key, in order to simplify
            using the current item. Does nothing if the player doesn't currently
            have an item to use.

            Calling code should not await this method.

            :returns: nothing """
        self.set_firing(True)
        await asyncio.sleep(0.1)
        self.set_firing(False)

    def use_nitrous(self, enable: bool) -> None:
        """ Sets whether or not to use nitrous in order to increase speed.
            Setting this to True is equivalent to holding down the nitrous
            key on the keyboard whilst playing - nitrous will be used as soon
            as it is collected.

            :param enable: whether or not to use nitrous
            :returns: nothing """
        self.connection.client.notify("use_nitrous", self.pid, enable)

    def set_firing(self, fire: bool) -> None:
        """ Sets whether or not fire is activated. Setting this to True is
            equivalent to holding down the fire key on the keyboard whilst
            playing - picked up items will be used the moment they are obtained.

            :param fire: whether or not to "hold down" the fire key
            :returns: nothing """
        self.connection.client.notify("set_firing", self.pid, fire)

    def start_drifting(self, direction: DriftDirection):
        """ Sets the direction that the player's kart should start drifting in,
            then initiates a continuous drift, which stops either when the
            player hits an obstacle / wall etc, or stop_drifting() is called.

            Once a drift is initiated, the steering angle affects how tight the
            drift is. Note that the kart's steering direction cannot be changed
            (e.g. from turning / drifting left to turning right) without
            stopping the current drift, or by changing the drifting direction by
            calling this method again with the opposite drift direction.

            :param direction: the direction to drift in
            :returns: nothing """
        self.connection.client.notify("start_drifting", self.pid, direction)

    def stop_drifting(self):
        """ Stops the kart from drifting, if a drift was previously initiated by
            calling start_drifting().

            Does nothing if the kart is not drifting.

            :returns: nothing """
        self.connection.client.notify("stop_drifting", self.pid)
