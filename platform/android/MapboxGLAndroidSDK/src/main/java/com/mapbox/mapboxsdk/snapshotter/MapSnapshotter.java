package com.mapbox.mapboxsdk.snapshotter;

import android.content.Context;
import android.graphics.Bitmap;

import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.storage.FileSource;

/**
 * Created by ivo on 10/08/2017.
 */
public class MapSnapshotter {

  // Holds the pointer to JNI NativeMapView
  private long nativePtr = 0;
  private MapboxMap.SnapshotReadyCallback callback;

  public static class Options {
    private int width;
    private int height;
    private String styleUrl;

    public Options(int width, int height, String styleUrl) {
      this.width = width;
      this.height = height;
      this.styleUrl = styleUrl;
    }

  }

  public MapSnapshotter(Context context, Options options) {
    FileSource fileSource = FileSource.getInstance(context);
    float pixelRatio = context.getResources().getDisplayMetrics().density;
    String programCacheDir = context.getCacheDir().getAbsolutePath();

    nativeInitialize(this, fileSource, pixelRatio, options.width, options.height, options.styleUrl, programCacheDir);
  }


  public void start(MapboxMap.SnapshotReadyCallback callback) {
    this.callback = callback;
    nativeStart();
  }

  protected void onSnapshotReady(Bitmap bitmap) {
    callback.onSnapshotReady(bitmap);
  }

  protected native void nativeInitialize(MapSnapshotter mapSnapshotter,
                                         FileSource fileSource, float pixelRatio,
                                         int width, int height,
                                         String styleUrl, String programCacheDir);

  protected native void nativeStart();

  @Override
  protected native void finalize() throws Throwable;
}
