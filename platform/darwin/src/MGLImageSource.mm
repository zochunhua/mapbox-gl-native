#import "MGLImageSource_Private.h"

#import "MGLGeometry_Private.h"
#import "MGLSource_Private.h"
#import "MGLTileSource_Private.h"
#import "NSURL+MGLAdditions.h"

#include <mbgl/style/sources/image_source.hpp>

@interface MGLImageSource ()

@property (nonatomic, readonly) mbgl::style::ImageSource *rawSource;

@end

@implementation MGLImageSource

- (instancetype)initWithIdentifier:(NSString *)identifier imageURL:(NSURL *)imageURL coordinateBounds:(MGLCoordinateBounds)coordinateBounds {

    const auto coordinates = MGLLatLngBoundsFromCoordinateBounds(coordinateBounds);
    std::vector<mbgl::LatLng> coordinatesVector;
    coordinatesVector.reserve(4);
    coordinatesVector.push_back(coordinates.northwest());
    coordinatesVector.push_back(coordinates.northeast());
    coordinatesVector.push_back(coordinates.southeast());
    coordinatesVector.push_back(coordinates.southwest());

    auto source = std::make_unique<mbgl::style::ImageSource>(identifier.UTF8String, coordinatesVector);

    source->setURL(imageURL.mgl_URLByStandardizingScheme.absoluteString.UTF8String);

    return self = [super initWithPendingSource:std::move(source)];
}

- (mbgl::style::ImageSource *)rawSource {
    return (mbgl::style::ImageSource *)super.rawSource;
}

- (NSString *)attributionHTMLString {
    auto attribution = self.rawSource->getAttribution();
    return attribution ? @(attribution->c_str()) : nil;
}

@end
