//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2013-2015 SuperTuxKart-Team
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

/*! \file stk_host.hpp
 *  \brief Defines an interface to use network low-level functions easily.
 */
#ifndef STK_HOST_HPP
#define STK_HOST_HPP

#include "network/network.hpp"
#include "network/network_string.hpp"
#include "network/transport_address.hpp"

#include "irrString.h"

// enet.h includes win32.h, which without lean_and_mean includes
// winspool.h, which defines MAX_PRIORITY as a macro, which then
// results in request_manager.hpp not being compilable.
#define WIN32_LEAN_AND_MEAN
#include <enet/enet.h>

#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <tuple>

class GameSetup;
class NetworkPlayerProfile;
class Server;
class SeparateProcess;

enum ENetCommandType : unsigned int
{
    ECT_SEND_PACKET = 0,
    ECT_DISCONNECT = 1
};

class STKHost
{
public:
    /** \brief Defines three host types for the server.
    *  These values tells the host where he will accept connections from.
    */
    enum
    {
        HOST_ANY = 0,             //!< Any host.
        HOST_BROADCAST = 0xFFFFFFFF,    //!< Defines the broadcast address.
        PORT_ANY = 0              //!< Any port.
    };

private:
    /** Singleton pointer to the instance. */
    static STKHost* m_stk_host;

    /** Separate process of server instance. */
    SeparateProcess* m_separate_process;

    /** ENet host interfacing sockets. */
    Network* m_network;

    /** Network console thread */
    std::thread m_network_console;

    /** Make sure the removing or adding a peer is thread-safe. */
    std::mutex m_peers_mutex;

    /** Let (atm enet_peer_send and enet_peer_disconnect) run in the listening
     *  thread. */
    std::list<std::tuple<ENetPeer*/*peer receive*/,
        ENetPacket*/*packet to send*/, uint32_t/*integer data*/,
        ENetCommandType> > m_enet_cmd;

    /** Protect \ref m_enet_cmd from multiple threads usage. */
    std::mutex m_enet_cmd_mutex;

    /** The list of peers connected to this instance. */
    std::map<ENetPeer*, std::shared_ptr<STKPeer> > m_peers;

    /** Next unique host id. It is increased whenever a new peer is added (see
     *  getPeer()), but not decreased whena host (=peer) disconnects. This
     *  results in a unique host id for each host, even when a host should
     *  disconnect and then reconnect. */
    int m_next_unique_host_id;

    /** Host id of this host. */
    uint8_t m_host_id;

    /** Stores data about the online game to play. */
    GameSetup* m_game_setup;

    /** Id of thread listening to enet events. */
    std::thread m_listening_thread;

    /** Flag which is set from the protocol manager thread which
     *  triggers a shutdown of the STKHost (and the Protocolmanager). */
    std::atomic_bool m_shutdown;

    /** Use as a timeout to waiting a disconnect event when exiting. */
    std::atomic<double> m_exit_timeout;

    /** An error message, which is set by a protocol to be displayed
     *  in the GUI. */
    irr::core::stringw m_error_message;

    /** The public address found by stun (if WAN is used). */
    TransportAddress m_public_address;

    /** The public address stun server used. */
    TransportAddress m_stun_address;

    /** The private port enet socket is bound. */
    uint16_t m_private_port;

             STKHost(std::shared_ptr<Server> server);
             STKHost(const irr::core::stringw &server_name);
    virtual ~STKHost();
    void init();
    void handleDirectSocketRequest(Network* direct_socket);
    // ------------------------------------------------------------------------
    void mainLoop();

public:
    /** If a network console should be started. Note that the console can cause
    *  a crash in release mode on windows (see #1529). */
    static bool m_enable_console;


    /** Creates the STKHost. It takes all confifguration parameters from
     *  NetworkConfig. This STKHost can either be a client or a server.
     */
    static void create(std::shared_ptr<Server> server = nullptr,
                       SeparateProcess* p = NULL);

