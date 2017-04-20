#pragma once

#include <mbgl/style/source.hpp>

namespace mbgl {
namespace style {
        
class ImageSource : public Source {
public:
    ImageSource(std::string id, const std::string& url);
    
    const std::string& getURL() const;
    void setURL(const std::string& url) ;

//    void setCoordinates(const std::vector&);
//    optional<std::vector> getCoordinates() const;

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
