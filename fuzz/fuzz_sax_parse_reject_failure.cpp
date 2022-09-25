#include "yaml/yaml2.hpp"

struct test_sax_handler {
    void null() {}
    void start_object() {}
    void end_object() {}
    void start_array() {}
    void end_array() {}
    void key(yaml::basic_string_view<uint8_t>) {}
    void string(yaml::basic_string_view<uint8_t>) {}
    void comment(yaml::basic_string_view<uint8_t>) {}
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    test_sax_handler handler = {};
    if(yaml::sax::parse(data, size, handler) == yaml::parse_result_code::success) {
        return 0;
    }
    return -1;
}