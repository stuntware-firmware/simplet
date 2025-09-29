# This file is for ESP-IDF component registration only
# For host builds, the root CMakeLists.txt handles everything

if(ESP_PLATFORM)
    idf_component_register(
        SRCS
            "simplet.c"
        INCLUDE_DIRS
            "include"
    )
endif()