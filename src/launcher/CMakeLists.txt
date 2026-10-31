add_executable(Launch WIN32
    main.cpp
)

target_compile_features(Launch PRIVATE cxx_std_17)

target_link_libraries(Launch
    PRIVATE
        user32
        shell32
)

set_target_properties(Launch PROPERTIES
    OUTPUT_NAME "Launch"
)
