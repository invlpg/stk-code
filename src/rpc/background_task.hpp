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

#ifndef HEADER_RPC_BACKGROUND_TASK_HPP
#define HEADER_RPC_BACKGROUND_TASK_HPP

#include <thread>
#include <memory>
#include <future>

#include <utils/no_copy.hpp>

namespace rpc {


/** CRTP wrapper class, which adds simple functionality to a class for starting
 *  and stopping a separate thread to run its code in.
 *
 * \remarks Subclasses should avoid exposing public constructors, as external
 *          code needs to call `start()` in order to create new threads.
 *
 *          Subclasses should expose a protected no-args constructor, as well as
 *          a protected `run()` method.
 *
 * \tparam T No-args constructable class with a `void run()` method.
 *
 * \see rpc::Server
 */
template <typename T>
class BackgroundTask : public NoCopy {
private:
    std::thread m_thread;

    //--------------------------------------------------------------------------
    // Cancellation signal source - subclasses should wait for
    // `cancellation_token` to be ready at some point in their execution.
private:
    bool cancelled = false;
    std::promise<void> cancellation_signal;
protected:
    std::future<void> cancellation_token;

    //--------------------------------------------------------------------------
    BackgroundTask()
        : cancellation_signal()
        , cancellation_token(cancellation_signal.get_future())
    { }

    //--------------------------------------------------------------------------
public:
    using BackgroundTaskCRTP = BackgroundTask;

    //--------------------------------------------------------------------------
    ~BackgroundTask()
    {
        stop(true);
    }

    //--------------------------------------------------------------------------
    static std::unique_ptr<T> start()
    {
        // `new` required in order to access T's presumably non-public ctor
        std::unique_ptr<T> task(new T());
        T* task_ptr = task.get();

        task->m_thread = std::thread([task_ptr] {
            assert( task_ptr != NULL );
            task_ptr->run();
        });

        return std::move(task);
    }

    //--------------------------------------------------------------------------
    void stop(bool join=false) noexcept
    {
        if (!cancelled)
        {
            cancelled = true;
            cancellation_signal.set_value();
        }

        if (join && m_thread.joinable()) {
            m_thread.join();
        }
    }
};


}

#endif // HEADER_RPC_BACKGROUND_TASK_HPP
