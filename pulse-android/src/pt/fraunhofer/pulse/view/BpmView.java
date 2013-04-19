package pt.fraunhofer.pulse.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.Gravity;
import android.widget.TextView;

public class BpmView extends TextView {
    
    public BpmView(Context context) {
        super(context);
        init();
    }

    public BpmView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public BpmView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
        init();
    }
    
    private double bpm;
    
    private Paint circlePaint;

    private void init() {
        setNoBpm();
        
        setBackgroundColor(Color.DKGRAY);
        setTextColor(Color.LTGRAY);
        setTextSize(TypedValue.COMPLEX_UNIT_FRACTION_PARENT, 60f);
        setTypeface(Typeface.MONOSPACE);
        setGravity(Gravity.CENTER);
        
        circlePaint = initCirclePaint();
    }
    
    private Paint initCirclePaint() {
        Paint p = new Paint(Paint.ANTI_ALIAS_FLAG);
        p.setColor(Color.RED);
        p.setStyle(Paint.Style.FILL);
        return p;
    }
    
    public double getBpm() {
        return bpm;
    }
    
    public void setBpm(double bpm) {
        this.bpm = bpm;
        long rounded = Math.round(bpm);
        if (rounded == 0) {
            setText("-");
        } else {
            setText(String.valueOf(rounded));
        }
    }
    
    public void setNoBpm() {
        setBpm(0);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawCircle(20, getHeight() / 2f, 10, circlePaint);
    }
    
}
