#include "yaml/yaml2.hpp"

struct test_sax_handler {
    void null() {}
    void start_scalar(yaml::block_style, yaml::chomping) {}
    void end_scalar() {}
    void start_object() {}
    void end_object() {}
    void start_array() {}
    void end_array() {}
    void string(yaml::basic_string_view<uint8_t>) {}
    void key(yaml::basic_string_view<uint8_t>) {}
    void comment(yaml::basic_string_view<uint8_t>) {}
};

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    test_sax_handler handler = {};
    const auto result = yaml::sax::read(data, size, handler);
    if(result.result_code == yaml::read_result_code::success) {
        return 0;
    }
    return -1;
}