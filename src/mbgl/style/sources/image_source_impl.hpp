#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/image_source.hpp>
#include <mbgl/util/image.hpp>

namespace mbgl {

class AsyncRequest;
class RenderSource;

namespace style {

class ImageSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, const std::string& url, const std::vector<LatLng>& coords);

    ~Impl() final;

    void setImage(mbgl::UnassociatedImage);

    void setURL(std::string);
    const std::string& getURL() const;
    void setCoordinates(const std::vector<LatLng> coords);
    std::vector<LatLng> getCoordinates() const;

    const mbgl::UnassociatedImage& getData() const;

    void loadDescription(FileSource&) final;

    std::unique_ptr<RenderSource> createRenderSource() const final;

private:
    std::string url;
    std::vector<LatLng> coords;
    std::unique_ptr<AsyncRequest> req;
    mbgl::UnassociatedImage image;
};

} // namespace style
} // namespace mbgl
