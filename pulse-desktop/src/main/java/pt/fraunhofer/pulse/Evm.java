package pt.fraunhofer.pulse;

import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Scalar;
import org.opencv.objdetect.CascadeClassifier;

public abstract class Evm {

    protected CascadeClassifier faceDetector;

    public Evm() {
    }

    public Evm(String filename) {
        init(filename);
    }

    private void init(String filename) {
        faceDetector = new CascadeClassifier(filename);
        if (faceDetector.empty()) {
            throw new RuntimeException("Failed to load cascade classifier");
        }
    }

    public void load(String filename) {
        init(filename);
    }

    protected static final Scalar BLUE = new Scalar(255, 0, 0, 255);
    protected static final Scalar RED = new Scalar(0, 0, 255, 255);

    protected static final Scalar ZERO = Scalar.all(0);

    public abstract void start(int width, int height);

    public abstract void stop();

    public abstract Mat onFrame(Mat frame);

    protected static void point(Mat o, Point p) {
        double value = o.get((int) p.y, (int) p.x)[2];
        Core.circle(o, p, 4, RED, 8);
        p.x += 10;
        Core.putText(o, String.valueOf(value), p, Core.FONT_HERSHEY_PLAIN, 2, RED);
        p.x -= 10;
    }

    // debug function
    protected static void printMat(String name, Mat mat, int rows, int cols, int channels) {
        System.out.println(name);
        System.out.println(mat);
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                System.out.print("(");
                for (int c = 0; c < channels; c++) {
                    System.out.print(mat.get(y, x)[c]);
                    if (c < channels - 1) {
                        System.out.print(", ");
                    }
                }
                System.out.print(")");
                if (x < cols - 1) {
                    System.out.print(" | ");
                }
            }
            System.out.println();
        }
    }

}
