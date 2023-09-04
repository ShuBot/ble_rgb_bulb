idf_component_register(SRCS "max98357.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver global_functions
                    EMBED_FILES n1.pcm n2.pcm n3.pcm n4.pcm
                    )
