#pragma once

#include <mbgl/style/source.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {
class LatLng;

namespace style {

class ImageSource : public Source {
public:
    ImageSource(std::string id, const std::string& url, const std::vector<LatLng>);
    
    const std::string& getURL() const;
    void setURL(const std::string& url) ;
    void setImage(mbgl::UnassociatedImage);
    void setCoordinates(const std::vector<LatLng>);
    
    // Private implementation
    class Impl;
    Impl* const impl;
};

template <>
inline bool Source::is<ImageSource>() const {
    return type == SourceType::Vector;
}

} // namespace style
} // namespace mbgl
