// "PlanetRenderer is a little too boring" -- Capital Asterisk, 2018
#pragma once

#include <Urho3D/Graphics/Geometry.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Model.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/VertexBuffer.h>

#include <Urho3D/Resource/ResourceCache.h>

#include <Urho3D/Container/Ptr.h>

#include <Urho3D/IO/Log.h>

#include <Urho3D/Core/CoreEvents.h>

#include <cstdint>

namespace osp
{

// The 20 faces of the icosahedron (Top, Left, Right)
// Each number pointing to a vertex
static constexpr uint8_t sc_icoTemplateTris[20 * 3] {
//  TT  LL  RR   TT  LL  RR   TT  LL  RR   TT  LL  RR   TT  LL  RR
     0,  2,  1,   0,  3,  2,   0,  4,  3,   0,  5,  4,   0,  1,  5,
     8,  1,  2,   2,  7,  8,   7,  2,  3,   3,  6,  7,   6,  3,  4,
     4,  10, 6,  10,  4,  5,   5,  9, 10,   9,  5,  1,   1,  8,  9,
    11,  7,  6,  11,  8,  7,  11,  9,  8,  11, 10,  9,  11,  6, 10
};

// The 20 faces of the icosahedron (Bottom, Right, Left)
static constexpr uint8_t sc_icoTemplateneighbours[20 * 3] {
//  BB  RR  LL   BB  RR  LL   BB  RR  LL   BB  RR  LL   BB  RR  LL
     5,  4,  1,   7,  0,  2,   9,  1,  3,  11,  2,  4,  13,  3,  0,
     0,  6, 14,  16,  5,  7,   1,  8,  6,  15,  7,  9,   2, 10,  8,
    19,  9, 11,   3, 12, 10,  18, 11, 13,   4, 14, 12,  17, 13,  5,
     8, 19, 16,   6, 15, 17,  14, 16, 18,  12, 17, 19,  10, 18, 15
};

// If this changes, then the universe is broken
static constexpr int gc_icosahedronFaceCount = 20;

static constexpr std::uint8_t gc_triangleMaskSubdivided = 0b0001;
static constexpr std::uint8_t gc_triangleMaskChunked    = 0b0010;

// Index to a triangle
using trindex = uint32_t;

// Index to a chunk
using chindex = unsigned int; // TODO: Why use 'unsigned' instead of uint32_t?

// Index to a buffer
using buindex = uint32_t;

struct UpdateRange
{
    // initialize with maximum buindex value for start (2^32),
    // and minimum buindex for end (0)
    // the first min and max operations will replace them
    buindex m_start = UINT32_MAX;
    buindex m_end   = 0;
    UpdateRange() = default;
};

// Triangle on the IcoSphereTree
struct SubTriangle
{
    trindex m_parent;
    trindex m_neighbours[3];
    buindex m_corners[3]; // to vertex buffer, 3 corners of triangle

    //bool subdivided;
    uint8_t m_bitmask;
    uint8_t m_depth;
    Urho3D::Vector3 m_center;

    // Data used when subdivided

    // index to first child, always has 4 children if subdivided
    trindex m_children;
    buindex m_midVerts[3]; // Bottom, Right, Left vertices in index buffer
    buindex m_index; // to index buffer

    // Data used when chunked
    chindex m_chunk; // Index to chunk. (First triangle ever chunked will be 0)
    buindex m_chunkIndex; // Index to index data in the index buffer
    buindex m_chunkVerts; // Index to vertex data
};


// An icosahedron with subdividable faces
// it starts with 20 triangles, and each face can be subdivided into 4 more
// Not implemented: this can be shared between multiple PlanetWrenderers
class IcoSphereTree : public Urho3D::RefCounted
{
    friend class PlanetWrenderer;

    IcoSphereTree() = default;
    ~IcoSphereTree() = default;

public:

    void initialize();

    /**
     * Get triangle from vector of triangles
     * be careful of reallocation!
     * @param t [in] Index to triangle
     * @return Pointer to triangle
     */
    SubTriangle* get_triangle(trindex t) const
    {
        return m_triangles.Buffer() + t;
    }

    /**
     * A quick way to set neighbours of a triangle
     * @param tri [ref] Reference to triangle
     * @param bot [in] Bottom
     * @param rte [in] Right
     * @param lft [in] Left
     */
    static void set_neighbours(SubTriangle& tri, trindex bot,
                               trindex rte, trindex lft);

