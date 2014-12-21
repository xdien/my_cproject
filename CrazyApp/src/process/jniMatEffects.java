package process;

import android.app.Activity;
import android.util.Log;


import org.opencv.core.Mat;

/**
 * Created by xdien on 10/25/14.
 */
public class jniMatEffects{
    static {
        System.loadLibrary("jniOpenCV");
    }
    public void jniMatEffects(){
        //Log.w("tu class:",getStringFromNative());
    }

    //su dung .getNativeObjAddr() truyen va nhan doi tuong
    public static native void Smoothing(long addrMatSrc, long addrMatDst);
    public static native void getStringFromNative();
}
