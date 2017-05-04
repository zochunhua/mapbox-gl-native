#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/algorithm/generate_clip_ids.hpp>
#include <mbgl/algorithm/generate_clip_ids_impl.hpp>

namespace mbgl {

using namespace style;

RenderImageSource::RenderImageSource(const style::ImageSource::Impl& impl_)
: RenderSource(impl_),
impl(impl_) {
}

bool RenderImageSource::isLoaded() const {
    return false;
}

void RenderImageSource::startRender(algorithm::ClipIDGenerator& ,
                                    const mat4& , const mat4& , const TransformState& ) {
    impl.getData();
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

void RenderImageSource::updateTiles(const TileParameters&) {
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
