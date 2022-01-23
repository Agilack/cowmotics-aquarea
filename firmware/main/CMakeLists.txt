##
 # @file  main/CMakeLists.txt
 # @brief CMake script used by idf to build "main" component
 #
 # @page License
 # This firmware is free software: you can redistribute it and/or modify it
 # under the terms of the GNU General Public License version 3 as published
 # by the Free Software Foundation. You should have received a copy of the
 # GNU General Public License along with this program, see LICENSE.md file
 # for more details.
 # This program is distributed WITHOUT ANY WARRANTY.
##

idf_component_register(SRCS "main.c"
                            "aquarea.c" "aquarea_ll.c"
                       INCLUDE_DIRS ".")
