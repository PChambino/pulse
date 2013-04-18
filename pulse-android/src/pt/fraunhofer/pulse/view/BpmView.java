package pt.fraunhofer.pulse.view;

import android.app.Activity;
import android.content.Context;
import android.util.AttributeSet;
import android.widget.TextView;

public class BpmView extends TextView {

    public BpmView(Context context) {
        super(context);
    }

    public BpmView(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public BpmView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }
    
    public void setBpm(final double bpm) {
        ((Activity)getContext()).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                setText(String.valueOf(Math.round(bpm)));
            }
        });
    }
    
}
