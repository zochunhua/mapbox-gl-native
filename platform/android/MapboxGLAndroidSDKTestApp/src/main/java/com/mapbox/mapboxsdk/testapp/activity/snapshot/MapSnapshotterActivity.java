package com.mapbox.mapboxsdk.testapp.activity.snapshot;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.ImageView;

import com.mapbox.mapboxsdk.constants.Style;
import com.mapbox.mapboxsdk.maps.MapboxMap;
import com.mapbox.mapboxsdk.snapshotter.MapSnapshotter;
import com.mapbox.mapboxsdk.testapp.R;

import timber.log.Timber;

/**
 * Test activity showing how to use a the {@link com.mapbox.mapboxsdk.snapshotter.MapSnapshotter}
 */
public class MapSnapshotterActivity extends AppCompatActivity {

  private ImageView imageView;
  private MapSnapshotter snapshotter;

  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_map_snapshotter);
    imageView = (ImageView) findViewById(R.id.map_snapshot_image);

    snapshotter = new MapSnapshotter(this, new MapSnapshotter.Options(512, 512 , Style.TRAFFIC_DAY));
    snapshotter.start(new MapboxMap.SnapshotReadyCallback() {
      @Override
      public void onSnapshotReady(Bitmap snapshot) {
        Timber.i("Got the snapshot");
        imageView.setImageBitmap(snapshot);
      }
    });
  }

  @Override
  public void onPause() {
    super.onPause();
    //snapshotter.stop();
  }

}
