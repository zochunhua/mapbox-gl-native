#import "MGLMapboxEvents.h"
#import <MapboxMobileEvents/MapboxMobileEvents.h>

#import "NSProcessInfo+MGLAdditions.h"

@interface MGLMapboxEvents ()

@property (nonatomic, copy) NSString *dateForDebugLogFile;
@property (nonatomic) BOOL canEnableDebugLogging;
@property (nonatomic, getter=isPaused) BOOL paused;
@property (nonatomic) NS_MUTABLE_ARRAY_OF(MGLMapboxEventAttributes *) *eventQueue;
@property (nonatomic) dispatch_queue_t serialQueue;
@property (nonatomic) dispatch_queue_t debugLogSerialQueue;

@end

@implementation MGLMapboxEvents {
//    UIBackgroundTaskIdentifier _backgroundTaskIdentifier;
}

+ (void)initialize {
    if (self == [MGLMapboxEvents class]) {
        NSBundle *bundle = [NSBundle mainBundle];
        NSNumber *accountTypeNumber = [bundle objectForInfoDictionaryKey:@"MGLMapboxAccountType"];
        [[NSUserDefaults standardUserDefaults] registerDefaults:@{@"MGLMapboxAccountType": accountTypeNumber ?: @0,
                                                                  @"MGLMapboxMetricsEnabled": @YES}];
    }
}