    /**
     * A quick way to set vertices of a triangle
     * @param tri [ref] Reference to triangle
     * @param top [in] Top
     * @param lft Left
     * @param rte Right
     */
    static void set_verts(SubTriangle& tri, trindex top,
                          trindex lft, trindex rte);

    void set_side_recurse(SubTriangle& tri, int side, trindex to);

    /**
     * Find which side a triangle is on another triangle
     * @param [ref] tri Reference to triangle to be searched
     * @param [in] lookingFor Index of triangle to search for
     * @return Neighbour index (0 - 2), or bottom, left, or right
     */
    static int neighbour_side(const SubTriangle& tri,
                              const trindex lookingFor);


    /**
     * Subdivide a triangle into 4 more
     * @param [in] Triangle to subdivide
     */
    void subdivide_add(trindex t);

    /**
     * Unsubdivide a triangle.
     * Removes children and sets neighbours of neighbours
     * @param t [in] Index of triangle to unsubdivide
     */
    void subdivide_remove(trindex t);

    /**
     * Calculates and sets m_center
     * @param tri [ref] Reference to triangle
     */
    void calculate_center(SubTriangle& tri);

private:



    // 6 components per vertex
    // PosX, PosY, PosZ, NormX, NormY, NormZ
    static constexpr int m_vertCompCount = 6;

    //PODVector<PlanetWrenderer> m_viewers;
    Urho3D::PODVector<float> m_vertBuf;
    Urho3D::PODVector<SubTriangle> m_triangles; // List of all triangles
    // List of indices to deleted triangles in the m_triangles
    Urho3D::PODVector<trindex> m_trianglesFree;
    Urho3D::PODVector<buindex> m_vertFree; // Deleted vertices in m_vertBuf
    // use "m_indDomain[buindex]" to get a triangle index

    unsigned m_maxDepth;
    unsigned m_minDepth; // never subdivide below this

    buindex m_maxVertice;
    buindex m_maxTriangles;

    buindex m_vertCount;


    float m_radius;
};

// Connects the dots between triangles in IcoSphereTree by making chunks
// Chunks being
class PlanetWrenderer
{

    Urho3D::SharedPtr<IcoSphereTree> m_icoTree;
    Urho3D::SharedPtr<Urho3D::IndexBuffer> m_indBufChunk;
    Urho3D::SharedPtr<Urho3D::VertexBuffer> m_chunkVertBuf;

    Urho3D::Vector3 m_offset;
    Urho3D::Vector3 m_camera;
    chindex m_chunkCount; // How many chunks there are right now

    Urho3D::PODVector<trindex> m_chunkIndDomain; // Maps chunks to triangles
    // Spots in the index buffer that want to die
    //Urho3D::PODVector<chindex> m_chunkIndDeleteMe;
    // List of deleted chunk data to overwrite
    Urho3D::PODVector<buindex> m_chunkVertFree;
    // same as above but for individual shared verticies
    Urho3D::PODVector<buindex> m_chunkVertFreeShared;

    // it's impossible for a vertex to have more than 6 users
    // Delete a shared vertex when it's users goes to zero
    // And use user count to calculate normals

    // Count how many times each shared chunk vertex is being used
    Urho3D::PODVector<uint8_t> m_chunkVertUsers;

    // Maps shared vertex indices to the index buffer, so that shared vertices
    // can be obtained from a chunk
    Urho3D::PODVector<buindex> m_chunkSharedIndices;

    Urho3D::Model* m_model;
    Urho3D::Geometry* m_geometryChunk; // Geometry for chunks


    buindex m_chunkVertCountShared; // Current number of shared vertices

    float m_cameraDist;
    float m_threshold;

    // Approx. screen area a triangle can take before it should be subdivided
    float m_subdivAreaThreshold = 0.02f;

    // Preferred total size of chunk vertex buffer (m_chunkVertBuf)
    buindex m_chunkMaxVert;
    // How much is reserved for shared vertices
    buindex m_chunkMaxVertShared;
    chindex m_maxChunks; // Max number of chunks

