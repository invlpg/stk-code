# STK Remote

IPC remote controller which can control a local SuperTuxKart player instead of a
human player.

This library allows users to control the player remotely, as well as observe the
player's environment programmatically. It is aimed at supporting the development
of simple intelligent agents, which can perceive and react to in-game events and
situations.

## Requirements

This package requires an IPC server embedded in the SuperTuxKart application in
order to function. [This repository](https://github.com/invlpg/stk-code)
contains a patched version of the source code which exposes required STK
functionality to clients via IPC.
