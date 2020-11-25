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

#include <chrono>
#include <boost/format.hpp>

#include "../cadmium-devstone-atomic.hpp"
#include "../cadmium-event-reader.hpp"

//#include <cadmium/modeling/coupled_model.hpp>
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/concept/coupled_model_assert.hpp>
#include <cadmium/modeling/dynamic_coupled.hpp>
#include <cadmium/modeling/dynamic_atomic.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/tuple_to_ostream.hpp>
#include <cadmium/logger/common_loggers.hpp>

using TIME = float;

// Ports for coupled models, we use the same in every level
struct coupledHOmod_in_port1 : public cadmium::in_port<int>{};
struct coupledHOmod_in_port2 : public cadmium::in_port<int>{};
struct coupledHOmod_out_port : public cadmium::out_port<int>{};

using ModelMatrix = std::vector<std::vector<std::shared_ptr<cadmium::dynamic::modeling::model>>>;

std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> create_HOmod_model(
        uint width, uint depth, int ext_cycles, int int_cycles, TIME time_advance) {
    // Creates the HOmod model with the passed parameters
    // Returns a shared_ptr to the TOP model

    auto make_atomic_devstone = [&ext_cycles, &int_cycles, &time_advance](std::string model_id) -> std::shared_ptr<cadmium::dynamic::modeling::model> {
        return cadmium::dynamic::translate::make_dynamic_atomic_model<devstone_atomic, TIME>(model_id, ext_cycles, int_cycles, time_advance);
    };
    //Level 0 has always a single model
    std::shared_ptr<cadmium::dynamic::modeling::model> devstone_atomic_L0_0 = make_atomic_devstone("devstone_atomic_L0_0");
    std::unordered_map<int, ModelMatrix> atomics_by_level;
    std::unordered_map<int, std::shared_ptr<cadmium::dynamic::modeling::model>> coupleds_by_level;

    cadmium::dynamic::modeling::Ports coupled_in_ports = {typeid(coupledHOmod_in_port1), typeid(coupledHOmod_in_port2)};
    cadmium::dynamic::modeling::Ports coupled_out_ports = {typeid(coupledHOmod_out_port)};

    for (int level=1; level <= depth; level++) {

        //atomics
        ModelMatrix atomics_current_level;
        if (level < depth) {
            //Last level does not have atomics
            for(int idx_column=0; idx_column < width-1; idx_column++) {
                cadmium::dynamic::modeling::Models models_col;
                for(int idx_row=0; idx_row < idx_column + 2; idx_row++) {
                    std::string atomic_name = "devstone_atomic_L" + std::to_string(level) + "_" + std::to_string(idx_column) + ";" + std::to_string(idx_row);
                    models_col.push_back(make_atomic_devstone(atomic_name));
                }
                atomics_current_level.push_back(models_col);
            }
        }
        atomics_by_level[level] = atomics_current_level;

        //coupled
        cadmium::dynamic::modeling::Models Lcoupled_submodels;
        cadmium::dynamic::modeling::EICs Lcoupled_eics;
        cadmium::dynamic::modeling::EOCs Lcoupled_eocs;
        cadmium::dynamic::modeling::ICs Lcoupled_ics = {};

        if (level == 1) {
            Lcoupled_submodels= {devstone_atomic_L0_0};
            Lcoupled_eics = {
                cadmium::dynamic::translate::make_EIC<coupledHOmod_in_port1, devstone_atomic_defs::in>("devstone_atomic_L0_0")
            };
            Lcoupled_eocs = {
              cadmium::dynamic::translate::make_EOC<devstone_atomic_defs::out,coupledHOmod_out_port>("devstone_atomic_L0_0")
            };
        } else {
            std::shared_ptr<cadmium::dynamic::modeling::model> coupled_prev_level = coupleds_by_level[level - 1];

            Lcoupled_submodels = { coupled_prev_level };

            Lcoupled_eics = {
                cadmium::dynamic::translate::make_EIC<coupledHOmod_in_port1, coupledHOmod_in_port1>(
                    coupled_prev_level.get()->get_id()
                )
            };

            Lcoupled_eocs = {
                cadmium::dynamic::translate::make_EOC<coupledHOmod_out_port, coupledHOmod_out_port>(
                    coupled_prev_level.get()->get_id()
                )
            };

            ModelMatrix atomics_prev_level = atomics_by_level[level-1];
            for(int idx_column=0; idx_column < width-1; idx_column++) {
                cadmium::dynamic::modeling::Models models_col;
                for(int idx_row=0; idx_row < idx_column + 2; idx_row++) {
                    auto atomic = atomics_prev_level[idx_column][idx_row];
                    Lcoupled_submodels.push_back(atomic);

                    if(idx_row == 0 || idx_row == idx_column + 1) { //only first and last row
                        Lcoupled_eics.push_back(
                                cadmium::dynamic::translate::make_EIC<coupledHOmod_in_port2, devstone_atomic_defs::in>(atomic.get()->get_id())
                        );
                    }

                    if(idx_row == 0) {
                        Lcoupled_ics.push_back(
                                cadmium::dynamic::translate::make_IC<devstone_atomic_defs::out, coupledHOmod_in_port2>(atomic.get()->get_id(), coupled_prev_level->get_id())
                        );
                    } else {
                        auto prev_atomic = atomics_prev_level[idx_column][idx_row-1];
                        Lcoupled_ics.push_back(
                                cadmium::dynamic::translate::make_IC<devstone_atomic_defs::out, devstone_atomic_defs::in>(atomic.get()->get_id(), prev_atomic.get()->get_id())
                        );
                    }
                }
            }
        }
        std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> L_coupled = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
             "L" + std::to_string(level) + "_coupled",
             Lcoupled_submodels,
             coupled_in_ports,
             coupled_out_ports,
             Lcoupled_eics,
             Lcoupled_eocs,
             Lcoupled_ics
        );
        coupleds_by_level[level] = L_coupled;
    }

    //Create instance of devstone_event_reader
    std::shared_ptr<cadmium::dynamic::modeling::model> devstone_event_reader1 = cadmium::dynamic::translate::make_dynamic_atomic_model<devstone_event_reader, TIME>("devstone_event_reader1");

    std::shared_ptr<cadmium::dynamic::modeling::model> last_level_coupled = coupleds_by_level[depth];

    //TOP model conecting a generator of events to the input
    cadmium::dynamic::modeling::Ports TOP_coupled_in_ports = {};
    cadmium::dynamic::modeling::Ports TOP_coupled_out_ports = {};
    cadmium::dynamic::modeling::Models TOP_submodels = {devstone_event_reader1, last_level_coupled};
    cadmium::dynamic::modeling::EICs TOP_eics = {};
    cadmium::dynamic::modeling::EOCs TOP_eocs = {};
    cadmium::dynamic::modeling::ICs TOP_ics = {
        cadmium::dynamic::translate::make_IC<devstone_event_reader_defs::out,coupledHOmod_in_port1>("devstone_event_reader1",last_level_coupled.get()->get_id()),
        cadmium::dynamic::translate::make_IC<devstone_event_reader_defs::out,coupledHOmod_in_port2>("devstone_event_reader1",last_level_coupled.get()->get_id())
    };
    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP_coupled = std::make_shared<cadmium::dynamic::modeling::coupled<TIME>>(
     "TOP_coupled",
     TOP_submodels,
     TOP_coupled_in_ports,
     TOP_coupled_out_ports,
     TOP_eics,
     TOP_eocs,
     TOP_ics
    );

    return TOP_coupled;
}
