/**
 * Copyright (c) 2017, Damian Vicino
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

#ifndef CADMIUM_DEVSTONE_ATOMIC_HPP
#define CADMIUM_DEVSTONE_ATOMIC_HPP

#include<cadmium/modeling/ports.hpp>
#include<cadmium/modeling/message_bag.hpp>
#include<limits>

#include "../dhry/dhry_1.c"


/**
 * @brief PDEVStone Atomic Model.
 *
 * PDEVStone (InternalTime, ExternalTime, Period):
 * This model executes:
 * - a Dhrystone for InternalCycles on each Internal transition,
 * - a Dhrystone for ExternalCycles on each External transition,
 * - the time advance after each external transition is Period.
*/

//  an integer input and output port for the model
struct devstone_atomic_defs{
    //custom ports
    struct in : public cadmium::in_port<int> {};
    struct out : public cadmium::out_port<int> {};
};


template<typename TIME>
class devstone_atomic {
    using defs=devstone_atomic_defs;
public:
    // default constructor
    constexpr devstone_atomic() noexcept {
        //preparing the output bag, since we return always same message
        cadmium::get_messages<typename defs::out>(outbag).emplace_back(1);
    }

    constexpr devstone_atomic(int ext_cycles, int int_cycles, TIME time_advance) noexcept
        : period(time_advance), external_cycles(ext_cycles), internal_cycles(int_cycles){
        //preparing the output bag, since we return always same message
        cadmium::get_messages<typename defs::out>(outbag).emplace_back(1);
    }

    // state definition
    using queued_processes=int; //for readability
    using state_type=queued_processes;
    state_type state = 0;

    // ports definition
    using input_ports=std::tuple<typename defs::in>;
    using output_ports=std::tuple<typename defs::out>;

protected:
    /*
     * This model executes:
     * - a Dhrystone for InternalCycles on each Internal transition,
     * - a Dhrystone for ExternalCycles on each External transition,
     * - the time advance after each external transition is Period.
     * The following 3 variables need to be overriden by the inheriting model constructor.
     */
    TIME period=std::numeric_limits<float>::infinity();
    int external_cycles=-1;
    int internal_cycles=-1;
    using outbag_t=typename cadmium::make_message_bags<output_ports>::type;
    outbag_t outbag;

public:
    void internal_transition() {
        DhryStone().dhrystoneRun(internal_cycles);
        state--;
    }

    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        DhryStone().dhrystoneRun(external_cycles);
        state+= cadmium::get_messages<typename defs::in>(mbs).size();
    }

    void confluence_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        internal_transition();
        external_transition(e, mbs);
    }

    outbag_t output() const {
        return outbag;
    }

    TIME time_advance() const {
        return (state!=0?period:std::numeric_limits<TIME>::infinity());
    }
};

#endif // CADMIUM_DEVSTONE_ATOMIC_HPP
