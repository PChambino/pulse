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

public class EvmGdownIdeal extends Evm {

    public EvmGdownIdeal() {
        super();
    }

    public EvmGdownIdeal(String filename) {
        super(filename);
    }

    private static final double MIN_FACE_SIZE   = 0.3f;
    private static final int    RATE            = 30; // TODO calculate fps
    private static final int    BLUR_LEVEL      = 4;
    private static final double F_LOW           = 45/60f;
    private static final double F_HIGH          = 240/60f;
    private static final int    WINDOW_SIZE     = 30;
    private static final Scalar ALPHA           = Scalar.all(50);

    private static final int F_LOW_FRAME = (int) (F_LOW/RATE*WINDOW_SIZE + 1);
    private static final int F_HIGH_FRAME = (int) (F_HIGH/RATE*WINDOW_SIZE + 1);

    private int t;
    private Point point;
    private Size minFaceSize;
    private Size maxFaceSize;
    private MatOfRect faces;
    private Mat gray;
    private Mat blurred;
    private Mat output;
    private Mat outputFloat;
    private Mat frameFloat;
    private Mat window;
    private Mat windowFiltered;
    private List<Mat> windowChannels;
    private Mat dftWindowChannel;
    private Mat idftWindowChannel;

    @Override
    public void start(int width, int height) {
        t = 0;
        point = new Point();
        minFaceSize = new Size(MIN_FACE_SIZE * width, MIN_FACE_SIZE * height);
        maxFaceSize = new Size();
        faces = new MatOfRect();
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

    @Override
    public void stop() {
        t = 0;
        faces.release();
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

    @Override
    public Mat onFrame(Mat frame) {
        // face detection
        Imgproc.cvtColor(frame, gray, Imgproc.COLOR_RGB2GRAY);
        faceDetector.detectMultiScale(gray, faces, 1.1, 2, 0, minFaceSize, maxFaceSize);

        // convert to float and remove alpha channel
        frame.convertTo(frameFloat, CvType.CV_32F);
        Imgproc.cvtColor(frameFloat, frameFloat, Imgproc.COLOR_RGBA2RGB);

        // apply spatial filter: blur and downsample
        frameFloat.copyTo(blurred);
        for (int i = 0; i < BLUR_LEVEL; i++) {
            Imgproc.pyrDown(blurred, blurred);
        }

        if (t < WINDOW_SIZE) {
            frame.copyTo(output);

            // create window for frames
            if (window.empty()) {
                window.create(blurred.width() * blurred.height(), WINDOW_SIZE, blurred.type());
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
                dftWindowChannel.colRange(0, F_LOW_FRAME).setTo(ZERO);
                dftWindowChannel.colRange(F_HIGH_FRAME, WINDOW_SIZE).setTo(ZERO);

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

            // add back alpha channel and convert to 8 bit
            Imgproc.cvtColor(outputFloat, outputFloat, Imgproc.COLOR_RGB2RGBA);
            outputFloat.convertTo(output, CvType.CV_8U);
        }

        // draw some rectangles and points
        for (Rect face : faces.toArray()) {
            Core.rectangle(output, face.tl(), face.br(), BLUE, 4);

            // forehead point
            point.x = face.tl().x + face.size().width * 0.5;
            point.y = face.tl().y + face.size().width * 0.2;
            point(output, point);

            // left cheek point
            point.x = face.tl().x + face.size().width * 0.7;
            point.y = face.tl().y + face.size().width * 0.6;
            point(output, point);

            // right cheek point
            point.x = face.tl().x + face.size().width * 0.3;
            point(output, point);
        }

        t++;
        
        return output;
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

}
