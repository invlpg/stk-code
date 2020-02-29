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


class STKPlayer:
    def __init__(
            self,
            connection: "STKConnection", controller: "STKController",
            pid: int):
        self.connection = connection
        self.controller = controller
        self.pid = pid

    def disable_controls(self, disable: bool):
        self.connection.client.notify(
            "disable_player_controls", self.pid, disable)

    def use_nitrous(self, enable: bool):
        self.connection.client.notify("use_nitrous", self.pid, enable)
