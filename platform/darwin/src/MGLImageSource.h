#import "MGLFoundation.h"
#import "MGLGeometry.h"
#import "MGLTileSource.h"

MGL_EXPORT
@interface MGLImageSource : MGLTileSource

- (instancetype)initWithIdentifier:(NSString *)identifier imageURL:(NSURL *)imageURL coordinateBounds:(MGLCoordinateBounds)coordinateBounds;

@end
