#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/renderer/tile_parameters.hpp>
#include <mbgl/util/tile_cover.hpp>
#include <mbgl/math/log2.hpp>
#include <mbgl/renderer/painter.hpp>

namespace mbgl {

using namespace style;

RenderImageSource::RenderImageSource(Immutable<style::ImageSource::Impl> impl_)
: RenderSource(impl_) {
}

const style::ImageSource::Impl& RenderImageSource::impl() const {
    return static_cast<const style::ImageSource::Impl&>(*baseImpl);
}

bool RenderImageSource::isLoaded() const {
    return !!bucket;
}

void RenderImageSource::startRender(algorithm::ClipIDGenerator& ,
                                    const mat4& projMatrix,
                                    const mat4& ,
                                    const TransformState& transformState) {

    if (!isLoaded()) {
        return;
    }
    matrix::identity(matrix);
    transformState.matrixFor(matrix, *tileId);
    matrix::multiply(matrix, projMatrix, matrix);
}

void RenderImageSource::finishRender(Painter& painter) {
    if (!isLoaded()) {
        return;
    }
    painter.renderTileDebug(matrix);
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

void RenderImageSource::upload(gl::Context& context) {
    if (isLoaded() && bucket->needsUpload()) {
        bucket->upload(context);
    }
}

void RenderImageSource::updateTiles(const TileParameters& parameters) {
    if(isLoaded()) {
        return;
    }
    auto transformState = parameters.transformState;
    auto size = transformState.getSize();
    double viewportHeight = size.height;

    auto coords = impl().getCoordinates();

    ScreenCoordinate nePixel = {-INFINITY, -INFINITY};
    ScreenCoordinate swPixel = {INFINITY, INFINITY};

    for (LatLng latLng : coords) {
        ScreenCoordinate pixel = transformState.latLngToScreenCoordinate(latLng);
        swPixel.x = std::min(swPixel.x, pixel.x);
        nePixel.x = std::max(nePixel.x, pixel.x);
        swPixel.y = std::min(swPixel.y, viewportHeight - pixel.y);
        nePixel.y = std::max(nePixel.y, viewportHeight - pixel.y);
    }

    double width = nePixel.x - swPixel.x;
    double height = nePixel.y - swPixel.y;

    // Calculate the zoom level.
    double minScale = INFINITY;
    if (width > 0 || height > 0) {
        double scaleX = double(size.width) / width;
        double scaleY = double(size.height) / height;
        minScale = util::min(scaleX, scaleY);
    }
    double zoom = transformState.getZoom() + util::log2(minScale);
    zoom = util::clamp(zoom, transformState.getMinZoom(), transformState.getMaxZoom());

    // Calculate Geometry Coordinates based on ideal Tile for these LatLng
    auto imageBounds = LatLngBounds::hull(coords[0], coords[1]);
    imageBounds.extend(coords[2]);
    imageBounds.extend(coords[3]);
    auto tileCover = util::tileCover(imageBounds, ::round(zoom));
    tileId = std::make_unique<UnwrappedTileID>(tileCover[0].wrap, tileCover[0].canonical);
    GeometryCoordinates geomCoords;
    for ( auto latLng : coords) {
        auto tc = TileCoordinate::fromLatLng(0, latLng);
        auto gc = TileCoordinate::toGeometryCoordinate(tileCover[0], tc.p);
        geomCoords.push_back(gc);
    }
    setupBucket(geomCoords);
}

void RenderImageSource::setupBucket(GeometryCoordinates& geomCoords) {
    UnassociatedImage img = impl().getImage().clone();
    if (!img.valid()) {
        return;
    }
    bucket = std::make_unique<RasterBucket>(std::move(img));

    //Set Bucket Vertices, Indices, and segments
    bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[0].x, geomCoords[0].y }, { 0, 0 }));
    bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[1].x, geomCoords[1].y }, { 32767, 0 }));
    bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[3].x, geomCoords[3].y }, { 0, 32767 }));
    bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[2].x, geomCoords[2].y }, { 32767, 32767 }));

    bucket->indices.emplace_back(0, 1, 2);
    bucket->indices.emplace_back(1, 2, 3);

    bucket->segments.emplace_back(0, 0, 4, 6);
}

void RenderImageSource::render(Painter& painter, PaintParameters& parameters, const RenderLayer& layer)  {
    if(isLoaded() && !bucket->needsUpload()) {
        bucket->render(painter, parameters, layer, matrix);
    }
}

void RenderImageSource::dumpDebugLogs() const {
    Log::Info(Event::General, "RenderImageSource::id: %s", impl().id.c_str());
    Log::Info(Event::General, "RenderImageSource::loaded: %s", isLoaded() ? "yes" : "no");
}

} // namespace mbgl
