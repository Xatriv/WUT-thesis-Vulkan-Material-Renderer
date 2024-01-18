#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include "app.h"
#include "app_config.h"

int main() {
    try {
        vmr::AppConfig config = vmr::AppConfig("./config.json");
        vmr::App app(&config);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}