package sliding;

/**
 * Created by xdien on 9/26/14.
 */



import com.crazy.xdien.imageedit.R;

import android.app.Fragment;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Matrix;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;


public class HomeFragment extends Fragment implements View.OnTouchListener {

    public HomeFragment(){}
    ImageView imageView;
    Bitmap bitmap;
    Canvas canvas;
    Paint paint;
    float downx = 0,downy = 0,upx = 0,upy = 0;
    //khai bao lay toa do 1 diem;
    float eventX;
    float eventY;
    float[] eventXY;
    Matrix invertMatrix;
    int y,x;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {

        final View rootView = inflater.inflate(R.layout.fragment_home, container, false);
        //imageView = (ImageViewTouch) rootView.findViewById(R.id.main_imageView);
        invertMatrix = new Matrix();

        /*rootView.setOnTouchListener(new View.OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                eventX = event.getX();
                eventY = event.getY();
                eventXY = new float[] {eventX, eventY};
                MainActivity.main_ImageView.getImageMatrix().invert(invertMatrix);

                invertMatrix.mapPoints(eventXY);
                x = Integer.valueOf((int)eventXY[0]);
                y = Integer.valueOf((int)eventXY[1]);
                if (event.getAction() == MotionEvent.ACTION_MOVE) {
                    Log.i("Touch main_ImageView coordinates", String.valueOf(x)+"x"+String.valueOf(y));
                }
                return true;
            }
        });*/
        return rootView;
    }
    public boolean onTouch(View v, MotionEvent event) {
        int action = event.getAction();
        //Log.i("Touch main_ImageView coordinates", String.valueOf(MainActivity.main_ImageView.getTop()));
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                downx = event.getX();
                downy = event.getY();
                break;
            case MotionEvent.ACTION_MOVE:
                upx = event.getX();
                upy = event.getY();
                canvas.drawLine(downx, downy, upx, upy, paint);
                imageView.invalidate();
                downx = upx;
                downy = upy;
                break;
            case MotionEvent.ACTION_UP:
                upx = event.getX();
                upy = event.getY();
                canvas.drawLine(downx, downy, upx, upy, paint);
                imageView.invalidate();
                break;
            case MotionEvent.ACTION_CANCEL:
                break;
            default:
                break;
        }
        return true;
    }
}