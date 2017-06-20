import XCTest
import Mapbox

class MGLOverlayTests: XCTestCase {
    
    func testCoverage() {
        MGLSDKTestHelpers.checkTestsContainAllMethods(testClass: CustomOverlay.self, in: MGLOverlay.self)
    }
    
}

class CustomOverlay: NSObject, MGLOverlay {

    var coordinate = CLLocationCoordinate2D(latitude: 0, longitude: 0)
    var overlayBounds = MGLCoordinateBounds(sw: CLLocationCoordinate2D(latitude: 0, longitude: 0), ne: CLLocationCoordinate2D(latitude: 0, longitude: 0))
    
    func testCoordinate() {
        XCTAssertNotNil(coordinate)
    }
    
    func testOverlayBounds() {
        XCTAssertNotNil(overlayBounds)
    }
    func intersects(_ overlayBounds: MGLCoordinateBounds) -> Bool { return false }
}
