/**
 * Copyright (c) 2019, Juan Lanuza
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

#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>

#include <boost/program_options.hpp>

#include <cadmium/engine/pdevs_dynamic_runner.hpp>

#include "dynamic/LI_generator.cpp"
#include "dynamic/HI_generator.cpp"
#include "dynamic/HO_generator.cpp"
#include "dynamic/HOmod_generator.cpp"

namespace po=boost::program_options;
using hclock=std::chrono::high_resolution_clock;
using Time=float;

int main(int argc, char* argv[]){

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("kind", po::value<std::string>()->required(), "set kind of devstone: LI, HI, HO or HOmod")
            ("width", po::value<int>()->required(), "set width of the DEVStone: integer value")
            ("depth", po::value<int>()->required(), "set depth of the DEVStone: integer value")
            ("int-cycles", po::value<int>()->required(), "set the Dhrystone cycles to expend in internal transtions: integer value")
            ("ext-cycles", po::value<int>()->required(), "set the Dhrystone cycles to expend in external transtions: integer value")
            ("time-advance", po::value<int>()->default_value(0), "set the time expend in external transtions by the Dhrystone in miliseconds: integer value")
            #ifdef CADMIUM_EXECUTE_CONCURRENT
            ("threads", po::value<int>()->required(), "amount of threads to use")
            #endif //CADMIUM_EXECUTE_CONCURRENT
            ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch ( boost::program_options::required_option be ){
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 0;
        } else {
            std::cout << be.what() << std::endl;
            std::cout << std::endl;
            std::cout << "for mode information run: " << argv[0] << " --help" << std::endl;
            return 1;
        }
    }
    std::string kind = vm["kind"].as<std::string>();
    if (kind.compare("LI") != 0  && kind.compare("HI") != 0 &&
        kind.compare("HO") != 0 && kind.compare("HOmod") != 0) {
        std::cout << "The kind needs to be LI, HI, HO or HOmod and received value was: " << kind << std::endl;
        std::cout << "for mode information run: " << argv[0] << " --help" << std::endl;
        return 1;
    }

    int width = vm["width"].as<int>();
    int depth = vm["depth"].as<int>();
    int int_cycles = vm["int-cycles"].as<int>();
    int ext_cycles = vm["ext-cycles"].as<int>();
    int time_advance = vm["time-advance"].as<int>();
    #ifdef CADMIUM_EXECUTE_CONCURRENT
    int threads = vm["threads"].as<int>();
    #endif //CADMIUM_EXECUTE_CONCURRENT
    //finished processing input

    auto processed_parameters = hclock::now();

    std::shared_ptr<cadmium::dynamic::modeling::coupled<Time>> TOP_coupled;
    if (kind.compare("LI") == 0){
        TOP_coupled = create_LI_model(width,depth, ext_cycles, int_cycles, time_advance);
    } else if (kind.compare("HI") == 0) {
        TOP_coupled = create_HI_model(width, depth, ext_cycles, int_cycles, time_advance);
    } else if (kind.compare("HO") == 0) {
        TOP_coupled = create_HO_model(width,depth, ext_cycles, int_cycles, time_advance);
    } else if (kind.compare("HOmod") == 0) {
        TOP_coupled = create_HOmod_model(width,depth, ext_cycles, int_cycles, time_advance);
    } else {
        abort();
    }

    auto model_init = hclock::now();

    std::cout << "Model creation time: " << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>( model_init - processed_parameters).count() << " seconds" << std::endl;

    #ifdef CADMIUM_EXECUTE_CONCURRENT
        cadmium::dynamic::engine::runner<Time, cadmium::logger::not_logger> r(TOP_coupled, 0.0, threads);
    #else
        cadmium::dynamic::engine::runner<TIME, cadmium::logger::not_logger> r(TOP_coupled, 0.0);
    #endif //CADMIUM_EXECUTE_CONCURRENT

    auto runner_init = hclock::now();

    std::cout << "Engine setup time: " << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(runner_init - model_init).count() << " seconds" << std::endl;

    r.run_until(1000000);

    auto finished_simulation = hclock::now();

    std::cout << "Simulation time: " << std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(finished_simulation - runner_init).count() << " seconds" << std::endl;
}
