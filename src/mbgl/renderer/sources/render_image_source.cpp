#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/renderer/tile_parameters.hpp>

namespace mbgl {

using namespace style;

RenderImageSource::RenderImageSource(const style::ImageSource::Impl& impl_)
: RenderSource(impl_),
 impl(impl_),
 isImageLoaded(false) {
    bucket = nullptr;
}

bool RenderImageSource::isLoaded() const {
    return isImageLoaded;
}

void RenderImageSource::startRender(algorithm::ClipIDGenerator& ,
                                    const mat4& ,
                                    const mat4& ,
                                    const TransformState& ) {

}

void RenderImageSource::finishRender(Painter& ) {
}

std::unordered_map<std::string, std::vector<Feature>>
RenderImageSource::queryRenderedFeatures(const ScreenLineString& ,
                                         const TransformState& ,
                                         const RenderedQueryOptions& ) const {
    return {};
}

std::vector<Feature> RenderImageSource::querySourceFeatures(const SourceQueryOptions& ) const {
    return {};
}

void RenderImageSource::removeTiles() {

}

void RenderImageSource::updateTiles(const TileParameters& ) {
    if(!isImageLoaded && impl.loaded) {
        bucket = new RasterBucket(std::move(*impl.getData()));
        isImageLoaded = true;
    }
}

void RenderImageSource::reloadTiles() {
}

void RenderImageSource::invalidateTiles() {
}

std::map<UnwrappedTileID, RenderTile>& RenderImageSource::getRenderTiles() {
    return tiles;
}

void RenderImageSource::setCacheSize(size_t ) {
}

void RenderImageSource::onLowMemory() {
}

void RenderImageSource::dumpDebugLogs() const {
}

} // namespace mbgl
