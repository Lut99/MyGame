# Add the subdirectories
add_subdirectory(Application)
add_subdirectory(Debug)
add_subdirectory(Shaders)
add_subdirectory(Vulkan)

# Carry the list to the parent scope
set(EXTRA_LIBS "${EXTRA_LIBS}" PARENT_SCOPE)
