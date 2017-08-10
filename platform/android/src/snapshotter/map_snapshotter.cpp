#include "map_snapshotter.hpp"

#include <mbgl/renderer/renderer.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/shared_thread_pool.hpp>
#include <mbgl/util/logging.hpp>

#include "../attach_env.hpp"
#include "../bitmap.hpp"
#include "snapshotter_renderer_frontend.hpp"

namespace mbgl {
namespace android {

MapSnapshotter::MapSnapshotter(jni::JNIEnv& _env,
                               jni::Object<MapSnapshotter> _obj,
                               jni::Object<FileSource> jFileSource,
                               jni::jfloat _pixelRatio,
                               jni::jint width,
                               jni::jint height,
                               jni::String styleURL,
                               jni::String _programCacheDir)
        : javaPeer(_obj.NewWeakGlobalRef(_env))
        , pixelRatio(_pixelRatio)
        , threadPool(sharedThreadPool()) {

    // Get a reference to the JavaVM for callbacks
    if (_env.GetJavaVM(&vm) < 0) {
        _env.ExceptionDescribe();
        return;
    }

    auto& fileSource = mbgl::android::FileSource::getDefaultFileSource(_env, jFileSource);
    auto size = mbgl::Size { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    // Create a renderer frontend
    rendererFrontend = std::make_unique<SnapshotterRendererFrontend>(pixelRatio,
                                                                     size,
                                                                     fileSource,
                                                                     *threadPool,
                                                                     jni::Make<std::string>(_env,
                                                                                        _programCacheDir));

    // Create the core map
    map = std::make_unique<mbgl::Map>(*rendererFrontend,
                                      *this,
                                      size,
                                      pixelRatio,
                                      fileSource,
                                      *threadPool,
                                      MapMode::Still,
                                      ConstrainMode::HeightOnly,
                                      ViewportMode::Default);

    map->getStyle().loadURL(jni::Make<std::string>(_env, styleURL));
}

void MapSnapshotter::start(JNIEnv&) {
    map->renderStill([this](std::exception_ptr err) {
        if (err) {
            Log::Error(Event::JNI, "Still image error");
        } else {
            Log::Info(Event::JNI, "Still image ready");
            auto image = rendererFrontend->getImage();

            android::UniqueEnv _env = android::AttachEnv();
            auto bitmap = Bitmap::CreateBitmap(*_env, std::move(image));
            (void) bitmap;

            // invoke callback
            static auto onSnapshotReady = javaClass.GetMethod<void (jni::Object<Bitmap>)>(*_env, "onSnapshotReady");
            javaPeer->Call(*_env, onSnapshotReady, bitmap);
        }
    });
}


// Static methods //

jni::Class<MapSnapshotter> MapSnapshotter::javaClass;

void MapSnapshotter::registerNative(jni::JNIEnv& env) {
    // Lookup the class
    MapSnapshotter::javaClass = *jni::Class<MapSnapshotter>::Find(env).NewGlobalRef(env).release();

#define METHOD(MethodPtr, name) jni::MakeNativePeerMethod<decltype(MethodPtr), (MethodPtr)>(name)

    // Register the peer
    jni::RegisterNativePeer<MapSnapshotter>(env, MapSnapshotter::javaClass, "nativePtr",
                                           std::make_unique<MapSnapshotter, JNIEnv&, jni::Object<MapSnapshotter>, jni::Object<FileSource>, jni::jfloat, jni::jint, jni::jint, jni::String, jni::String>,
                                           "nativeInitialize",
                                           "nativeDestroy",
                                            METHOD(&MapSnapshotter::start, "nativeStart")
    );
}

} // namespace android
} // namespace mbgl