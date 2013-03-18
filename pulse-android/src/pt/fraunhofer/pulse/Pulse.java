package pt.fraunhofer.pulse;

import org.opencv.core.Mat;

public class Pulse {

    public Pulse() {
        self = _initialize();
    }

    public void load(String filename) {
        _load(self, filename);
    }
    
    public void start(int width, int height) {
        _start(self, width, height);
    }

    public void onFrame(Mat frame) {
        _onFrame(self, frame.getNativeObjAddr());
    }
    
    public void release() {
        _destroy(self);
        self = 0;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        release();
    }

    private long self = 0;
    private static native long _initialize();
    private static native void _load(long self, String filename);
    private static native void _start(long self, int width, int height);
    private static native void _onFrame(long self, long frame);
    private static native void _destroy(long self);
}
