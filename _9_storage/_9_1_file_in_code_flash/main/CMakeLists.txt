idf_component_register(SRCS
    "main.c"
    INCLUDE_DIRS
    "."
    EMBED_TXTFILES
    "files/index.html"
    "files/sample.txt"
    EMBED_FILES
    "files/pinout.jpg"
)

# ######### OLD WAY
# set(COMPONENT_SRCS "main.c")
# set(COMPONENT_ADD_INCLUDEDIRS ".")

# set(COMPONENT_EMBED_TXTFILES
# "files/index.html"
# "files/sample.txt"
# )

# set(COMPONENT_EMBED_FILES
# "files/pinout.jpg"
# )

# register_component()