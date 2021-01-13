/* VERTEX.cpp
 *   by Lut99
 *
 * Created:
 *   13/01/2021, 14:32:34
 * Last edited:
 *   13/01/2021, 14:32:34
 * Auto updated?
 *   Yes
 *
 * Description:
 *   Vertex class to define how a single vertex looks like in our program.
**/

#include "Debug/Debug.hpp"
#include "Vertex.hpp"

using namespace std;
using namespace HelloVikingRoom;
using namespace Tools;
using namespace Debug::SeverityValues;


/***** VERTEX CLASS *****/
/* Constructor for the Vertex class, which takes a position / color pair. */
Vertex::Vertex(const glm::vec2& pos, const glm::vec3& color) :
    pos(pos),
    color(color)
{}

/* Returns the binding descriptor, i.e., how we'll read memory. */
VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription binding_description{};
    // Tell it we'll only use the first (and only) binding we have
    binding_description.binding = 0;
    // Tell it that each vertex is the size of our Vertex struct
    binding_description.stride = sizeof(Vertex);
    // Tell it to use vertex-rendering instead of instance rendering (whatever that may be)
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
}
/* Returns an attribute descriptor, which tells Vulkan what to do with each chunk read specified by the binding. */
Tools::Array<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
    Array<VkVertexInputAttributeDescription> attribute_descriptions = { {}, {} };
    
    // We have two structs, one per input layer
    // First struct: we pass the position to the shader, so we take the first binding, the first location, tell it it are two 32 byte floats and tell it where we can find it in the struct
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, pos);
    // Second struct: we pass the color to the shader, so we take the first binding, the second location, tell it it are three 32 byte floats and tell it where we can find it in the struct
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    // Done, return the structs
    return attribute_descriptions;
}
