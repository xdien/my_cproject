package com.crazy.xdien.imageedit.sliding.ImageCache;

/**
 * Created by xdien on 10/23/14.
 */
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.GridView;
import android.widget.ImageView;

import java.util.ArrayList;

/*public class ImageAdapter extends BaseAdapter {
    private Context mContext;

    // Keep all Images in array
    public Integer[] mThumbIds = {
            R.drawable.pic_1, R.drawable.pic_2,
            R.drawable.pic_3, R.drawable.pic_4,
            R.drawable.pic_5, R.drawable.pic_6,
            R.drawable.pic_7, R.drawable.pic_8,
            R.drawable.pic_9, R.drawable.pic_10,
            R.drawable.pic_10, R.drawable.pic_10,
            R.drawable.pic_10, R.drawable.pic_10,
            R.drawable.pic_10
    };

    // Constructor
    public ImageAdapter(Context c){
        mContext = c;
    }

    @Override
    public int getCount() {
        return mThumbIds.length;
    }

    @Override
    public Object getItem(int position) {
        return mThumbIds[position];
    }

    @Override
    public long getItemId(int position) {
        return 0;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        ImageView imageView = new ImageView(mContext);
        imageView.setImageResource(mThumbIds[position]);
        imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
        imageView.setLayoutParams(new GridView.LayoutParams(70, 70));
        return imageView;
    }

}*/
public class GridViewAdapter extends BaseAdapter {
    private Context mContext;
    private Integer[] pics;
    private Bitmap[] bitmaps = new Bitmap[] {null,null,null,null,null};

    public GridViewAdapter(Context c) {
        mContext = c;
    }

    public int getCount() {
        return bitmaps.length;
    }

    public Object getItem(int position) {return null;}

    public long getItemId(int position) {return 0;}

    // create a new ImageView for each item referenced by the Adapter
    public View getView(int position, View convertView, ViewGroup parent) {
        ImageView imageView;
        if (convertView == null) {  // if it's not recycled, initialize some attributes
            imageView = new ImageView(mContext);
            imageView.setLayoutParams(new GridView.LayoutParams(150, 150));
            imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
            imageView.setPadding(8, 8, 8, 8);
            imageView.setHorizontalScrollBarEnabled(true);
        } else {
            imageView = (ImageView) convertView;
        }
        imageView.setImageBitmap(bitmaps[position]);
        return imageView;
    }

    public void AddImages(Bitmap src, int id){
        Bitmap bm = Bitmap.createScaledBitmap(src,100,100,true);
        try {
            bitmaps[id] = bm;
        }
        catch (Exception e)
        {
            Log.e("loi gi!!", "Khong biet :)) hi vong van chay");
        }
    }

}
