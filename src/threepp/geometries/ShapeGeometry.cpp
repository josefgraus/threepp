
#include "threepp/geometries/ShapeGeometry.hpp"
#include "threepp/extras/ShapeUtils.hpp"

#include <functional>

using namespace threepp;


ShapeGeometry::ShapeGeometry(const std::vector<const Shape*>& shapes, unsigned int curveSegments) {

    // buffers

    std::vector<unsigned int> indices;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> uvs;

    // helper variables

    int groupStart = 0;
    int groupCount = 0;

    std::function<void(const Shape&)> addShape = [&](const Shape& shape) {
        const auto indexOffset = vertices.size() / 3;
        const auto points = shape.extractPoints(curveSegments);

        auto shapeVertices = points.shape;
        auto shapeHoles = points.holes;

        // check direction of vertices

        if (!shapeutils::isClockWise(shapeVertices)) {

            std::reverse(shapeVertices.begin(), shapeVertices.end());
        }

        for (auto& shapeHole : shapeHoles) {

            if (shapeutils::isClockWise(shapeHole)) {

                std::reverse(shapeHole.begin(), shapeHole.end());
            }
        }

        auto faces = shapeutils::triangulateShape(shapeVertices, shapeHoles);

        // join vertices of inner and outer paths to a single array

        for (const auto& shapeHole : shapeHoles) {

            shapeVertices.insert(shapeVertices.end(), shapeHole.begin(), shapeHole.end());
        }

        // vertices, normals, uvs

        for (auto& vertex : shapeVertices) {

            vertices.insert(vertices.end(), {vertex.x, vertex.y, 0});
            normals.insert(normals.end(), {0, 0, 1});
            uvs.insert(uvs.end(), {vertex.x, vertex.y});// world uvs
        }

        // indices

        for (const auto& face : faces) {

            const unsigned int a = face[0] + indexOffset;
            const unsigned int b = face[1] + indexOffset;
            const unsigned int c = face[2] + indexOffset;

            indices.insert(indices.end(), {a, b, c});
            groupCount += 3;
        }
    };

    if (shapes.size() == 1) {

        addShape(*shapes.front());

    } else {

        for (unsigned i = 0; i < shapes.size(); i++) {

            addShape(*shapes[i]);

            this->addGroup(groupStart, groupCount, i);// enables MultiMaterial support

            groupStart += groupCount;
            groupCount = 0;
        }
    }

    this->setIndex(indices);
    this->setAttribute("position", FloatBufferAttribute::create(vertices, 3));
    this->setAttribute("normal", FloatBufferAttribute::create(normals, 3));
    this->setAttribute("uv", FloatBufferAttribute::create(uvs, 2));
}

std::shared_ptr<ShapeGeometry> ShapeGeometry::create(const Shape& shape, unsigned int curveSegments) {

    return std::shared_ptr<ShapeGeometry>(new ShapeGeometry({&shape}, curveSegments));
}

std::shared_ptr<ShapeGeometry> ShapeGeometry::create(const std::vector<std::shared_ptr<Shape>>& shapes, unsigned int curveSegments) {

    std::vector<const Shape*> ptrs(shapes.size());
    std::transform(shapes.begin(), shapes.end(), ptrs.begin(), [&](auto& shape) { return shape.get(); });

    return std::shared_ptr<ShapeGeometry>(new ShapeGeometry(ptrs, curveSegments));
}

std::shared_ptr<ShapeGeometry> ShapeGeometry::create(const std::vector<const Shape*>& shapes, unsigned int curveSegments) {

    return std::shared_ptr<ShapeGeometry>(new ShapeGeometry(shapes, curveSegments));
}

std::string ShapeGeometry::type() const {

    return "ShapeGeometry";
}
