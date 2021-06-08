idf_component_register(SRCS node/node.cpp node/node_message.cpp node/node_functions.cpp 
							br/border_router.cpp br/br_message.cpp
                       INCLUDE_DIRS include
                       REQUIRES tcp_client)