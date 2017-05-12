#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>
#include <mbgl/util/geo.hpp>

namespace mbgl {
namespace style {

ImageSource::ImageSource(std::string id, const std::vector<LatLng> coords)
    : Source(SourceType::Image,
     std::make_unique<ImageSource::Impl>(std::move(id), *this, coords)),
     impl(static_cast<Impl*>(baseImpl.get())) {
}

void ImageSource::setCoordinates(const std::vector<LatLng> coords) {
    impl->setCoordinates(coords);
}

void ImageSource::setURL(const std::string& url) {
    impl->setURL(url);
}

void ImageSource::setImage(mbgl::UnassociatedImage image) {
    impl->setImage(std::move(image));
}

const std::string& ImageSource::getURL() const {
     return impl->getURL();
}

} // namespace style
} // namespace mbgl
