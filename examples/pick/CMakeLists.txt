# CM2D Pick Examples
# Lightweight wrapper for cm_example_project with module-specific settings

macro(cm_example_project sub_folder project_name)
    cm_example_project_base(
        PROJECT_NAME ${project_name}
        FOLDER_PATH "Examples/CM2D/${sub_folder}"
        SOURCES ${ARGN}
        PRIVATE_LIBRARIES CM2D
    )
endmacro()

cm_example_project("Pick" Pick2DTest    Pick2DTest.cpp)
