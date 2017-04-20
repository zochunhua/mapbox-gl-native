#include <mbgl/util/logging.hpp>
#include <mbgl/storage/file_source.hpp>
#include <mbgl/style/source_observer.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

#include <sstream>

namespace mbgl {
namespace style {

ImageSource::Impl::Impl(std::string id_, Source& base_, const std::string& url)
    : Source::Impl(SourceType::Image, std::move(id_), base_), url(url) {
}

ImageSource::Impl::~Impl() = default;

void ImageSource::Impl::setURL(std::string url_) {
    url = std::move(url_);
}

const std::string& ImageSource::Impl::getURL() const {
    return url;
}

void ImageSource::Impl::loadDescription(FileSource&) {
}

optional<Range<uint8_t>> ImageSource::Impl::getZoomRange() const {
    //TODO: AHM
    return {};
}

std::unique_ptr<Tile> ImageSource::Impl::createTile(const OverscaledTileID& ,
                                                      const UpdateParameters& ) {
    //TODO: AHM
    assert(loaded);
    return {};
}

} // namespace style
} // namespace mbgl
