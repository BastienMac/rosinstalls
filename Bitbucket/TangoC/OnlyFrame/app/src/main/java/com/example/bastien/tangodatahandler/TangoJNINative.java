package com.example.bastien.tangodatahandler;

import android.os.IBinder;
import android.util.Log;

import com.projecttango.examples.cpp.util.TangoInitializationHelper;

/**
 * Created by bastien on 16/05/16.
 */
public class TangoJNINative {
    // loading native helloc module/lib
    static {
        try {
            System.loadLibrary("cpp_tango_data_handler");
            if (TangoInitializationHelper.loadTangoSharedLibrary() ==
                    TangoInitializationHelper.ARCH_ERROR) {
                Log.e("TangoJNINative", "ERROR! Unable to load libtango_client_api.so!");
            }
        } catch (UnsatisfiedLinkError ule) {
            Log.e("HelloC", "WARNING: Could not load native library: " + ule.getMessage());
        }
    }

    // Check that the installed version of the Tango API is up to date.
    //
    // @return returns true if the application version is compatible with the
    //         Tango Core version.
    public static native boolean checkTangoVersion(MainActivity activity,
                                                   int minTangoVersion);

    // Called when Tango Service is connected successfully.
    public static native boolean onTangoServiceConnected(IBinder binder);

    // Setup the configuration file of the Tango Service. We are also setting up
    // the auto-recovery option from here.
    public static native int setupConfig();

    // Connect the onPoseAvailable callback.
    public static native int connectCallbacks();

    // Connect to the Tango Service.
    // This function will start the Tango Service pipeline, in this case, it will
    // start Motion Tracking and Depth sensing.
    public static native boolean connect();

    // Disconnect from the Tango Service, release all the resources that the app is
    // holding from the Tango Service.
    public static native void disconnect();

    // Get the latest pose string from our application for display in our debug UI.
    public static native String getPoseString();

    // Get the latest point cloud string from our application for display in our debug UI.
    public static native String getPointCloudString();

    // Get the latest picture string from our application for display in our debug UI.
    public static native String getPictureString();

    // Get the latest event string from our application for display in our debug UI.
    public static native String getEventString();

    public static native String getTest();

    ////////////////TEST
    // Connect the onFrameAvailable callback.
    public static native int connectOnFrameAvailableCallbackB();

    // Setup the configuration file of the Tango Service. We are also setting up
    // the auto-recovery option from here.
    public static native int setupConfigB();
}
