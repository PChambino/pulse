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

    private static final Scalar ZERO = Scalar.all(0);

    private static final float MIN_FACE_SIZE = 0.3f;
    private static final int RATE = 30;
    private static final int BLUR_LEVEL = 4;
    private static final float F_LOW = 45/60f;
    private static final float F_HIGH = 240/60f;
    private static final int WINDOW_SIZE = 30; // TODO alter to have a varying size
    private static final Scalar ALPHA = Scalar.all(50);

    private static final int F_LOW_FRAME = (int) (F_LOW/RATE*WINDOW_SIZE + 1);
    private static final int F_HIGH_FRAME = (int) (F_HIGH/RATE*WINDOW_SIZE + 1);

    private int t;
    private Point point;
    private Size minFaceSize;
    private Size maxFaceSize;
    private Mat gray;
    private Mat blurred;
    private Mat output;
    private Mat outputFloat;
    private Mat frameFloat;
    private Mat window;
    private Mat windowFiltered;
    private List<Mat> windowChannels = new ArrayList<Mat>();
    private Mat dftWindowChannel;
    private Mat idftWindowChannel;

    public void start(int width, int height) {
        t = 0;
        point = new Point();
        minFaceSize = new Size(MIN_FACE_SIZE * width, MIN_FACE_SIZE * height);
        maxFaceSize = new Size();
        gray = new Mat();
        blurred = new Mat();
        output = new Mat();
        outputFloat = new Mat();
        frameFloat = new Mat();
        window = new Mat();
        windowFiltered = new Mat();
        windowChannels = new ArrayList<Mat>();
        dftWindowChannel = new Mat();
        idftWindowChannel = new Mat();
    }

    public void stop() {
        t = 0;
        gray.release();
        blurred.release();
        output.release();
        outputFloat.release();
        frameFloat.release();
        window.release();
        windowFiltered.release();
        windowChannels.clear();
        dftWindowChannel.release();
        idftWindowChannel.release();
    }

    public Mat onFrame(Mat frame) {

        // convert to YUV color space
        frame.convertTo(frameFloat, CvType.CV_32F);
        Imgproc.cvtColor(frameFloat, frameFloat, Imgproc.COLOR_RGB2YCrCb);

        // apply spatial filter: blur and downsample
        frameFloat.copyTo(blurred);
        for (int i = 0; i < BLUR_LEVEL; i++) {
            Imgproc.pyrDown(blurred, blurred);
        }


        if (t < WINDOW_SIZE) {
            // face detection
            MatOfRect faces = new MatOfRect();
            if (faceDetector != null) {
                Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGB2GRAY);
                faceDetector.detectMultiScale(gray, faces, 1.1, 2, 0, minFaceSize, maxFaceSize);
            }

            // draw some rectangles and points
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

            // create window for frames
            if (window.empty()) {
                window.create(blurred.width() * blurred.height(), WINDOW_SIZE, CvType.CV_32FC(blurred.channels()));
            }
            putToWindow(window, blurred, t);
        } else {
            // shift window
            window.colRange(1, WINDOW_SIZE).copyTo(window.colRange(0, WINDOW_SIZE - 1));
            putToWindow(window, blurred, WINDOW_SIZE - 1);

            Core.split(window, windowChannels);
            for (Mat channel : windowChannels) {
                Core.dft(channel, dftWindowChannel, Core.DFT_ROWS, 0);

                // apply ideal temporal filter
                if (F_LOW_FRAME <= F_HIGH_FRAME) {
                    dftWindowChannel.colRange(0, F_LOW_FRAME).setTo(ZERO);
                    dftWindowChannel.colRange(F_HIGH_FRAME, WINDOW_SIZE).setTo(ZERO);
                } else {
                    dftWindowChannel.colRange(F_HIGH_FRAME, F_LOW_FRAME).setTo(ZERO);
                }

                Core.idft(dftWindowChannel, idftWindowChannel, Core.DFT_ROWS + Core.DFT_SCALE, 0);

                idftWindowChannel.copyTo(channel);
            }
            Core.merge(windowChannels, windowFiltered);

            getLastFromWindow(windowFiltered, blurred);

            // amplify
            Core.multiply(blurred, ALPHA, blurred);

            // resize back to original size
            for (int i = 0; i < BLUR_LEVEL; i++) {
                Imgproc.pyrUp(blurred, blurred);
            }
            Imgproc.resize(blurred, outputFloat, frame.size());

            // add back to original frame
            Core.add(frameFloat, outputFloat, outputFloat);

            Imgproc.cvtColor(outputFloat, outputFloat, Imgproc.COLOR_YCrCb2RGB);
            Imgproc.cvtColor(outputFloat, outputFloat, Imgproc.COLOR_RGB2RGBA);
            outputFloat.convertTo(output, CvType.CV_8U);
        }

        t++;
        
        return output;
    }

    private static void point(Mat original, Mat o, Point p) {
        Core.circle(o, p, 4, BLUE, 8);
        double value = original.get((int) p.x, (int) p.y)[0];
        p.x += 10;
        Core.putText(o, String.valueOf(value), p, Core.FONT_HERSHEY_PLAIN, 2, BLUE);
        p.x -= 10;
    }

    private static void putToWindow(Mat window, Mat a, int t) {
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

    // debug function
    private static void printMat(String name, Mat mat, int rows, int cols, int channels) {
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
