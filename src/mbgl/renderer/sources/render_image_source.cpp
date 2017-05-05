#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/util/tile_coordinate.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

using namespace style;

RenderImageSource::RenderImageSource(const style::ImageSource::Impl& impl_)
: RenderSource(impl_),
 impl(impl_),
 loaded(false) {
}

bool RenderImageSource::isLoaded() const {
    return loaded;
}

void RenderImageSource::startRender(algorithm::ClipIDGenerator& ,
                                    const mat4& projMatrix,
                                    const mat4& ,
                                    const TransformState& transform) {
    matrix::identity(matrix);
    transform.matrixFor(matrix, {0,0,0});
    matrix::multiply(matrix, projMatrix, matrix);
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

void RenderImageSource::upload(gl::Context& context) {
    if (isLoaded() && bucket->needsUpload()) {
        bucket->upload(context);
    }
}

void RenderImageSource::updateTiles(const TileParameters& ) {
    if(impl.loaded && !isLoaded()) {
        //TODO: AHM: Don't make a copy here
        UnassociatedImage img = impl.getData().clone();
        bucket = std::make_unique<RasterBucket>(std::move(img));
        loaded = true;
    }

    if (isLoaded()) {
        auto coords = impl.getCoordinates();
        GeometryCoordinates geomCoords;
        for ( auto latLng : coords) {
            geomCoords.push_back(TileCoordinate::toGeometryCoordinate(latLng));
        }
        assert(geomCoords.size() == 4);
        bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[0].x, geomCoords[0].y }, { 0, 0 }));
        bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[1].x, geomCoords[1].y }, { 32767, 0 }));
        bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[2].x, geomCoords[2].y }, { 0, 32767 }));
        bucket->vertices.emplace_back(RasterProgram::layoutVertex({ geomCoords[3].x, geomCoords[3].y }, { 32767, 32767 }));

        bucket->indices.emplace_back(0, 1, 2);
        bucket->indices.emplace_back(2, 3, 0);

        bucket->segments.emplace_back(0, 0, 4, 6);
    }
}

void RenderImageSource::render(Painter& painter, PaintParameters& parameters, const RenderLayer& layer)  {
    if(isLoaded() && !bucket->needsUpload()) {
        bucket->render(painter, parameters, layer, matrix);
    }
}

void RenderImageSource::dumpDebugLogs() const {
    Log::Info(Event::General, "RenderImageSource::id: %s", baseImpl.id.c_str());
    Log::Info(Event::General, "RenderImageSource::loaded: %s", isLoaded() ? "yes" : "no");
}

} // namespace mbgl
