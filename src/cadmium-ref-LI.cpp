
//THIS IS A REF FOR AN AUTOGENERATED MODEL
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

#include <chrono>
#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/engine/pdevs_runner.hpp>
#include "cadmium-devstone-atomic.hpp"
#include "cadmium-event-reader.hpp"

// Ports for coupled models, we use the same in every level
struct coupled_in_port : public cadmium::in_port<int>{};
struct coupled_out_port : public cadmium::out_port<int>{};
using coupled_in_ports = std::tuple<coupled_in_port>;
using coupled_out_ports = std::tuple<coupled_out_port>;

//Devstones have no Internal coupling
using ics=std::tuple<>;

//A configured version of the devstone atomic, we use same configuration in every atomic.
template<typename TIME>
struct configured_atomic_devstone : devstone_atomic<TIME>{
    configured_atomic_devstone(){
        devstone_atomic<TIME>::period = 1;
        devstone_atomic<TIME>::external_cycles = 100;
        devstone_atomic<TIME>::internal_cycles = 100;
    }
};

//This model is LI devstone W=3, D=3
//Level 0 has always a single model
template<typename TIME>
struct devstone_atomic_L0_0 : configured_atomic_devstone<TIME>{};

//Level 1
//atomics
template<typename TIME>
struct devstone_atomic_L1_0 : configured_atomic_devstone<TIME>{};
template<typename TIME>
struct devstone_atomic_L1_1 : configured_atomic_devstone<TIME>{};
template<typename TIME>
struct devstone_atomic_L1_2 : configured_atomic_devstone<TIME>{};
//coupled
using L1_submodels=cadmium::modeling::models_tuple<devstone_atomic_L0_0>;
using L1_eics=std::tuple<
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L0_0, devstone_atomic_defs::in>
>;
using L1_eocs=std::tuple<
    cadmium::modeling::EOC<devstone_atomic_L0_0, devstone_atomic_defs::out, coupled_out_port>
>;
template<typename TIME>
using L1_coupled=cadmium::modeling::coupled_model<TIME, coupled_in_ports, coupled_out_ports, L1_submodels, L1_eics, L1_eocs, ics>;

//Level 2
//atomics
template<typename TIME>
struct devstone_atomic_L2_0 : configured_atomic_devstone<TIME>{};
template<typename TIME>
struct devstone_atomic_L2_1 : configured_atomic_devstone<TIME>{};
template<typename TIME>
struct devstone_atomic_L2_2 : configured_atomic_devstone<TIME>{};
//coupled
using L2_submodels=cadmium::modeling::models_tuple<devstone_atomic_L1_0, devstone_atomic_L1_1, devstone_atomic_L1_2, L1_coupled>;
using L2_eics=std::tuple<
    cadmium::modeling::EIC<coupled_in_port, L1_coupled, coupled_in_port>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L1_0, devstone_atomic_defs::in>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L1_1, devstone_atomic_defs::in>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L1_2, devstone_atomic_defs::in>
>;
using L2_eocs=std::tuple<
    cadmium::modeling::EOC<L1_coupled,coupled_out_port, coupled_out_port>
>;
template<typename TIME>
using L2_coupled=cadmium::modeling::coupled_model<TIME, coupled_in_ports, coupled_out_ports, L2_submodels, L2_eics, L2_eocs, ics>;

//Level 3 has no atomics because it is the last level
//coupled
using L3_submodels=cadmium::modeling::models_tuple<devstone_atomic_L2_0, devstone_atomic_L2_1, devstone_atomic_L2_2, L2_coupled>;
using L3_eics=std::tuple<
    cadmium::modeling::EIC<coupled_in_port, L2_coupled, coupled_in_port>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L2_0, devstone_atomic_defs::in>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L2_1, devstone_atomic_defs::in>,
    cadmium::modeling::EIC<coupled_in_port, devstone_atomic_L2_2, devstone_atomic_defs::in>
>;
using L3_eocs=std::tuple<
    cadmium::modeling::EOC<L2_coupled,coupled_out_port, coupled_out_port>
>;
template<typename TIME>
using L3_coupled=cadmium::modeling::coupled_model<TIME, coupled_in_ports, coupled_out_ports, L3_submodels, L3_eics, L3_eocs, ics>;

//TOP model conecting a generator of events to the input
using TOP_coupled_in_ports=std::tuple<>;
using TOP_coupled_out_ports=std::tuple<>;
using TOP_submodels=cadmium::modeling::models_tuple<devstone_event_reader, L3_coupled>;
using TOP_eics=std::tuple<>;
using TOP_eocs=std::tuple<>;
using TOP_ics=std::tuple<
cadmium::modeling::IC<devstone_event_reader, devstone_event_reader_defs::out, L3_coupled, coupled_in_port>
>;
template<typename TIME>
using TOP_coupled=cadmium::modeling::coupled_model<TIME, TOP_coupled_in_ports, TOP_coupled_out_ports, TOP_submodels, TOP_eics, TOP_eocs, TOP_ics>;

using hclock=std::chrono::high_resolution_clock; //for measuring execution time

int main(){
    auto start = hclock::now(); //to measure simulation execution time

    cadmium::engine::runner<float, TOP_coupled, cadmium::logger::not_logger> r{0.0};

    r.run_until_passivate();

    auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>
    (hclock::now() - start).count();
    std::cout << "Simulation took:" << elapsed << "sec" << std::endl;
    return 0;
}
