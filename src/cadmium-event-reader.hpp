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


#ifndef CADMIUM_EVENT_READER_HPP
#define CADMIUM_EVENT_READER_HPP

#include<cadmium/modeling/ports.hpp>
#include<cadmium/modeling/message_bag.hpp>
#include<limits>
#include<fstream>
#include <cassert>

/**
 * Events are read from "events.txt", first column is absolute time the event has to be sent,
 * the second column tells the integer to sent in the "out" port
 */

//  an integer output port for the model
struct devstone_event_reader_defs{
    //custom ports
    struct in : public cadmium::in_port<int> {};
    struct out : public cadmium::out_port<int> {};
};


template<typename TIME>
class devstone_event_reader {
public:
    using defs=devstone_event_reader_defs;

    // state definition is ignored, the state is hold out of the system
    using state_type=int;
    state_type state = 0;
    
    // ports definition
    using input_ports=std::tuple<>;
    using output_ports=std::tuple<typename defs::out>;
    using outbag_t=typename cadmium::make_message_bags<output_ports>::type;
    outbag_t outbag;

    std::ifstream is; //the stream
    TIME last;
    TIME next;
    int prefetched_message;
    
    // default constructor opens the stream and sets initial time
    constexpr devstone_event_reader() {
        last = 0;
        is.open("events.txt");
        if (!is.good()) throw std::runtime_error("failed to open events file: events.txt");
        is >> next;
        if (is.eof()){
             next = std::numeric_limits<float>::infinity();
        } else {
            is >> prefetched_message;
        }
    }
    
    void internal_transition() {
        last = next;
        fetchUntilTimeAdvances();
    }
    
    void external_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        assert(false && "Non external input is expected in this model");
    }
    
    void confluence_transition(TIME e, typename cadmium::make_message_bags<input_ports>::type mbs) {
        assert(false && "Non external input is expected in this model");
    }
    
    outbag_t output() const {
        return outbag;
    }
    
    TIME time_advance() const {
        return (next < std::numeric_limits<float>::infinity()?next-last:next);
    }
    
    
private:
    //helper function
    void fetchUntilTimeAdvances() {
        //making use of the prefetched values
        cadmium::get_messages<typename defs::out>(outbag) = {prefetched_message};
        TIME t;
        int m;
        //fetching next messages
        is >> t;
        if (is.eof()){
            next = std::numeric_limits<float>::infinity();
            return;
        }
        while (!is.eof() && t == next){
            is >> m;
            cadmium::get_messages<typename defs::out>(outbag).push_back(m);
            is >> t;
        }
        if (is.eof()){
            next = std::numeric_limits<float>::infinity();
            return;
        } else {
            if (next < t) {
                next = t;
                //cache the last message fetched
                is >> prefetched_message;
            } else {
                throw std::runtime_error("next is before than now");
            }
        }
    }

};



#endif // CADMIUM_EVENT_READER_HPP
