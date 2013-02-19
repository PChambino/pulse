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
    private static final int BLUR_LEVEL = 4;
    private static final int WINDOW_SIZE = 100; // TODO alter to have a MIN and MAX window size

    private int t;
    private Mat gray;
    private Mat blurred;
    private Mat output;
    private Point point;
    private Size minFaceSize;
    private Size maxFaceSize;
    private Mat window;
    private List<Mat> windowChannels = new ArrayList<Mat>();
    private Mat floatWindowChannel;
    private Mat dftWindowChannel;
    private Mat idftWindowChannel;

    public void start(int width, int height) {
        t = 0;
        gray = new Mat();
        blurred = new Mat();
        output = new Mat();
        point = new Point();
        minFaceSize = new Size(MIN_FACE_SIZE * width, MIN_FACE_SIZE * height);
        maxFaceSize = new Size();
        window = new Mat();
        windowChannels = new ArrayList<Mat>();
        floatWindowChannel = new Mat();
        dftWindowChannel = new Mat();
        idftWindowChannel = new Mat();
    }

    public void stop() {
        t = 0;
        gray.release();
        blurred.release();
        output.release();
        window.release();
        windowChannels.clear();
        floatWindowChannel.release();
        dftWindowChannel.release();
        idftWindowChannel.release();
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
        for (int i = 0; i < BLUR_LEVEL; i++) {
            Imgproc.pyrDown(blurred, blurred);
        }

        if (t < WINDOW_SIZE) {
            if (window.empty()) {
                window.create(blurred.width() * blurred.height(), WINDOW_SIZE, blurred.type());
                floatWindowChannel.create(window.size(), CvType.CV_32FC1);
            }
            putToWindow(window, blurred, t);
        } else {
            // shift window
            for (int x = 1; x < WINDOW_SIZE; x++) {
                window.col(x).copyTo(window.col(x - 1));
            }
            putToWindow(window, blurred, WINDOW_SIZE - 1);

            Core.split(window, windowChannels);
            for (int i = 0; i < windowChannels.size(); i++) {
                windowChannels.get(i).convertTo(floatWindowChannel, CvType.CV_32F);

//                System.out.println("FloatChannel("+i+")"); // TODO remove
//                System.out.println(floatWindowChannel);
//                for (int y = 0; y < 5; y++) {
//                    for (int x = 0; x < WINDOW_SIZE; x++) {
//                        System.out.print(floatWindowChannel.get(y, x)[0]+" | ");
//                    }
//                    System.out.println();
//                }

                Core.dft(floatWindowChannel, dftWindowChannel, Core.DFT_ROWS, 0);

//                System.out.println("DFT("+i+")"); // TODO remove
//                System.out.println(dftWindow);
//                for (int y = 0; y < 5; y++) {
//                    for (int x = 0; x < WINDOW_SIZE; x++) {
//                        System.out.print(dftWindow.get(y, x)[0]+" | ");
//                    }
//                    System.out.println();
//                }

                // apply ideal temporal filter
//                Mat dftMaskL = new Mat();
//                Mat dftMaskH = new Mat();
//                Mat dftMask = new Mat();
                // FIXME mask needs tiling ?
//                Core.inRange();
//                FIXME Replace with function inRange
//                Core.compare(dftWindow, Scalar.all(50/60), dftMaskL, Core.CMP_LT);
//                Core.compare(dftWindow, Scalar.all(60/60), dftMaskH, Core.CMP_GT);
//                Core.bitwise_or(dftMaskL, dftMaskH, dftMask);
//                Core.bitwise_and(dftWindow, dftMask, dftWindow);

                Core.idft(dftWindowChannel, idftWindowChannel, Core.DFT_ROWS + Core.DFT_SCALE, 0);

//                System.out.println("IDFT("+i+")"); // TODO remove
//                System.out.println(idftWindow);
//                for (int y = 0; y < 5; y++) {
//                    for (int x = 0; x < WINDOW_SIZE; x++) {
//                        System.out.print(idftWindow.get(y, x)[0]+" | ");
//                    }
//                    System.out.println();
//                }

                Mat a = new Mat(idftWindowChannel.size(), CvType.CV_8UC1);
                idftWindowChannel.convertTo(a, CvType.CV_8S);
                windowChannels.set(i, a);
            }
            Mat windowFiltered = new Mat();
            Core.merge(windowChannels, windowFiltered);
            System.out.println(windowFiltered);

            getLastFromWindow(windowFiltered, blurred);

            // amplify
            // Core.multiply(blurred, Scalar.all(50), blurred);

            for (int i = 0; i < BLUR_LEVEL; i++) {
                Imgproc.pyrUp(blurred, blurred);
            }
            Imgproc.resize(blurred, output, frame.size());
            System.out.println(output); // TODO remove

            // TODO verify type and depth, create new matrices and convert if necessary
            // blurred.convertTo(output, CvType.CV_8UC4);
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
        int rows = a.rows();
        int cols = a.cols();
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                window.put(y * cols + x, t, a.get(y, x));
            }
        }
    }

    private void getLastFromWindow(Mat window, Mat a) {
        int rows = a.rows();
        int cols = a.cols();
        for (int y = 0; y < rows; y++) {
            for (int x = 0; x < cols; x++) {
                a.put(y, x, window.get(y * cols + x, WINDOW_SIZE - 1));
            }
        }
    }

}
