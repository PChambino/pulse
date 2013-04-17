package pt.fraunhofer.pulse.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.AttributeSet;
import android.view.View;

public class PulseView extends View {

    public PulseView(Context context) {
        super(context);
    }

    public PulseView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public PulseView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        canvas.drawColor(Color.RED);
    }
    
}
