/* VERTEX.hpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 14:32:39
 * Last edited:
 *   13/01/2021, 14:32:39
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Vertex class to define how a single vertex looks like in our program.
**/

#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <vulkan/vulkan.h>

#include "glm/glm.hpp"
#include "Tools/Array.hpp"

namespace HelloVikingRoom {
    /* The Vertex class, which defines how a single vertex looks like in our program. */
    class Vertex {
    public:
        /* Describes the position (in 2D) of our vertex. */
        glm::vec2 pos;
        /* Describes the color (as RGB) of our vertex. */
        glm::vec3 color;

        /* Constructor for the Vertex class, which takes a position / color pair. */
        Vertex(const glm::vec2& pos, const glm::vec3& color);

        /* Returns the binding descriptor, i.e., how we'll read memory. */
        static VkVertexInputBindingDescription getBindingDescription();
        /* Returns an attribute descriptor, which tells Vulkan what to do with each chunk read specified by the binding. */
        static Tools::Array<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };
}

#endif
