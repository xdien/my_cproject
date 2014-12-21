package draw_;

/**
 * Created by xdien on 9/29/14.
 */

        import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.text.TextPaint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.R;

import com.crazy.xdien.imageedit.CrazyActivity;


public class DragRectView extends View {

    private Paint mRectPaint;
    private int width;
    private int height;
    float scaleWidth;
    float scaleHeight;
    //khai bao cho do phan giai bit map
    private int bmH;
    private int bmW;
    private int mStartX = 0;
    private int mStartY = 0;
    private int mEndX = 0;
    private int mEndY = 0;
    private int rectW;
    private int rectH,canvasa;
    private float mImgeX,mImgeY;
    private Bitmap bitmap1;
    private boolean mDrawRect = false;
    private TextPaint mTextPaint = null;
    private float ratioImage;
    Matrix matrix;

    private OnUpCallback mCallback = null;

    public interface OnUpCallback {
        void onRectFinished(Rect rect);
    }

    public DragRectView(final Context context) {
        super(context);
        init();
    }

    public DragRectView(final Context context, final AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public DragRectView(final Context context, final AttributeSet attrs, final int defStyle) {
        super(context, attrs, defStyle);
        init();
    }

    /**
     * Sets callback for up
     *
     * @param callback {@link OnUpCallback}
     */
    public void setOnUpCallback(OnUpCallback callback) {
        mCallback = callback;
    }

    /**
     * Inits internal data
     */

    private void init() {
        mRectPaint = new Paint();
        mRectPaint.setColor(getContext().getResources().getColor(R.color.holo_green_light));
        mRectPaint.setStyle(Paint.Style.STROKE);
        mRectPaint.setStrokeWidth(5); // TODO: should take from resources

        mTextPaint = new TextPaint();
        mTextPaint.setColor(getContext().getResources().getColor(R.color.holo_green_light));
        mTextPaint.setTextSize(20);
        bitmap1 = CrazyActivity.bmCache.getBitmap(CrazyActivity.picturePath);
        if(bitmap1 != null) {
            //Log.w("dragRect bitmamap null","canh bao");
            bmH = bitmap1.getHeight();
            bmW = bitmap1.getWidth();

        }
        matrix = new Matrix();
    }

    @Override
    public boolean onTouchEvent(final MotionEvent event) {
        int action = event.getAction();
        // TODO: be aware of multi-touches
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                mDrawRect = false;
                mStartX = (int) event.getX();
                mStartY = (int) event.getY();
                invalidate();
                break;

            case MotionEvent.ACTION_MOVE:
                final int x = (int) event.getX();
                final int y = (int) event.getY();

                if (!mDrawRect || Math.abs(x - mEndX) > 5 || Math.abs(y - mEndY) > 5) {
                    mEndX = x;
                    mEndY = y;
                    invalidate();
                }

                mDrawRect = true;
                break;

            case MotionEvent.ACTION_UP:
                if (mCallback != null) {
                    mCallback.onRectFinished(new Rect(Math.min(mStartX, mEndX), Math.min(mStartY, mEndY),
                            Math.max(mEndX, mStartX), Math.max(mEndY, mStartX)));
                }
                invalidate();
                break;

            default:
                break;
        }

        return true;
    }

    @Override
    protected void onDraw(final Canvas canvas) {
        super.onDraw(canvas);
        if (mDrawRect) {
            canvas.drawRect(Math.min(mStartX, mEndX), Math.min(mStartY, mEndY),
                    Math.max(mEndX, mStartX), Math.max(mEndY, mStartY), mRectPaint);
            width = Math.abs(mStartX - mEndX);
            height = Math.abs(mStartY - mEndY);
            canvas.drawText("  (" + width/*rong*/ + ", " + height + ")",
                    Math.max(mEndX, mStartX), Math.max(mEndY, mStartY), mTextPaint);
            /*1200 480 */
            canvasa = canvas.getWidth();
            ratioImage = ((float)canvasa)/bmW;
            Log.w("ratioImage..",Float.toString(ratioImage));
            rectW = Math.round (width/ratioImage);
            rectH = Math.round(height/ratioImage);
            //tinh diem bat dau cat
            mImgeY =((float) Math.min(mStartY,mEndY))/ratioImage;
            mImgeX = ((float) Math.min(mStartX,mEndX))/ratioImage;
            if(mImgeX+rectW<bmW && mImgeY + rectH < bmH) {
                if (rectH <= bmH
                        && rectW <= bmW) {
                    if (rectH > 0 && rectW > 0) {
                        //Bitmap bitmap = Bitmap.createBitmap(bitmap1, Math.round(mImgeX), Math.round(mImgeY), rectW, rectH, matrix, false);
                        //MainActivity.bmCache.putBitmap("cut2" + MainActivity.picturePath, bitmap);
                    }
                }
            }
            //
        }
    }
}