    // ------------------------------------------------------------------------
    /** Returns the instance of STKHost. */
    static STKHost *get()
    {
        assert(m_stk_host != NULL);
        return m_stk_host;
    }   // get
    // ------------------------------------------------------------------------
    static void destroy()
    {
        assert(m_stk_host != NULL);
        delete m_stk_host;
        m_stk_host = NULL;
    }   // destroy
    // ------------------------------------------------------------------------
    /** Checks if the STKHost has been created. */
    static bool existHost() { return m_stk_host != NULL; }
    // ------------------------------------------------------------------------
    const TransportAddress& getPublicAddress() const
                                                   { return m_public_address; }
    // ------------------------------------------------------------------------
    const TransportAddress& getStunAddress() const   { return m_stun_address; }
    // ------------------------------------------------------------------------
    uint16_t getPrivatePort() const                  { return m_private_port; }
    // ------------------------------------------------------------------------
    void setPrivatePort();
    // ------------------------------------------------------------------------
    void setPublicAddress();
    // ------------------------------------------------------------------------
    GameSetup* setupNewGame();
    // ------------------------------------------------------------------------
    void disconnectAllPeers(bool timeout_waiting = false);
    // ------------------------------------------------------------------------
    bool connect(const TransportAddress& peer);
    //-------------------------------------------------------------------------
    /** Requests that the network infrastructure is to be shut down. This
    *   function is called from a thread, but the actual shutdown needs to be
    *   done from the main thread to avoid race conditions (e.g.
    *   ProtocolManager might still access data structures when the main thread
    *   tests if STKHost exist (which it does, but ProtocolManager might be
    *   shut down already.
    */
    void requestShutdown()
    {
        m_shutdown.store(true);
    }   // requestExit
    //-------------------------------------------------------------------------
    void shutdown();
    //-------------------------------------------------------------------------
    void sendPacketToAllPeers(NetworkString *data, bool reliable = true);
    //-------------------------------------------------------------------------
    void sendPacketExcept(STKPeer* peer,
                          NetworkString *data,
                          bool reliable = true);
    void        setupClient(int peer_count, int channel_limit,
                            uint32_t max_incoming_bandwidth,
                            uint32_t max_outgoing_bandwidth);
    void        startListening();
    void        stopListening();
    bool        peerExists(const TransportAddress& peer_address);
    bool        isConnectedTo(const TransportAddress& peer_address);
    std::shared_ptr<STKPeer> getServerPeerForClient() const;
    std::vector<NetworkPlayerProfile*> getMyPlayerProfiles();
    void        setErrorMessage(const irr::core::stringw &message);
    bool        isAuthorisedToControl() const;
    //-------------------------------------------------------------------------
    void addEnetCommand(ENetPeer* peer, ENetPacket* packet, uint32_t i,
                        ENetCommandType ect)
    {
        std::lock_guard<std::mutex> lock(m_enet_cmd_mutex);
        m_enet_cmd.emplace_back(peer, packet, i, ect);
    }
    // ------------------------------------------------------------------------
    /** Returns the last error (or "" if no error has happened). */
    const irr::core::stringw& getErrorMessage() const
                                                    { return m_error_message; }
    // ------------------------------------------------------------------------
    /** Returns true if a shutdown of the network infrastructure was
     *  requested. */
    bool requestedShutdown() const                { return m_shutdown.load(); }
    // ------------------------------------------------------------------------
    /** Returns the current game setup. */
    GameSetup* getGameSetup()                          { return m_game_setup; }
    // ------------------------------------------------------------------------
    int receiveRawPacket(char *buffer, int buffer_len, 
                         TransportAddress* sender, int max_tries = -1)
    {
        return m_network->receiveRawPacket(buffer, buffer_len, sender,
                                           max_tries);
    }   // receiveRawPacket
    // ------------------------------------------------------------------------
    void sendRawPacket(const BareNetworkString &buffer,
                       const TransportAddress& dst)
    {
        m_network->sendRawPacket(buffer, dst);
    }  // sendRawPacket
    // ------------------------------------------------------------------------
    /** Returns a copied list of peers. */
    std::vector<std::shared_ptr<STKPeer> > getPeers()
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        std::vector<std::shared_ptr<STKPeer> > peers;
        for (auto p : m_peers)
        {
            peers.push_back(p.second);
        }
        return peers;
    }
    // ------------------------------------------------------------------------
    /** Returns the next (unique) host id. */
    unsigned int getNextHostId() const
    {
        assert(m_next_unique_host_id >= 0);
        return m_next_unique_host_id;
    }
    // ------------------------------------------------------------------------
    /** Returns the number of currently connected peers. */
    unsigned int getPeerCount()
    {
        std::lock_guard<std::mutex> lock(m_peers_mutex);
        return m_peers.size();
    }
    // ------------------------------------------------------------------------
    /** Sets the global host id of this host. */
    void setMyHostId(uint8_t my_host_id)            { m_host_id = my_host_id; }
    // ------------------------------------------------------------------------
    /** Returns the host id of this host. */
    uint8_t getMyHostId() const                           { return m_host_id; }
    // ------------------------------------------------------------------------
    void sendToServer(NetworkString *data, bool reliable = true);
    // ------------------------------------------------------------------------
    /** True if this is a client and server in graphics mode made by server
     *  creation screen. */
    bool isClientServer() const          { return m_separate_process != NULL; }

};   // class STKHost

#endif // STK_HOST_HPP
