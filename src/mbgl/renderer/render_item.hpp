#pragma once

#include <mbgl/util/color.hpp>

#include <unordered_set>
#include <vector>

namespace mbgl {

class RenderLayer;
class RenderSource;
class RenderTile;
class Bucket;

namespace style {
} // namespace style

class RenderItem {
public:
    RenderItem(const RenderLayer& layer_,
               RenderSource * renderSource_,
               std::vector<std::reference_wrapper<RenderTile>> tiles_ = {})
        : layer(layer_), source(renderSource_), tiles(std::move(tiles_)) {
    }

    const RenderLayer& layer;
    const RenderSource* source;
    std::vector<std::reference_wrapper<RenderTile>> tiles;
};

class RenderData {
public:
    Color backgroundColor;
    std::unordered_set<RenderSource*> sources;
    std::vector<RenderItem> order;
};

} // namespace mbgl
