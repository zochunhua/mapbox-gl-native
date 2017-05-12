#include <mbgl/util/logging.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <sstream>

namespace mbgl {
namespace style {

ImageSource::Impl::Impl(std::string id_, Source& base_, const std::vector<LatLng>& coords_)
    : Source::Impl(SourceType::Image, std::move(id_), base_),
    coords(std::move(coords_)) {
}

ImageSource::Impl::~Impl() = default;

void ImageSource::Impl::setURL(std::string url_) {
    url = std::move(url_);
    // Signal that the source description needs a reload
    if (loaded || req) {
        loaded = false;
        req.reset();
        observer->onSourceDescriptionChanged(base);
    }
}

const std::string& ImageSource::Impl::getURL() const {
    return url;
}

void ImageSource::Impl::setCoordinates(const std::vector<LatLng> coords_) {
    coords = std::move(coords_);
    observer->onSourceChanged(base);
}

std::vector<LatLng> ImageSource::Impl::getCoordinates() const {
    return coords;
}

void ImageSource::Impl::setImage(mbgl::UnassociatedImage image_) {
    image = std::move(image_);
    if (req) {
        req.reset();
    }
    observer->onSourceChanged(base);
}

std::unique_ptr<RenderSource> ImageSource::Impl::createRenderSource() const {
    return std::make_unique<RenderImageSource>(*this);
}


void ImageSource::Impl::loadDescription(FileSource& fileSource) {
    if (url.empty()) {
        loaded = true;
    }

    if (req ||  loaded) {
        return;
    }
    const Resource imageResource { Resource::Unknown, url, {}, Resource::Necessity::Required };

    req = fileSource.request(imageResource, [this](Response res) {
        if (res.error) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error("unexpectedly empty image url")));
        } else {
            try {
                image = util::unpremultiply(decodeImage(*res.data));
            } catch (...) {
                observer->onSourceError(base, std::current_exception());
            }
            loaded = true;
            observer->onSourceLoaded(base);
        }
    });
}

const mbgl::UnassociatedImage& ImageSource::Impl::getData() const {
    return image;
}

} // namespace style
} // namespace mbgl
