# set(COMPONENT_SRCS "main.c"
#                    "system_stats.c"
#                    "tasks.c")

# set(COMPONENT_ADD_INCLUDEDIRS "."
#                               "../components")

idf_component_register(SRCS "main.c"
                    "tasks.c"
                    INCLUDE_DIRS "."
                    "../components/")
# # Import tinyxml2 targets 
# add_subdirectory(lib/system_stats)

# register_component()