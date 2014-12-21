package sliding;

import android.app.Activity;
import android.app.Dialog;
import android.app.Fragment;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Matrix;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;

import com.crazy.xdien.imageedit.CrazyActivity;
import com.crazy.xdien.imageedit.R;

import com.larswerkman.holocolorpicker.ColorPicker;

import draw_.drawRect;

import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.core.Point;

import process.jniMatEffects;


import static org.opencv.core.Core.putText;
import static org.opencv.core.Core.rectangle;

/**
 * Created by xdien on 10/7/14.
 */
public class DrawFragment extends Fragment {

    //khai bao lay toa do 1 diem
    Matrix invertMatrix;
    private Mat src;
    int y, x;
    int[] xy;
    private int action = 0;
    private static boolean isDraw = false,isDrawrect=false;
    private boolean isEnd = true;
    private String inputText;
    private int r,b,g,temp,oldColor;
    private drawRect rect;
    private int code;
    private Point toado;
    private int mStartX = 0;
    private int mStartY = 0;
    private int mEndX = 0;
    private int mEndY = 0;
    private boolean mDrawRect = false;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        final View rootView = inflater.inflate(R.layout.fragment_draw, container, false);
        rect = new drawRect();
        //chuye doi bitmap thanh mat
        Mat vao = new Mat();
        Mat ra = new Mat();
        jniMatEffects jniMatEffect = new jniMatEffects();
        jniMatEffects.Smoothing(vao.getNativeObjAddr(),ra.getNativeObjAddr());
        jniMatEffects.getStringFromNative();

        //MainActivity.thumbnail = null;
        //Bitmap tbm = Bitmap.createBitmap(ra.width(),ra.height(), Bitmap.Config.ARGB_8888);
        //Utils.matToBitmap(ra,tbm);
        //CrazyActivity.main_ImageView.setImageBitmap(tbm);
        CrazyActivity.main_ImageView.invalidate();
        //
        src = new Mat();
        rect.setSourceMat(src);
        Utils.bitmapToMat(CrazyActivity.thumbnail, src);
        //su kien button focus
        ImageButton colorPick = (ImageButton) rootView.findViewById(R.id.imageButton_colorPick);
        colorPick.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.i("Click button pick color:","ok?");
                openColorPicker(getActivity(),oldColor);
            }
        });
        ImageButton drawrect = (ImageButton) rootView.findViewById(R.id.imageButton_rect);
        drawrect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                rect.setDraw(rect.RECTANGLE);
                if (isDrawrect) {
                    CrazyActivity.main_ImageView.setScrollEnabled(isDraw);
                    isDrawrect = false;
                } else {
                    CrazyActivity.main_ImageView.setScrollEnabled(isDraw);
                    Log.w("Khong cho phep keo", "DrawFragment");
                    isDrawrect = true;
                }
            }
        });
        //su kien cho nut insertTex
        ImageButton insetText = (ImageButton) rootView.findViewById(R.id.imageButton_insertText);
        insetText.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (isDraw) {
                    CrazyActivity.main_ImageView.setScrollEnabled(isDraw);
                    isDraw = false;
                } else {
                    CrazyActivity.main_ImageView.setScrollEnabled(isDraw);
                    Log.w("Khong cho phep keo", "DrawFrament");
                    isDraw = true;
                }
            }
        });
        //
        invertMatrix = new Matrix();
        xy = new int[]{0, 0};
        CrazyActivity.main_ImageView.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                action = motionEvent.getAction();
                switch (action) {
                    case MotionEvent.ACTION_UP:
                        //MainActivity.getCoordinate(motionEvent);
                        if(isDrawrect)
                        {
                            rect.invalidate();
                        }

                        if(isDraw) {

                        }
                        break;
                    case MotionEvent.ACTION_DOWN:
                        toado = CrazyActivity.getCoordinate(motionEvent);
                        mDrawRect = false;
                        mStartX = (int) toado.x;
                        mStartY = (int) toado.y;
                        //Log.w("bao nhieu cham? actiondown","dsa");
                        break;
                    case MotionEvent.ACTION_MOVE:
                        toado = CrazyActivity.getCoordinate(motionEvent);
                        if(isDrawrect){
                            if (!mDrawRect || Math.abs(x - mEndX) > 5 || Math.abs(y - mEndY) > 5) {
                                mEndX = (int) toado.x;
                                mEndY = (int) toado.y;
                                rect.setPointRect(Math.min(mStartX, mEndX), Math.min(mStartY, mEndY),
                                        Math.max(mEndX, mStartX), Math.max(mEndY, mStartY));
                                rect.drawrectangle();
                            }
                            mDrawRect = true;
                        }
                        break;
                    default:
                        break;
                }
                return false;
            }
        });

        return rootView;
    }

    private void openColorPicker(final  Activity context, int oldcolor)
    {
        final Dialog dialog = new Dialog(context);
        dialog.setTitle("Chon mau");
        dialog.setContentView(R.layout.dialog_color_pick);
        final Button ok = (Button) dialog.findViewById(R.id.button_okDialogPick);
        final ColorPicker colorPicker = (ColorPicker) dialog.findViewById(R.id.view_color_picker);
        //set lai mau cu da chon;
        if(oldcolor != 0) {
            colorPicker.setColor(oldcolor);
        }
        ok.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //luu gia tri mau duoc chon thanh gia tri int mau rgb
                    oldColor = colorPicker.getColor();
                    r = Color.red(oldColor);
                    g = Color.green(oldColor);
                    b = Color.blue(oldColor);
                    Log.i("New color: ",String.valueOf(oldColor));
                    dialog.dismiss();
            }
        });
        Button huy = (Button) dialog.findViewById(R.id.buttonCenDialogPick);
        huy.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                dialog.dismiss();
            }
        });
        dialog.show();
    }

    // The method that displays the  for text input.
    private void showPopup(final Activity context) {

        // Created a new Dialog
        final Dialog dialog = new Dialog(context);

        // Set the title
        dialog.setTitle("Dialog Title");

        // inflate the layout
        dialog.setContentView(R.layout.popup);

        Button buttonOK = (Button) dialog.findViewById(R.id.buttonOk);
        buttonOK.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText input = (EditText) dialog.findViewById(R.id.editText_input);
                inputText = input.getText().toString();
                rect.setSourceMat(src);
                rect.setInputXY(x, y);
                rect.setText(inputText);
                dialog.dismiss();

            }
        });
        // Display the dialog
        dialog.show();
    }
}
