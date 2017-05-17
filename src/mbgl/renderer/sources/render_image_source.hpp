#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/tile/geometry_tile_data.hpp>
#include <mbgl/util/optional.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>

namespace mbgl {
class RenderLayer;
class PaintParameters;

namespace gl {
    class Context;
}

class RenderImageSource : public RenderSource {
public:
    RenderImageSource(Immutable<style::ImageSource::Impl>);
    
    bool isLoaded() const final;
    void upload(gl::Context&) ;

    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&) final;

    void render(Painter&, PaintParameters& , const RenderLayer& ) ;

    void finishRender(Painter&) final;

    void updateTiles(const TileParameters&);
    void removeTiles() {}
    void invalidateTiles() {}
    void reloadTiles() {}

    std::map<UnwrappedTileID, RenderTile>& getRenderTiles() {
        return tiles;
    }

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderedQueryOptions& options) const final;

    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final {}
    void onLowMemory() final {}
    void dumpDebugLogs() const final;

private:
    const style::ImageSource::Impl& impl() const;

    void setupBucket(GeometryCoordinates& coordiantes);
    std::map<UnwrappedTileID, RenderTile> tiles;

    std::unique_ptr<UnwrappedTileID> tileId;
    std::unique_ptr<RasterBucket> bucket;
    mat4 matrix;

};
    
} // namespace mbgl
