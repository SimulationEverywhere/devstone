#ifndef TEST_HELPERS_HPP
#define TEST_HELPERS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "../DEVSDiagrammer/model_json_exporter/include/dynamic_json_exporter.hpp"

namespace pt = boost::property_tree;
using TIME = float;

pt::ptree to_prop_tree(std::shared_ptr<cadmium::dynamic::modeling::coupled<TIME>>& model) {
    std::stringstream modeljson;
    dynamic_export_model_to_json(modeljson, model);
//    std::cout << modeljson.str() << std::endl;
    pt::ptree tree;
    pt::read_json(modeljson, tree);
    return tree;
}


auto iscoupled = [](auto par) {
    return par.second.get_child("type").data() == "coupled";
};
#endif // TEST_HELPERS_HPP
