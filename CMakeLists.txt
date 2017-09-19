project(PedestrianTracking)
cmake_minimum_required(VERSION 3.1)

find_package( OpenCV REQUIRED )
find_library( DARKNET_LIB darknet PATHS ${PROJECT_SOURCE_DIR}/data/yolo/lib )

#needs opencv_contrib
add_definitions(-DUSE_OCV_UKF)
add_definitions(-DUSE_OCV_KCF)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

configure_file(config.cfg config.cfg COPYONLY)

set(folder_source  src/main.cpp
    src/TrackerInterface.cpp
    src/Properties.cpp
    src/Detector/IBackgroundSubtractor.cpp
    src/Detector/InceptionClassifierInterface.cpp
    src/Detector/YoloInterface.cpp
    src/Tracker/Ctracker.cpp
    src/Tracker/HungarianAlg/HungarianAlg.cpp
    src/Tracker/LocalTracker.cpp
    src/Tracker/Kalman.cpp
    src/ConfigLoader/ConfigFile.cpp)

set(folder_header
    src/Utils.h
    src/DetectorInterface.h
    src/TrackerInterface.h
    src/Detector/IBackgroundSubtractor.h
    src/Detector/InceptionClassifierInterface.h
    src/Detector/YoloInterface.h
    src/Tracker/Ctracker.h
    src/Tracker/track.h
    src/Tracker/HungarianAlg/HungarianAlg.h
    src/Tracker/LocalTracker.h
    src/Tracker/Kalman.h
    src/Tracker/defines.h
    src/ConfigLoader/ConfigFile.h
    src/Properties.h)

set(graph_source
    src/Tracker/graph/tokenise.cpp
    src/Tracker/graph/mytree.cpp
    src/Tracker/graph/mygraph.cpp
    src/Tracker/graph/mwbmatching.cpp
    src/Tracker/graph/mincut.cpp
    src/Tracker/graph/gport.cpp
    src/Tracker/graph/fheap.c
    )

set(graph_header
   src/Tracker/graph/tokenise.h
   src/Tracker/graph/mytree.h
   src/Tracker/graph/mygraph.h
   src/Tracker/graph/mwbmatching.h
   src/Tracker/graph/mincut.h
   src/Tracker/graph/gport.h
   src/Tracker/graph/gdefs.h
   src/Tracker/graph/fheap.h
   )

set(gtl_source
    src/Tracker/graph/GTL/src/bellman_ford.cpp
    src/Tracker/graph/GTL/src/bfs.cpp
    src/Tracker/graph/GTL/src/biconnectivity.cpp
    src/Tracker/graph/GTL/src/bid_dijkstra.cpp
    src/Tracker/graph/GTL/src/components.cpp
    src/Tracker/graph/GTL/src/debug.cpp
    src/Tracker/graph/GTL/src/dfs.cpp
    src/Tracker/graph/GTL/src/dijkstra.cpp
    src/Tracker/graph/GTL/src/edge.cpp
    src/Tracker/graph/GTL/src/embedding.cpp
    src/Tracker/graph/GTL/src/fm_partition.cpp
    src/Tracker/graph/GTL/src/gml_parser.cpp
    src/Tracker/graph/GTL/src/gml_scanner.cpp
    src/Tracker/graph/GTL/src/graph.cpp
    src/Tracker/graph/GTL/src/maxflow_ff.cpp
    src/Tracker/graph/GTL/src/maxflow_pp.cpp
    src/Tracker/graph/GTL/src/maxflow_sap.cpp
    src/Tracker/graph/GTL/src/min_tree.cpp
    src/Tracker/graph/GTL/src/node.cpp
    src/Tracker/graph/GTL/src/planarity.cpp
    src/Tracker/graph/GTL/src/pq_node.cpp
    src/Tracker/graph/GTL/src/pq_tree.cpp
    src/Tracker/graph/GTL/src/ratio_cut_partition.cpp
    src/Tracker/graph/GTL/src/st_number.cpp
    src/Tracker/graph/GTL/src/topsort.cpp
    )

