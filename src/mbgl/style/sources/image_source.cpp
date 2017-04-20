#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/style/sources/image_source_impl.hpp>

namespace mbgl {
    namespace style {
        
        ImageSource::ImageSource(std::string id, const std::string& url)
        : Source(SourceType::Image,
                 std::make_unique<ImageSource::Impl>(std::move(id), *this, url)),
        impl(static_cast<Impl*>(baseImpl.get())) {
        }
        
        void ImageSource::setURL(const std::string& url) {
            impl->setURL(url);
        }
        
        const std::string& ImageSource::getURL() const {
             return impl->getURL();
        }
        
    } // namespace style
} // namespace mbgl
