package pt.fraunhofer.pulse;

import java.util.ArrayList;
import java.util.List;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.core.Size;
import org.opencv.imgproc.Imgproc;
import org.opencv.objdetect.CascadeClassifier;

public class Evm {

    private CascadeClassifier faceDetector;

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

    private static final Scalar RED = new Scalar(255, 0, 0, 255);
    private static final Scalar BLUE = new Scalar(0, 0, 255, 255);

    private static final float MIN_FACE_SIZE = 0.3f;
    private static final int PYR_DOWN_LEVEL = 4;
    private static final int WINDOW_SIZE = 100; // TODO alter to have a MIN and MAX window size

    private int t;
    private Mat gray;
    private Mat blurred;
    private Mat output;
    private Point point;
    private Size minFaceSize;
    private Size maxFaceSize;
    private Mat window;

    public void start(int width, int height) {
        t = 0;
        gray = new Mat();
        blurred = new Mat();
        output = new Mat();
        point = new Point();
        minFaceSize = new Size(MIN_FACE_SIZE * width, MIN_FACE_SIZE * height);
        maxFaceSize = new Size();
        window = new Mat();
    }

    public void stop() {
        t = 0;
        gray.release();
        blurred.release();
        output.release();
        window.release();
    }

    public Mat onFrame(Mat frame) {
        MatOfRect faces = new MatOfRect();

        if (faceDetector != null) {
            Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGB2GRAY);
            faceDetector.detectMultiScale(gray, faces, 1.1, 2, 0, minFaceSize, maxFaceSize);
        }

        frame.copyTo(output);

        for (Rect face : faces.toArray()) {
            Core.rectangle(output, face.tl(), face.br(), RED, 4);

            // forehead point
            point.x = face.tl().x + face.size().width * 0.5;
            point.y = face.tl().y + face.size().width * 0.2;
            point(frame, output, point);

            // left cheek point
            point.x = face.tl().x + face.size().width * 0.7;
            point.y = face.tl().y + face.size().width * 0.6;
            point(frame, output, point);

            // right cheek point
            point.x = face.tl().x + face.size().width * 0.3;
            point(frame, output, point);
        }

        // apply spatial filter: blur and downsample
        frame.copyTo(blurred);
        for (int i = 0; i < PYR_DOWN_LEVEL; i++) {
            Imgproc.pyrDown(blurred, blurred);
        }

        if (t < WINDOW_SIZE) {
            if (window.empty()) {
                window.create(blurred.width() * blurred.height(), WINDOW_SIZE, blurred.type());
            }
            putToWindow(window, blurred, t);
        } else {
            // shift window
            for (int x = 1; x < window.cols(); x++) {
                window.col(x).copyTo(window.col(x - 1));
            }
            putToWindow(window, blurred, WINDOW_SIZE - 1);

            List<Mat> windowChannels = new ArrayList<Mat>();
            Core.split(window, windowChannels);
            for (int i = 0; i < windowChannels.size(); i++) {
                System.out.println("...");
                System.out.flush();
                Mat channel = windowChannels.get(i);
                Mat floatChannel = new Mat(channel.size(), CvType.CV_32FC1);
                channel.convertTo(floatChannel, floatChannel.type());
                System.out.println("!!!");
                System.out.flush();

                // TODO resize Mat to optimal dft size
                // Core.getOptimalDFTSize(...);
                Mat dftWindow = new Mat();
                Core.dft(floatChannel, dftWindow, Core.DFT_ROWS, floatChannel.rows());
                System.out.println("???");
                System.out.flush();

                // apply ideal temporal filter
                Mat dftMaskL = new Mat();
                Mat dftMaskH = new Mat();
                Mat dftMask = new Mat();
                // FIXME mask needs tiling ?
                Core.compare(dftWindow, Scalar.all(50/60), dftMaskL, Core.CMP_LT);
                Core.compare(dftWindow, Scalar.all(60/60), dftMaskH, Core.CMP_GT);
                Core.bitwise_or(dftMaskL, dftMaskH, dftMask);
                Core.bitwise_and(dftWindow, dftMask, dftWindow);

                Mat idftWindow = new Mat();
                Core.idft(dftWindow, idftWindow, Core.DFT_ROWS + Core.DFT_REAL_OUTPUT, 0);

                windowChannels.set(i, idftWindow);
            }
            Mat windowFiltered = new Mat();
            System.out.println(windowChannels.size());
            Core.merge(windowChannels, windowFiltered);

            getLastFromWindow(windowFiltered, output);

            // amplify
            Core.multiply(output, Scalar.all(50), output);
        }

        t++;

        return output;
    }

    private void point(Mat original, Mat o, Point p) {
        Core.circle(o, p, 4, BLUE, 8);
        double value = original.get((int) p.x, (int) p.y)[0];
        p.x += 10;
        Core.putText(o, String.valueOf(value), p, Core.FONT_HERSHEY_PLAIN, 2, BLUE);
        p.x -= 10;
    }

    private void putToWindow(Mat window, Mat a, int t) {
        for (int y = 0; y < a.rows(); y++) {
            for (int x = 0; x < a.cols(); x++) {
                window.put(y * a.cols() + x, t, a.get(y, x));
            }
        }
    }

    private void getLastFromWindow(Mat window, Mat a) {
        for (int y = 0; y < a.rows(); y++) {
            for (int x = 0; x < a.cols(); x++) {
                a.put(y, x, window.get(y * a.cols() + x, window.rows() - 1));
            }
        }
    }

}
