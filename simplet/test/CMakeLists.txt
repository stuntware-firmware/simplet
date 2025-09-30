# This file is for ESP-IDF test component registration only
# For host builds, the root CMakeLists.txt handles everything

if(ESP_PLATFORM)
    idf_component_register(
        SRC_DIRS
            "."
        INCLUDE_DIRS
            "."
        REQUIRES
            unity
            simplet
    )
endif()