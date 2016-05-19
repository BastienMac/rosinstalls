package com.example.bastien.tangodatahandler;

import android.app.Activity;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.Display;
import android.widget.Toast;

import com.projecttango.examples.cpp.util.TangoInitializationHelper;

public class MainActivity extends Activity{
    // The minimum Tango Core version required from this application.
    private static final int  MIN_TANGO_CORE_VERSION = 9377;

    // Tag for debug logging.
    private static final String TAG = MainActivity.class.getSimpleName();

    // The interval at which we'll update our UI debug text in milliseconds.
    // This is the rate at which we query our native wrapper around the tango
    // service for pose and event information.
    private static final int UPDATE_UI_INTERVAL_MS = 100;

    // Screen size for normalizing the touch input for orbiting the render camera.
    private Point mScreenSize = new Point();

    // Handles the debug text UI update loop.
    private Handler mHandler = new Handler();


    // Tango Service connection.
    ServiceConnection mTangoServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName name, IBinder service) {
            TangoJNINative.onTangoServiceConnected(service);

            // Setup the configuration for the TangoService.
            TangoJNINative.setupConfig();

            // Connect the onPoseAvailable callback.
            TangoJNINative.connectCallbacks();

            // Connect to Tango Service (returns true on success).
            // Starts Motion Tracking and Area Learning.
            if (!TangoJNINative.connect()) {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        // End the activity and let the user know something went wrong.
                        Toast.makeText(MainActivity.this, "Connect Tango Service Error",
                                Toast.LENGTH_LONG).show();
                        finish();
                        return;
                    }
                });
            }
        }

        public void onServiceDisconnected(ComponentName name) {
            // Handle this if you need to gracefully shutdown/retry
            // in the event that Tango itself crashes/gets upgraded while running.
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Query screen size, the screen size is used for computing the normalized
        // touch point.
        Display display = getWindowManager().getDefaultDisplay();
        display.getSize(mScreenSize);

        // Setting content view of this activity.
        setContentView(R.layout.activity_main);

        // Check if the Tango Core is out of date.
        if (!TangoJNINative.checkTangoVersion(this, MIN_TANGO_CORE_VERSION)) {
            Toast.makeText(this, "Tango Core out of date, please update in Play Store",
                    Toast.LENGTH_LONG).show();
            finish();
            return;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        TangoInitializationHelper.bindTangoService(this, mTangoServiceConnection);

        // Start the debug text UI update loop.
        mHandler.post(mUpdateUiLoopRunnable);
    }

    @Override
    protected void onPause() {
        super.onPause();

        // Stop the debug text UI update loop.
        mHandler.removeCallbacksAndMessages(null);

        // Disconnect from Tango Service, release all the resources that the app is
        // holding from Tango Service.
        TangoJNINative.disconnect();
        unbindService(mTangoServiceConnection);
    }

    // Debug text UI update loop, updating at 10Hz.
    private Runnable mUpdateUiLoopRunnable = new Runnable() {
        public void run() {
            updateUi();
            mHandler.postDelayed(this, UPDATE_UI_INTERVAL_MS);
        }
    };

    // Update the debug text UI.
    private void updateUi() {
        try {
            //Log.d("Position : ", "======PPP=====>>>>>>>>" + TangoJNINative.getPoseString() + "<<<<<<<<<<======PPP=====");
            Log.d("Point Cloud : ", "======CCC=====>>>>>>>>" + TangoJNINative.getPointCloudString() + "<<<<<<<<<<======CCC=====");
            //Log.d("Frame : ", "======FFF=====>>>>>>>>" + TangoJNINative.getPictureString() + "<<<<<<<<<<======FFF=====");
        } catch (Exception e) {
            e.printStackTrace();
            Log.e(TAG, "Exception updateing UI elements");
        }
    }
}
