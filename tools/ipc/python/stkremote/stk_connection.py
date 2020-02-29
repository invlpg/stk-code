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
    hello_str = "this should be echoed back"

    @property
    def host(self):
        return self.address.host

    @property
    def port(self):
        return self.address.port

    def __init__(self, host: str = "127.0.0.1", port: int = 42069):
        self.address = msgpackrpc.Address(host, port)

    async def __aenter__(self) -> STKController:
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
        self.client.close()