    // How much screen area a triangle can take before it should be chunked
    float m_chunkAreaThreshold = 0.04f;
    unsigned m_chunkResolution = 31; // How many vertices wide each chunk is
    unsigned m_chunkVertsPerSide; // = m_chunkResolution - 1
    unsigned m_chunkSharedCount; // How many shared verticies per chunk
    unsigned m_chunkSize; // How many vertices there are in each chunk
    unsigned m_chunkSizeInd; // How many triangles in each chunk

    // Not implented, for something like running a server
    //
    //bool m_noGPU = false;

    bool m_ready = false;

    // Vertex buffer data is divided unevenly for chunks
    // In m_chunkVertBuf:
    // [shared vertex data, shared vertices]
    //                     ^               ^
    //        (m_chunkMaxVertShared)    (m_chunkMaxVert)

    // if chunk resolution is 16, then...
    // Chunks are triangles of 136 vertices (m_chunkSize)
    // There are 45 vertices on the edges, (sides + corners)
    // = (14 + 14 + 14 + 3) = m_chunkSharedCount;
    // Which means there is 91 vertices left in the middle
    // (m_chunkSize - m_chunkSharedCount)

public:
    PlanetWrenderer() = default;
    ~PlanetWrenderer() = default;

    constexpr bool is_ready() const;

    /**
     * Calculate initial icosahedron and initialize buffers.
     * Call before drawing
     * @param context [in] Context used to initialize Urho3D objects
     * @param size [in] Minimum height of planet, or radius
     */
    void initialize(Urho3D::Context* context, Urho3D::Image* heightMap,
                    double size);

    /**
     * Recalculates camera positiona and sub_recurses the main 20 triangles.
     * Call this when the camera moves.
     * @param camera [in] Position of camera center
     */
    void update(Urho3D::Vector3 const& camera);


    /**
     * Print out information on vertice count, chunk count, etc...
     */
    void log_stats() const;

    Urho3D::Model* get_model() { return m_model; }

protected:

    /**
     * Recursively check every triangle for which ones need
     * (un)subdividing or chunking
     * @param t [in] Triangle to start with
     */
    void sub_recurse(trindex t);

    /**
     *
     * @param t [in] Index of triangle to add chunk to
     * @param gpuIgnore
     */
    void chunk_add(trindex t, UpdateRange* gpuVertChunk = nullptr,
                   UpdateRange* gpuVertInd = nullptr);

    /**
     * @brief chunk_remove
     * @param t
     * @param gpuIgnore
     */
    void chunk_remove(trindex t, UpdateRange* gpuVertChunk = nullptr,
                      UpdateRange* gpuVertInd = nullptr);

    /**
     * Convert XY coordinates to a triangular number index
     *
     * 0
     * 1  2
     * 3  4  5
     * 6  7  8  9
     * x = right, y = down
     *
     * @param x [in]
     * @param y [in]
     * @return
     */
    constexpr unsigned get_index(int x, int y) const;

    /**
     * Similar to the normal get_index, but the first possible indices returned
     * makes a border around the triangle
     *
     * 6
     * 7  5
     * 8  9  4
     * 0  1  2  3
     * x = right, y = down
     *
     * 0, 1, 2, 3, 4, 5, 6, 7, 8 makes a ring
     *
     * @param x [in]
     * @param y [in]
     * @return
     */
    unsigned get_index_ringed(unsigned x, unsigned y) const;

    /**
     * Grab a shared vertex from the side of a triangle.
     * @param sharedIndex [out] Set to index to shared vertex when successful
     * @param tri [in] Triangle to grab a
     * @param side [in] 0: bottom, 1: right, 2: left
     * @param pos [in] float from (usually) 0.0-1.0, position of vertex to grab
     * @return true when a shared vertex can be taken from tri
     */
    bool get_shared_from_tri(buindex* sharedIndex, const SubTriangle& tri,
                             unsigned side, float pos) const;


    /**
     * Add up memory usages from most variables associated with this instance.
     * @return Memory usage in bytes
     */
    uint64_t get_memory_usage() const;

    /**
     * For debugging only: search for triangles that should have been deleted
     * @param tri
     */
    void find_refs(SubTriangle& tri);
};

constexpr bool PlanetWrenderer::is_ready() const
{
    return m_ready;
}

constexpr unsigned PlanetWrenderer::get_index(int x, int y) const
{
    return unsigned(y * (y + 1) / 2 + x);
}

} // namespace osp
