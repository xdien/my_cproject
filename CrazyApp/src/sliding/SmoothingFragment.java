package sliding;

import android.app.Fragment;
import android.graphics.Bitmap;
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
import org.opencv.core.Size;

import static org.opencv.imgproc.Imgproc.GaussianBlur;

/**
 * Created by xdien on 10/2/14.
 */
public class SmoothingFragment extends Fragment {
    SeekBar numColumnsControl;
    private Bitmap bm;
    private Mat bmSrc,bmDst;
    private Bitmap bmResult;
    private String picturePath;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        picturePath = CrazyActivity.picturePath;
        bmSrc = new Mat();
        //bmSrc = Highgui.imread(picturePath);
        bmDst = new Mat();
        bm = CrazyActivity.bmCache.getBitmap(picturePath);
        if(bm != null)
        {
            Utils.bitmapToMat(bm, bmSrc);
        }
        else
        {
            Log.e("Load image from cache","fail..");
        }
        View rootView = inflater.inflate(R.layout.fragment_smoothing, container, false);

        numColumnsControl = (SeekBar) rootView.findViewById(R.id.smoothing_seekBar);
        numColumnsControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                Log.i("seeking value: ",String.valueOf(i));
                bmDst = bmSrc.clone();
                img_smoothing(bmSrc,bmDst,i);
                }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                //Chuyen thanh bitmap va luu vao cache
                bmResult = Bitmap.createBitmap(bmDst.cols(), bmDst.rows(), Bitmap.Config.ARGB_8888);
                Utils.matToBitmap(bmDst,bmResult);
                CrazyActivity.bmCache.putBitmap("smoothing"+picturePath,bmResult);
                CrazyActivity.upDisplayImageView("smoothing" + picturePath);
            }
        });
        //  builder.setView(menuView);
        return rootView;
    }
    private static void img_smoothing(Mat src,Mat dst,int MAX_KERNEL_LENGTH)
    {
        for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
        {
            GaussianBlur( src, dst, new Size( i, i ), 0, 0 );
            //medianBlur ( src, dst, i );

        }
    }


}
