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

import msgpackrpc
from msgpackrpc.error import TransportError

from .awaitable import future_call
from .stk_controller import STKController


class STKConnection:
    """ STK RPC channel context manager.

        Instances of this class should only really be used in an `async with`
        statement, e.g. `async with STKConnection() as controller:`.

        Entering the context manager yields a managed STKController instance.
        """

    hello_str = "this should be echoed back"

    @property
    def host(self):
        """ The network host that this connection is connected to. """
        return self.address.host

    @property
    def port(self):
        """ The network port that this connection is connected to. """
        return self.address.port

    def __init__(self, host: str = "127.0.0.1", port: int = 42069):
        """ Associates a network host and port with this connection. """
        self.address = msgpackrpc.Address(host, port)

    async def __aenter__(self) -> STKController:
        """ Magic method which connects to a running STK instance.

            If no STK instance is running (or the running instance has RPC
            disabled), this function will continue attempting to connect ad
            infinitum, until it eventually succeeds. """
        while True:
            try:
                self.client = \
                    msgpackrpc.Client(self.address, timeout=5, reconnect_limit=1)

                echo = await future_call(self.client, "hello", self.hello_str)

                if echo.decode("utf-8") == self.hello_str:
                    break
            except TransportError:
                pass

            await asyncio.sleep(1)

        return STKController(self)

    async def __aexit__(self, exc_type, exc, tb):
        """ Magic method which closes the socket connection to STK, if open. """
        self.client.close()
