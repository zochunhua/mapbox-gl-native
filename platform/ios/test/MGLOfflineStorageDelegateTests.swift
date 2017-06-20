import XCTest
import Mapbox

class MGLOfflineStorageDelegateTests: XCTestCase {
    
    func testCoverage() {
        MGLSDKTestHelpers.checkTestsContainAllMethods(testClass: MGLOfflineStorageDelegateTests.self, in: MGLOfflineStorageDelegate.self)
    }
    
}

extension MGLOfflineStorageDelegateTests: MGLOfflineStorageDelegate {
    func offlineStorage(_ storage: MGLOfflineStorage, urlForResourceOf kind: MGLResourceKind, with url: URL) -> URL { return URL(string: "https://www.mapbox.com")! }
}
