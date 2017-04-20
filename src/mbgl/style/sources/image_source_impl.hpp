#pragma once

#include <mbgl/style/source_impl.hpp>
#include <mbgl/style/sources/image_source.hpp>

namespace mbgl {

class AsyncRequest;

namespace style {

class ImageSource::Impl : public Source::Impl {
public:
    Impl(std::string id, Source&, const std::string& url);
    ~Impl() final;

    void setURL(std::string);
    const std::string& getURL() const;

    void loadDescription(FileSource&) final;

    uint16_t getTileSize() const final {
        return util::tileSize;
    }

    optional<Range<uint8_t>> getZoomRange() const final;

private:
    std::string url;
    std::unique_ptr<AsyncRequest> req;
    std::unique_ptr<Tile> createTile(const OverscaledTileID& tileID, const UpdateParameters& parameters) final;

};

} // namespace style
} // namespace mbgl