set(gtl_header
    src/Tracker/graph/GTL/include/GTL/algorithm.h
    src/Tracker/graph/GTL/include/GTL/bellman_ford.h
    src/Tracker/graph/GTL/include/GTL/bfs.h
    src/Tracker/graph/GTL/include/GTL/biconnectivity.h
    src/Tracker/graph/GTL/include/GTL/bid_dijkstra.h
    src/Tracker/graph/GTL/include/GTL/bin_heap.h
    src/Tracker/graph/GTL/include/GTL/components.h
    src/Tracker/graph/GTL/include/GTL/debug.h
    src/Tracker/graph/GTL/include/GTL/dfs.h
    src/Tracker/graph/GTL/include/GTL/dijkstra.h
    src/Tracker/graph/GTL/include/GTL/edge.h
    src/Tracker/graph/GTL/include/GTL/edge_data.h
    src/Tracker/graph/GTL/include/GTL/edge_map.h
    src/Tracker/graph/GTL/include/GTL/embedding.h
    src/Tracker/graph/GTL/include/GTL/fm_partition.h
    src/Tracker/graph/GTL/include/GTL/gml_parser.h
    src/Tracker/graph/GTL/include/GTL/gml_scanner.h
    src/Tracker/graph/GTL/include/GTL/graph.h
    src/Tracker/graph/GTL/include/GTL/maxflow_ff.h
    src/Tracker/graph/GTL/include/GTL/maxflow_pp.h
    src/Tracker/graph/GTL/include/GTL/maxflow_sap.h
    src/Tracker/graph/GTL/include/GTL/min_tree.h
    src/Tracker/graph/GTL/include/GTL/ne_map.h
    src/Tracker/graph/GTL/include/GTL/node.h
    src/Tracker/graph/GTL/include/GTL/node_data.h
    src/Tracker/graph/GTL/include/GTL/node_map.h
    src/Tracker/graph/GTL/include/GTL/planarity.h
    src/Tracker/graph/GTL/include/GTL/pq_node.h
    src/Tracker/graph/GTL/include/GTL/pq_tree.h
    src/Tracker/graph/GTL/include/GTL/ratio_cut_partition.h
    src/Tracker/graph/GTL/include/GTL/st_number.h
    src/Tracker/graph/GTL/include/GTL/symlist.h
    src/Tracker/graph/GTL/include/GTL/topsort.h
    src/Tracker/graph/GTL/include/GTL/version.h
    src/Tracker/graph/GTL/include/GTL/GTL.h
)

SOURCE_GROUP("Source Files" FILES ${folder_source})
SOURCE_GROUP("Header Files" FILES ${folder_header})
SOURCE_GROUP("graph" FILES ${graph_source} ${graph_header})
SOURCE_GROUP("GTL" FILES ${gtl_source} ${gtl_header})

add_executable(${PROJECT_NAME} ${folder_source} ${folder_header} ${graph_source} ${graph_header} ${gtl_source} ${gtl_header})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} ${DARKNET_LIB} )

INCLUDE_DIRECTORIES(  ${PROJECT_SOURCE_DIR}/src
                      ${PROJECT_SOURCE_DIR}/src/Detector
                      ${PROJECT_SOURCE_DIR}/src/Tracker
                      ${PROJECT_SOURCE_DIR}/src/Tracker/HungarianAlg
                      ${PROJECT_SOURCE_DIR}/src/Tracker/graph
                      ${PROJECT_SOURCE_DIR}/src/Tracker/graph/GTL/include
                      ${PROJECT_SOURCE_DIR}/data/yolo/include)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${PROJECT_SOURCE_DIR}/data/yolo/lib/libdarknet.so"
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${PROJECT_SOURCE_DIR}/data/yolo"
      $<TARGET_FILE_DIR:${PROJECT_NAME}>)

