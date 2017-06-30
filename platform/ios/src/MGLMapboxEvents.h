#import <Foundation/Foundation.h>

#import "MGLTypes.h"

NS_ASSUME_NONNULL_BEGIN

typedef NS_DICTIONARY_OF(NSString *, id) MGLMapboxEventAttributes;
typedef NS_MUTABLE_DICTIONARY_OF(NSString *, id) MGLMutableMapboxEventAttributes;

@interface MGLMapboxEvents : NSObject

+ (nullable instancetype)sharedManager;
+ (void)ensureMetricsOptoutExists;

@end

NS_ASSUME_NONNULL_END