- (instancetype) init {
    self = [super init];
    if (self) {
//        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
//        _serialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.serial", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);

        // Events Control
        _eventQueue = [[NSMutableArray alloc] init];

        // Configure logging
        if ([self isProbablyAppStoreBuild]) {
            self.canEnableDebugLogging = NO;

            if ([[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsDebugLoggingEnabled"]) {
                NSLog(@"Telemetry logging is only enabled in non-app store builds.");
            }
        } else {
            self.canEnableDebugLogging = YES;
        }

        // Watch for changes to telemetry settings by the user
        // TELEM_TODO: set the telem instance account and enabled settings based on MGL specific user default values
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(userDefaultsDidChange:) name:NSUserDefaultsDidChangeNotification object:nil];
    }
    return self;
}

// Called implicitly from any public class convenience methods.
// May return nil if this feature is disabled.
//
+ (nullable instancetype)sharedManager {
    if (NSProcessInfo.processInfo.mgl_isInterfaceBuilderDesignablesAgent) {
        return nil;
    }
    static dispatch_once_t onceToken;
    static MGLMapboxEvents *_sharedManager;
    dispatch_once(&onceToken, ^{
        _sharedManager = [[self alloc] init];
    });
    return _sharedManager;
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)userDefaultsDidChange:(NSNotification *)notification {
    dispatch_async(dispatch_get_main_queue(), ^{
        BOOL metricsEnabled = [[NSUserDefaults standardUserDefaults] boolForKey:@"MGLMapboxMetricsEnabled"];
        NSUInteger accountType = [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"];
        [MMEEventsManager sharedManager].metricsEnabled = metricsEnabled;
        [MMEEventsManager sharedManager].accountType = accountType;
        [[MMEEventsManager sharedManager] pauseOrResumeMetricsCollectionIfRequired];
    });
}

+ (void)ensureMetricsOptoutExists {
    NSNumber *shownInAppNumber = [[NSBundle mainBundle] objectForInfoDictionaryKey:@"MGLMapboxMetricsEnabledSettingShownInApp"];
    BOOL metricsEnabledSettingShownInAppFlag = [shownInAppNumber boolValue];

    if (!metricsEnabledSettingShownInAppFlag &&
        [[NSUserDefaults standardUserDefaults] integerForKey:@"MGLMapboxAccountType"] == 0) {
        // Opt-out is not configured in UI, so check for Settings.bundle
        id defaultEnabledValue;
        NSString *appSettingsBundle = [[NSBundle mainBundle] pathForResource:@"Settings" ofType:@"bundle"];

        if (appSettingsBundle) {
            // Dynamic Settings.bundle loading based on http://stackoverflow.com/a/510329/2094275
            NSDictionary *settings = [NSDictionary dictionaryWithContentsOfFile:[appSettingsBundle stringByAppendingPathComponent:@"Root.plist"]];
            NSArray *preferences = settings[@"PreferenceSpecifiers"];
            for (NSDictionary *prefSpecification in preferences) {
                if ([prefSpecification[@"Key"] isEqualToString:@"MGLMapboxMetricsEnabled"]) {
                    defaultEnabledValue = prefSpecification[@"DefaultValue"];
                }
            }
        }

        if (!defaultEnabledValue) {
            [NSException raise:@"Telemetry opt-out missing" format:
             @"End users must be able to opt out of Mapbox Telemetry in your app, either inside Settings (via Settings.bundle) or inside this app. "
             @"By default, this opt-out control is included as a menu item in the attribution action sheet. "
             @"If you reimplement the opt-out control inside this app, disable this assertion by setting MGLMapboxMetricsEnabledSettingShownInApp to YES in Info.plist."
             @"\n\nSee https://www.mapbox.com/ios-sdk/#telemetry_opt_out for more information."
             @"\n\nAdditionally, by hiding this attribution control you agree to display the required attribution elsewhere in this app."];
        }
    }
}

#pragma mark MGLMapboxEvents Debug

//- (void)pushDebugEvent:(NSString *)event withAttributes:(MGLMapboxEventAttributes *)attributeDictionary {
//    if (![self debugLoggingEnabled]) {
//        return;
//    }
//
//    if (!event) {
//        return;
//    }
//
//    MGLMutableMapboxEventAttributes *evt = [MGLMutableMapboxEventAttributes dictionaryWithDictionary:attributeDictionary];
//    [evt setObject:event forKey:@"event"];
//    [evt setObject:[self.rfc3339DateFormatter stringFromDate:[NSDate date]] forKey:@"created"];
//    [evt setValue:[self applicationState] forKey:@"applicationState"];
//    [evt setValue:@([[self class] isEnabled]) forKey:@"telemetryEnabled"];
//    [evt setObject:self.instanceID forKey:@"instance"];
//
//    MGLMapboxEventAttributes *finalEvent = [NSDictionary dictionaryWithDictionary:evt];
//    [self writeEventToLocalDebugLog:finalEvent];
//}
//
//- (void)writeEventToLocalDebugLog:(MGLMapboxEventAttributes *)event {
//    if (![self debugLoggingEnabled]) {
//        return;
//    }
//
//    NSLog(@"%@", [self stringForDebugEvent:event]);
//
//    if (!self.dateForDebugLogFile) {
//        NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
//        [dateFormatter setDateFormat:@"yyyy'-'MM'-'dd"];
//        [dateFormatter setTimeZone:[NSTimeZone systemTimeZone]];
//        self.dateForDebugLogFile = [dateFormatter stringFromDate:[NSDate date]];
//    }
//
//    if (!self.debugLogSerialQueue) {
//        NSString *uniqueID = [[NSProcessInfo processInfo] globallyUniqueString];
//        self.debugLogSerialQueue = dispatch_queue_create([[NSString stringWithFormat:@"%@.%@.events.debugLog", _appBundleId, uniqueID] UTF8String], DISPATCH_QUEUE_SERIAL);
//    }
//
//    dispatch_async(self.debugLogSerialQueue, ^{
//        if ([NSJSONSerialization isValidJSONObject:event]) {
//            NSData *jsonData = [NSJSONSerialization dataWithJSONObject:event options:NSJSONWritingPrettyPrinted error:nil];
//
//            NSString *jsonString = [[NSString alloc] initWithData:jsonData encoding:NSUTF8StringEncoding];
//            jsonString = [jsonString stringByAppendingString:@",\n"];
//
//            NSString *logFilePath = [[NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) firstObject] stringByAppendingPathComponent:[NSString stringWithFormat:@"telemetry_log-%@.json", self.dateForDebugLogFile]];
//
//            NSFileManager *fileManager = [[NSFileManager alloc] init];
//            if ([fileManager fileExistsAtPath:logFilePath]) {
//                NSFileHandle *fileHandle = [NSFileHandle fileHandleForWritingAtPath:logFilePath];
//                [fileHandle seekToEndOfFile];
//                [fileHandle writeData:[jsonString dataUsingEncoding:NSUTF8StringEncoding]];
//            } else {
//                [fileManager createFileAtPath:logFilePath contents:[jsonString dataUsingEncoding:NSUTF8StringEncoding] attributes:@{ NSFileProtectionKey: NSFileProtectionCompleteUntilFirstUserAuthentication }];
//            }
//        }
//    });
//}
//
//- (NSString *)stringForDebugEvent:(MGLMapboxEventAttributes *)event {
//    // redact potentially sensitive location details from system console log
//    if ([event[@"event"] isEqualToString:MGLEventTypeLocation]) {
//        MGLMutableMapboxEventAttributes *evt = [MGLMutableMapboxEventAttributes dictionaryWithDictionary:event];
//        [evt setObject:@"<redacted>" forKey:@"lat"];
//        [evt setObject:@"<redacted>" forKey:@"lng"];
//        event = evt;
//    }
//
//    return [NSString stringWithFormat:@"Mapbox Telemetry event %@", event];
//}

- (BOOL)isProbablyAppStoreBuild {
#if TARGET_IPHONE_SIMULATOR
    return NO;
#else
    // BugshotKit by Marco Arment https://github.com/marcoarment/BugshotKit/
    // Adapted from https://github.com/blindsightcorp/BSMobileProvision

    NSString *binaryMobileProvision = [NSString stringWithContentsOfFile:[NSBundle.mainBundle pathForResource:@"embedded" ofType:@"mobileprovision"] encoding:NSISOLatin1StringEncoding error:NULL];
    if (!binaryMobileProvision) {
        return YES; // no provision
    }

    NSScanner *scanner = [NSScanner scannerWithString:binaryMobileProvision];
    NSString *plistString;
    if (![scanner scanUpToString:@"<plist" intoString:nil] || ! [scanner scanUpToString:@"</plist>" intoString:&plistString]) {
        return YES; // no XML plist found in provision
    }
    plistString = [plistString stringByAppendingString:@"</plist>"];

    NSData *plistdata_latin1 = [plistString dataUsingEncoding:NSISOLatin1StringEncoding];
    NSError *error = nil;
    NSDictionary *mobileProvision = [NSPropertyListSerialization propertyListWithData:plistdata_latin1 options:NSPropertyListImmutable format:NULL error:&error];
    if (error) {
        return YES; // unknown plist format
    }

    if (!mobileProvision || ! mobileProvision.count) {
        return YES; // no entitlements
    }

    if (mobileProvision[@"ProvisionsAllDevices"]) {
        return NO; // enterprise provisioning
    }

    if (mobileProvision[@"ProvisionedDevices"] && [mobileProvision[@"ProvisionedDevices"] count]) {
        return NO; // development or ad-hoc
    }

    return YES; // expected development/enterprise/ad-hoc entitlements not found
#endif
}

@end
