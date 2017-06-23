#import "MBXAppDelegate.h"
#import "MBXViewController.h"
#import <Mapbox/Mapbox.h>

NSString * const MBXMapboxAccessTokenDefaultsKey = @"MBXMapboxAccessToken";

@implementation MBXAppDelegate

+ (void)load {
    // TELEM_TODO: Remove these
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"MGLMapboxMetricsDebugLoggingEnabled"];
    [[NSUserDefaults standardUserDefaults] setBool:YES forKey:@"MGLMapboxMetricsEnabledInSimulator"];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    // Set access token, unless MGLAccountManager already read it in from Info.plist.
    if ( ! [MGLAccountManager accessToken]) {
        NSString *accessToken = [[NSProcessInfo processInfo] environment][@"MAPBOX_ACCESS_TOKEN"];
        if (accessToken) {
            // Store to preferences so that we can launch the app later on without having to specify
            // token.
            [[NSUserDefaults standardUserDefaults] setObject:accessToken forKey:MBXMapboxAccessTokenDefaultsKey];
        } else {
            // Try to retrieve from preferences, maybe we've stored them there previously and can reuse
            // the token.
            accessToken = [[NSUserDefaults standardUserDefaults] objectForKey:MBXMapboxAccessTokenDefaultsKey];
        }
        [MGLAccountManager setAccessToken:accessToken];
    }
    
    return YES;
}

@end
