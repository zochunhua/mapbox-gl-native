#include <mbgl/renderer/layers/render_raster_layer.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/style/layers/raster_layer_impl.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/tile/tile.hpp>

namespace mbgl {

RenderRasterLayer::RenderRasterLayer(Immutable<style::RasterLayer::Impl> _impl)
    : RenderLayer(style::LayerType::Raster, _impl) {
}

const style::RasterLayer::Impl& RenderRasterLayer::impl() const {
    return static_cast<const style::RasterLayer::Impl&>(*baseImpl);
}

std::unique_ptr<Bucket> RenderRasterLayer::createBucket(const BucketParameters&, const std::vector<const RenderLayer*>&) const {
    assert(false);
    return nullptr;
}

void RenderRasterLayer::transition(const TransitionParameters& parameters) {
    unevaluated = impl().paint.transition(parameters, std::move(unevaluated));
}

void RenderRasterLayer::evaluate(const PropertyEvaluationParameters& parameters) {
    evaluated = unevaluated.evaluate(parameters);

    passes = evaluated.get<style::RasterOpacity>() > 0 ? RenderPass::Translucent : RenderPass::None;
}

bool RenderRasterLayer::hasTransition() const {
    return unevaluated.hasTransition();
}

void RenderRasterLayer::uploadBuckets(gl::Context& context) {
    for (const auto& tileRef : renderTiles) {
        const auto& bucket = tileRef.get().tile.getBucket(*this);
        if (bucket && bucket->needsUpload()) {
            bucket->upload(context);
        }
    }
}

void RenderRasterLayer::render(Painter& painter, PaintParameters& parameters, const RenderSource*) {
    for (auto& tileRef : renderTiles) {
        auto& tile = tileRef.get();
//        MBGL_DEBUG_GROUP(context, getID() + " - " + util::toString(tile.id));
        auto bucket = tile.tile.getBucket(*this);
        bucket->render(painter, parameters, *this, tile);
    }
}


} // namespace mbgl
