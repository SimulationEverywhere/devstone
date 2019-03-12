/**
 * Copyright (c) 2013-2016, Damian Vicino
 * Carleton University, Universite de Nice-Sophia Antipolis
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


#include <boost/test/unit_test.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "test_helpers.cpp"
#include "../src/dynamic/LI_generator.cpp"
#include "../../DEVSDiagrammer/model_json_exporter/include/dynamic_json_exporter.hpp"

namespace pt = boost::property_tree;

struct LI_33_as_json {
    LI_33_as_json() : TOP_coupled(create_LI_model(3,3)) {
        std::stringstream modeljson;
        dynamic_export_model_to_json(modeljson, TOP_coupled);
        pt::read_json(modeljson, tree);
    }

    std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP_coupled;
    pt::ptree tree;
};


BOOST_FIXTURE_TEST_SUITE( cadmium_dynamic_LI_test_suite, LI_33_as_json )

BOOST_AUTO_TEST_CASE( top_level_has_two_models_test ){
    auto TOP_submodels = tree.get_child("models");
    BOOST_CHECK(TOP_submodels.size() == 2);
    for (auto model: TOP_submodels) {
        BOOST_CHECK_EQUAL(model.first, ""); // Its an array
        BOOST_CHECK(model.second.get<std::string>("id") == "devstone_event_reader1" ||
                    model.second.get<std::string>("id") == "L3_coupled");
    }
}

BOOST_AUTO_TEST_CASE ( top_level_has_an_IC_test ){
    BOOST_CHECK_EQUAL(tree.get_child("ic").size(), 1);
    BOOST_CHECK_EQUAL(tree.get<std::string>("ic..from_model"), "devstone_event_reader1");
    BOOST_CHECK_EQUAL(tree.get<std::string>("ic..to_model"), "L3_coupled");
}

BOOST_AUTO_TEST_CASE ( coupled_models_have_W_submodels ) {
    auto TOP_submodels = tree.get_child("models");
    auto it_coupled = std::find_if(TOP_submodels.begin(), TOP_submodels.end(), iscoupled);
    BOOST_REQUIRE(it_coupled != TOP_submodels.end()); //a coupled model was found
    BOOST_CHECK_EQUAL(it_coupled->second.get_child("models").size(), 3);
}

BOOST_AUTO_TEST_CASE ( coupled_models_have_one_input_and_one_output ) {
    auto TOP_submodels = tree.get_child("models");
    auto it_coupled = std::find_if(TOP_submodels.begin(), TOP_submodels.end(), iscoupled);
    BOOST_REQUIRE(it_coupled != TOP_submodels.end()); //a coupled model was found

    BOOST_CHECK_EQUAL(it_coupled->second.get_child("ports.out.").size(), 1);
    BOOST_CHECK_EQUAL(it_coupled->second.get_child("ports.out..port_kind").data(), "out");
    BOOST_CHECK_EQUAL(it_coupled->second.get_child("ports.in.").size(), 1);
    BOOST_CHECK_EQUAL(it_coupled->second.get_child("ports.in..port_kind").data(), "in");
}


BOOST_AUTO_TEST_CASE ( coupled_models_have_input_connected_to_all_submodels ) {
    auto TOP_submodels = tree.get_child("models");
    auto it_coupled = std::find_if(TOP_submodels.begin(), TOP_submodels.end(), iscoupled);
    BOOST_REQUIRE(it_coupled != TOP_submodels.end()); //a coupled model was found

    auto submodels = it_coupled->second.get_child("models");
    auto eics = it_coupled->second.get_child("eic");

    std::vector<std::string> submodel_names;
    for (auto submodel : submodels) {
        submodel_names.push_back(submodel.second.get<std::string>("id"));
    }

    std::string coupled_inport_name = it_coupled->second.get_child("ports.in..name").data();

    BOOST_CHECK_EQUAL(submodels.size(), eics.size());

    for (auto eic : eics) {
        BOOST_CHECK_EQUAL(eic.second.get<std::string>("from_port"), coupled_inport_name);

        std::string to_model_name = eic.second.get<std::string>("to_model");

        auto it = std::find(submodel_names.begin(), submodel_names.end(), to_model_name);
        BOOST_CHECK(it != submodel_names.end());
        submodel_names.erase(it);
    }
}

BOOST_AUTO_TEST_CASE( coupled_models_have_coupled_only_coupled_child_connected_to_output) {
    auto TOP_submodels = tree.get_child("models");
    auto it_coupled = std::find_if(TOP_submodels.begin(), TOP_submodels.end(), iscoupled);
    BOOST_REQUIRE(it_coupled != TOP_submodels.end()); //a coupled model was found

    auto submodels = it_coupled->second.get_child("models");
    auto eocs = it_coupled->second.get_child("eoc");

    BOOST_CHECK_EQUAL(eocs.size(), 1);

    auto coupled_child = std::find_if(submodels.begin(), submodels.end(), iscoupled);
    std::string coupled_child_name = coupled_child->second.get<std::string>("id");

    std::string coupled_outport_name = it_coupled->second.get_child("ports.out..name").data();

    BOOST_CHECK_EQUAL(eocs.get<std::string>(".to_port"), coupled_outport_name);
    BOOST_CHECK_EQUAL(eocs.get<std::string>(".from_model"), coupled_child_name);
}







BOOST_AUTO_TEST_SUITE_END()
