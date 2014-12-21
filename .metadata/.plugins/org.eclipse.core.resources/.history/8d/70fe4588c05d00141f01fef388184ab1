package com.crazy.xdien.imageedit.sliding.draw_;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PorterDuff;
import android.graphics.PorterDuffXfermode;
import android.util.Log;


import com.crazy.xdien.imageedit.sliding.CrazyActivity;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;

import static org.opencv.core.Core.addWeighted;
import static org.opencv.core.Core.getTextSize;
import static org.opencv.core.Core.rectangle;
import static org.opencv.imgproc.Imgproc.drawContours;

/**
 * Created by xdien on 10/13/14.
 */
public class drawRect {
    private double[] rgb;
    private Point topLeft,bottomRight,centerText;
    private double inputX,inputY;
    private Mat src,orgSrc;
    private Scalar colorRGB;
    private int thickness;
    public String inputText;
    private Size textSize;
    private int[] baseline;
    private Bitmap bitmap;
    private boolean cache;
    public final int RECTANGLE =10;
    public final int TEXT = 11;
    public int code;
    public drawRect()
    {
        orgSrc = new Mat();
        textSize = new Size();
        baseline = new int[] {0};

        thickness = 2;
        inputText ="";
        //khung vien hinh vuong mat dinh mau xanh
        //khoi dong diem mat dinh
        topLeft = new Point(0,0);
        bottomRight = new Point(0,0);
        centerText = new Point(0,0);
        rgb = new double[] {0,0,255};
        src = new Mat();
        colorRGB = new Scalar(rgb);
        Log.w("Ham khoi dong rect: ","ok?");
    }
    public boolean recCollision(int x, int y){
        if(x >= topLeft.x && x <= bottomRight.x && y >=topLeft.y && y<=bottomRight.y)
            return true;
        else
            return false;
    }
    //ham ve hinh vuong va text
    public void draw(int code){
        if(src != null) {
            if(code == RECTANGLE) {
                calculatorCenterPoint(inputX, inputY);
                Rect rect = new Rect(100,20,100,100);
                src.submat(rect);
                //rectangle(src, topLeft, bottomRight, colorRGB, Core.FILLED);
                //rectangle (img, topLeft, bottomRight, colorRGB, Core.FILLED);

                Utils.matToBitmap(src, CrazyActivity.thumbnail);
                CrazyActivity.main_ImageView.invalidate();
            }
            if(code == TEXT)
            {
                calculatorCenterPoint(inputX, inputY);
                Core.putText(src, inputText, centerText, Core.CMP_LT, 1, colorRGB, thickness);
                Utils.matToBitmap(src, CrazyActivity.thumbnail);
                CrazyActivity.main_ImageView.invalidate();
            }
        }
        else
        {
            Log.w("Nguon Mat la rong:","ban chua setSouce?");
        }
    }
    //set mau
    public void setColor(int redValue, int greenValue, int blueValue)//gia tri la 0-255
    {
        rgb[0] =redValue;
        rgb[1] = greenValue;
        rgb[2] = blueValue;
    }
    public  void setSourceMat(Mat mat){
        src = mat;
        orgSrc = mat;
    }
    private void calculatorCenterPoint(double x, double y){
        centerText.x = (x - (x+x+textSize.width)/2)+x;
        centerText.y = (y - (y-textSize.height+ y)/2)+y;
        Log.w(": ",centerText.toString());
        topLeft.x = centerText.x;
        topLeft.y = centerText.y-textSize.height;
        bottomRight.x = centerText.x + textSize.width;
        bottomRight.y =centerText.y;
    }
    //ham set text
    public void setText(String input){
        inputText = input;
        baseline[0] = baseline[0] + thickness;
        textSize = getTextSize(inputText,Core.CMP_LT,1,2,baseline);
    }
    public void setInputXY(double x,double y){
        inputX = x;
        inputY = y;
    }
    public void moveTo(double x, double y)
    {
        setInputXY(x,y);
        if(cache)
        {
            orgSrc.copyTo(src);
            calculatorCenterPoint(inputX, inputY);
            Core.putText(src, inputText, centerText, Core.CMP_LT, 1,colorRGB, thickness);
            Utils.matToBitmap(src, CrazyActivity.thumbnail);
            CrazyActivity.main_ImageView.invalidate();
            cache = false;
        }
        else{
            bitmap = CrazyActivity.bmCache.getBitmap("rect");
            Utils.bitmapToMat(bitmap,orgSrc);
            cache = true;
        }
    }
    public boolean isRect()
    {
        if (code == RECTANGLE)
            return true;
        else
            return false;
    }
    public void setDraw(int incode){
        code = incode;
    }
    public void setPointRect(double tfx,double tfy, double brx, double bry)
    {
        topLeft.x = tfx;
        topLeft.y = tfy;
        bottomRight.x = brx;
        bottomRight.y = bry;
    }
    public void drawrectangle()
    {
        orgSrc.copyTo(src);
        rectangle(src, topLeft, bottomRight, colorRGB, thickness);
        //Rect rect = new Rect(10,20,100,100);
        //double alpha = 0.3;
        //Mat roi = new Mat(src,rect);
        //Mat color = new Mat(roi.size(),  CvType.CV_8UC3, new Scalar(0, 125, 125));
        //addWeighted(color, alpha, roi, 1.0 - alpha , 0.0,roi);
        Utils.matToBitmap(src, CrazyActivity.thumbnail);
        CrazyActivity.main_ImageView.invalidate();
    }
    public  void invalidate()
    {
        //orgSrc = src.clone();
        orgSrc.copyTo(src);
        //Utils
    }
    private Bitmap punchAHoleInABitmap(Bitmap foreground) {
        Bitmap bitmap = Bitmap.createBitmap(foreground.getWidth(), foreground.getHeight(), Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);
        Paint paint = new Paint();
        canvas.drawBitmap(foreground, 0, 0, paint);
        paint.setAntiAlias(true);
        paint.setXfermode(new PorterDuffXfermode(PorterDuff.Mode.CLEAR));
        //float radius = (float)(getScreenSize().x *.35);
        //float x = (float) ((getScreenSize().x*.5) + (radius * .5));
        //float y = (float)  ((getScreenSize().y*.5) + (radius * .5));
        //canvas.drawCircle(x, y, radius, paint);
        return bitmap;
    }
}
