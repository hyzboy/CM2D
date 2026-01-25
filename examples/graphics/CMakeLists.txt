# CM2D Graphics Examples
# Lightweight wrapper for cm_example_project with module-specific settings

macro(cm_example_project sub_folder project_name)
    cm_example_project_base(
        PROJECT_NAME ${project_name}
        FOLDER_PATH "Examples/CM2D/${sub_folder}"
        SOURCES ${ARGN}
        PRIVATE_LIBRARIES CM2D
    )
endmacro()

cm_example_project("Graphics" DistributionChart2D DistributionChart2D.cpp BitmapFont.cpp)
cm_example_project("Graphics" PlayerTraceChart2D PlayerTraceChart2D.cpp)
