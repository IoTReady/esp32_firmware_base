# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# (Not part of the boilerplate)
# Include components from the 'lib' directory for the project
set(EXTRA_COMPONENT_DIRS ../../lib/common ../../lib/system_stats ../../lib/wifi_softap ../../lib/wifi_station ../../lib/utilities ../../lib/wifi_config_rest)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(iotready_wifi_config_rest_example_esp32)
