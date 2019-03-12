
auto iscoupled = [](auto par) {
    return par.second.get_child("type").data() == "coupled";
};
