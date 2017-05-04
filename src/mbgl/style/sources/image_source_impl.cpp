#include <mbgl/util/logging.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/renderer/raster_bucket.hpp>
#include <mbgl/util/premultiply.hpp>
#include <mbgl/renderer/sources/render_image_source.hpp>
#include <mbgl/renderer/render_tile.hpp>

#include <sstream>

namespace mbgl {
namespace style {

ImageSource::Impl::Impl(std::string id_, Source& base_, const std::string& url_,
                        const std::vector<LatLng>& coords_)
    : Source::Impl(SourceType::Image, std::move(id_), base_),
    url(url_),
    coords(std::move(coords_)) {
}

ImageSource::Impl::~Impl() = default;

void ImageSource::Impl::setURL(std::string url_) {
    url = std::move(url_);
}

const std::string& ImageSource::Impl::getURL() const {
    return url;
}

void ImageSource::Impl::setCoordinates(const std::vector<LatLng> coords_) {
    coords = std::move(coords_);
}

std::unique_ptr<RenderSource> ImageSource::Impl::createRenderSource() const {
    return std::make_unique<RenderImageSource>(*this);
}


void ImageSource::Impl::loadDescription(FileSource& fileSource) {

    if (req) {
        return;
    }

    req = fileSource.request(Resource::source(url), [this](Response res) {
        if (res.error) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error(res.error->message)));
        } else if (res.notModified) {
            return;
        } else if (res.noContent) {
            observer->onSourceError(base, std::make_exception_ptr(std::runtime_error("unexpectedly empty image url")));
        } else {
            try {
                image = std::make_unique<mbgl::UnassociatedImage>(util::unpremultiply(decodeImage(*res.data)));
            } catch (...) {
                observer->onSourceError(base, std::current_exception());
            }

            loaded = true;

            observer->onSourceLoaded(base);
        }
    });
}

mbgl::UnassociatedImage* ImageSource::Impl::getData() const {
    return image.get();
}

} // namespace style
} // namespace mbgl
