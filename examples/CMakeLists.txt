# CM2D Examples
# Lightweight wrapper for cm_example_project with module-specific settings

macro(cm_example_project sub_folder project_name)
    cm_example_project_base(
        PROJECT_NAME ${project_name}
        FOLDER_PATH "Examples/CM2D/${sub_folder}"
        SOURCES ${ARGN}
        PRIVATE_LIBRARIES CM2D
    )
endmacro()

# Root examples
#cm_example_project("" TerrainGenExample     TerrainGenExample.cpp)

# Blend system test
cm_example_project("" BlendTest             BlendTest.cpp)

# Transform and Resize test
cm_example_project("" TransformResizeTest   TransformResizeTest.cpp)

# Draw Geometry test
cm_example_project("" DrawGeometryTest      DrawGeometryTest.cpp)

# Channel Operations test
cm_example_project("" ChannelOpsTest        ChannelOpsTest.cpp)

# Curve drawing test
cm_example_project("" DrawCurvesTest        DrawCurvesTest.cpp)

# ImageMagick example (only if ImageMagick is enabled)
if(CM2D_USE_IMAGEMAGICK AND ImageMagick_FOUND)
    cm_example_project("" ImageMagickExample   ImageMagickExample.cpp)
endif()

# Graphics examples subdirectory
add_subdirectory(graphics)

# Pick examples subdirectory
add_subdirectory(pick)
