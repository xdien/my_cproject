package sliding;

import android.app.Fragment;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;

import com.crazy.xdien.imageedit.CrazyActivity;
import com.crazy.xdien.imageedit.R;


import org.opencv.android.Utils;
import org.opencv.core.Mat;


import static org.opencv.imgproc.Imgproc.cvtColor;
import static org.opencv.imgproc.Imgproc.threshold;

/**
 * Created by xdien on 10/6/14.
 */
public class ThresholdFragment extends Fragment {
    private SeekBar ThresholdValue,ThresholdType;
    private int value=0, type=0;
    private Mat src,src_gray,dst;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             final Bundle savedInstanceState) {

        final View rootView = inflater.inflate(R.layout.fragment_threshold, container, false);
        //lay bitmap tu cache chuyen thanh MAT
        src = new Mat();
        src_gray = new Mat();
        dst = new Mat();
        Utils.bitmapToMat(CrazyActivity.thumbnail,src);
        cvtColor(src, src_gray, 7);// CV_RGB2GRAY =7

        ThresholdValue = (SeekBar) rootView.findViewById(R.id.seekBar_threshold_value);
        ThresholdType = (SeekBar) rootView.findViewById(R.id.seekBar_thresholdType);
        //type
        ThresholdType.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                type = i;
                threshold(src_gray,dst,value,255,type);
                Utils.matToBitmap(dst, CrazyActivity.thumbnail);
                Log.w("value: ",String.valueOf(value));
                Log.w("Type: ",String.valueOf(type));
                CrazyActivity.main_ImageView.invalidate();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                saveBitmap();
            }
        });
        //Value
        ThresholdValue.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                value = i;
                threshold(src_gray,dst,value,255,type);
                Utils.matToBitmap(dst, CrazyActivity.thumbnail);
                Log.w("value: ",String.valueOf(value));
                Log.w("Type: ",String.valueOf(type));
                CrazyActivity.main_ImageView.invalidate();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                saveBitmap();
            }
        });
        return rootView;
    }
    private static void saveBitmap(){
        CrazyActivity.bmCache.putBitmap("threshold"+ CrazyActivity.picturePath, CrazyActivity.thumbnail);
    }

    /*Ham tach nguong*/

}
