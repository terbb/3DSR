#define TINYOBJLOADER_IMPLEMENTATION
#include "mesh.h"

Mesh::Mesh(std::string_view path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;  

    // LoadObj now only passes in 1 std::string for error handling for release v1.0.6.
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.data());

    if (!err.empty()) {
      std::cerr << "Error: " << err << std::endl;
    }

    if (!ret) {
      exit(1);
    }

    parse_obj(attrib, shapes, materials);
}

void Mesh::parse_obj(const tinyobj::attrib_t& attrib, const std::vector<tinyobj::shape_t>& shapes, const std::vector<tinyobj::material_t>& materials)
{
    // For each shape...
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // For each face...
        size_t f_index_begin = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // Find the number of vertices that describes it. 
            int num_vertices = shapes[s].mesh.num_face_vertices[f];

            // All vertices are stored in counter-clockwise order by default.
            for (size_t v = 0; v < num_vertices; v++)
            {
                // Fetch the index of the 1st/2nd/3rd... etc. vertex
                // This index is NOT the vertex index, but rather the index in mesh.indices.
                tinyobj::index_t idx = shapes[s].mesh.indices[f_index_begin + v]; 
                // We want to loop back to the first vertex if we are at the last vertex.
                tinyobj::index_t next_idx = shapes[s].mesh.indices[f_index_begin + (v+1)%num_vertices];

                // We can obtain the vertex index by calling idx.vertex_index.
                // All vertices are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index + 2];
                local_coords.push_back(vec3(vx, vy, vz));

                // We can obtain the vertex index by calling idx.normal_index.
                // All normals are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index + 2];
                normals.push_back(vec3(nx, ny, nz));

                // Textures.
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                texture_coords.push_back(vec2(tx, ty));
            }

            // The index at which each face begins in mesh.indices.
            // Remember that each face could have a variable number of vertices (3, 4, 5, etc.), so the index at which a face begins must be adjusted
            // each time for each face that we process.
            f_index_begin += num_vertices;
        }
    }

}