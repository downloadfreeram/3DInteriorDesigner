#ifndef MODEL_SNAPSHOT_H
#define MODEL_SNAPSHOT_H

#include <iostream>
#include <glm/glm.hpp>
#include "Model.h"  

struct ModelSnapshot {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    // Include any additional properties that are important for your model

    // default constructor
    ModelSnapshot() = default;

    // constructor to create a snapshot from a Model
    ModelSnapshot(const Model& model)
        : position(model.getPosition()),
        rotation(model.getRotation()),
        scale(model.getScale()) {
        // Initialize other properties from the model here
    }

    // apply this snapshot to a Model
    void applyToModel(Model& model) const {
        model.setPosition(position);
        model.setRotation(rotation);
        model.setScale(scale);
        // Set other properties to the model here
    }

    // serialize the snapshot to an output stream
    void serialize(std::ostream& os) const {
        os.write(reinterpret_cast<const char*>(&position), sizeof(position));
        os.write(reinterpret_cast<const char*>(&rotation), sizeof(rotation));
        os.write(reinterpret_cast<const char*>(&scale), sizeof(scale));
        // Serialize other properties here
    }

    // deserialize the snapshot from an input stream
    void deserialize(std::istream& is) {
        is.read(reinterpret_cast<char*>(&position), sizeof(position));
        is.read(reinterpret_cast<char*>(&rotation), sizeof(rotation));
        is.read(reinterpret_cast<char*>(&scale), sizeof(scale));
        // Deserialize other properties here
    }
};

#endif // MODEL_SNAPSHOT_H
