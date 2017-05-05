#pragma once

#include <mbgl/renderer/render_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {
class RasterBucket;

class RenderImageSource : public RenderSource {
public:
    RenderImageSource(const style::ImageSource::Impl&);
    
    bool isLoaded() const final;
    
    void startRender(algorithm::ClipIDGenerator&,
                     const mat4& projMatrix,
                     const mat4& clipMatrix,
                     const TransformState&) final;
    void finishRender(Painter&) final;

    void updateTiles(const TileParameters&);
    void removeTiles();
    void invalidateTiles();
    void reloadTiles();
    
    std::map<UnwrappedTileID, RenderTile>& getRenderTiles();

    std::unordered_map<std::string, std::vector<Feature>>
    queryRenderedFeatures(const ScreenLineString& geometry,
                          const TransformState& transformState,
                          const RenderedQueryOptions& options) const final;
    
    std::vector<Feature>
    querySourceFeatures(const SourceQueryOptions&) const final;

    void setCacheSize(size_t) final;
    void onLowMemory() final;
    void dumpDebugLogs() const final;

    RasterBucket* bucket;

private:
    const style::ImageSource::Impl& impl;
    std::map<UnwrappedTileID, RenderTile> tiles;
    bool isImageLoaded;
};
    
} // namespace mbgl
