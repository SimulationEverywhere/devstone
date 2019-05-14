/**
 * Copyright (c) 2013-2014, Carleton University, Universite de Nice-Sophia Antipolis
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef P_DEVSTONE_ATOMIC_H
#define P_DEVSTONE_ATOMIC_H

#include <boost/simulation/pdevs/atomic.hpp>
#include "../dhry/dhry_1.c"

namespace cdpp {
/**
 * @brief PDEVStone Atomic Model.
 *
 * PDEVStone (InternalTime, ExternalTime, Period):
 * This model executes:
 * - a Dhrystone for InternalCycles on each Internal transition,
 * - a Dhrystone for ExternalCycles on each External transition,
 * - the time advance after each external transition is Period.
*/
template<class TIME, class MSG>
class PDEVStoneAtomic : public boost::simulation::pdevs::atomic<TIME, MSG>
{
    std::vector<MSG> _out;
    int _internal_cycles;
    int _external_cycles;
    TIME _period;
    int _queued_processes;
public:
    /**
     * @brief DEVStoneAtomic constructor.
     *
     * @param internal_cycles the cycles dhrystone will be run in internal transitions.
     * @param external_cycles the cycles dhrystone will be run in external transitions.
     * @param period the time used for all time_advances.
     */
    explicit PDEVStoneAtomic(int internal_cycles, int external_cycles,  TIME period)
        : _internal_cycles(internal_cycles), _external_cycles(external_cycles), _period(period), _queued_processes(0)
    {
        _out.push_back(1);
    }
    /**
     * @brief internal function.
     */
    void internal() noexcept {
        DhryStone().dhrystoneRun(_internal_cycles);
        _queued_processes--;
    }
    /**
     * @brief advance function.
     * @return Time until next internal event.
     */
    TIME advance() const noexcept {
        return (_queued_processes?_period: boost::simulation::pdevs::atomic<TIME, MSG>::infinity);
    }
    /**
     * @brief out function.
     * @return Message defined in contruction.
     */
    std::vector<MSG> out() const noexcept{
        return _out;
    }
    /**
     * @brief external function domain is empty, so it throws.
     * @param msg external input message.
     * @param t time the external input is received.
     */
    void external(const std::vector<MSG>& msg, const TIME& t) noexcept {
        DhryStone().dhrystoneRun(_external_cycles);
        _queued_processes+=msg.size();
    }
    /**
     * @brief confluence function as defined in PDEVS
     * Message interpretation (with cdpp::message_cast) is confluence function responsability.
     * @param mb is a bag of messages coming from outside
     * @param t is the time the message is received
     */
    void confluence(const std::vector<MSG>& mb, const TIME& t) noexcept{
        internal();
        external(mb, TIME(0));
    }

};

}

#endif // P_DEVSTONE_ATOMIC_H
