#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {

ImageSource::ImageSource(std::string id, const std::string& url,
                         const std::vector<LatLng> coords)
    : Source(SourceType::Image,
     std::make_unique<ImageSource::Impl>(std::move(id), *this, url, coords)),
     impl(static_cast<Impl*>(baseImpl.get())) {
}

void ImageSource::setCoordinates(const std::vector<LatLng> coords) {
    impl->setCoordinates(coords);
}

void ImageSource::setURL(const std::string& url) {
    impl->setURL(url);
}

const std::string& ImageSource::getURL() const {
     return impl->getURL();
}

} // namespace style
} // namespace mbgl
