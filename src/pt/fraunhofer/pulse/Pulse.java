package pt.fraunhofer.pulse;

import org.opencv.core.Mat;
import org.opencv.core.MatOfDouble;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;

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

    public Face[] getFaces() {
        int count = _facesCount(self);
        Face[] faces = new Face[count];
        for (int i = 0; i < count; i++) {
            faces[i] = new Face(_face(self, i));
        }
        return faces;
    }

    public double getRelativeMinFaceSize() {
        return _relativeMinFaceSize(self);
    }

    public int getMaxSignalSize() {
        return _maxSignalSize(self);
    }

    public boolean hasMagnification() {
        return _magnification(self);
    }

    public void setMagnification(boolean m) {
        _magnification(self, m);
    }

    public int getMagnificationFactor() {
        return _magnificationFactor(self);
    }

    public void setMagnificationFactor(int m) {
        _magnificationFactor(self, m);
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
    private static native int _facesCount(long self);
    private static native long _face(long self, int i);
    private static native double _relativeMinFaceSize(long self);
    private static native int _maxSignalSize(long self);
    private static native boolean _magnification(long self);
    private static native void _magnification(long self, boolean m);
    private static native int _magnificationFactor(long self);
    private static native void _magnificationFactor(long self, int m);
    private static native void _destroy(long self);

    public static class Face {

        private MatOfRect box;
        private MatOfDouble pulse;

        private Face(long addr) {
            this.self = addr;
        }

        public int getId() {
            return _id(self);
        }

        public Rect getBox() {
            if (box == null) box = new MatOfRect();
            _box(self, box.getNativeObjAddr());
            return box.toArray()[0];
        }

        public double getBpm() {
            return _bpm(self);
        }

        public double[] getPulse() {
            if (pulse == null) pulse = new MatOfDouble();
            _pulse(self, pulse.getNativeObjAddr());
            return pulse.toArray();
        }

        public boolean existsPulse() {
            return _existsPulse(self);
        }

        private long self = 0;
        private static native int _id(long self);
        private static native void _box(long self, long mat);
        private static native double _bpm(long self);
        private static native void _pulse(long self, long mat);
        private static native boolean _existsPulse(long self);
    }
}
