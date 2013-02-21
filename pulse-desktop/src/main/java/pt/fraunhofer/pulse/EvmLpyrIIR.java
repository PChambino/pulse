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

public class EvmLpyrIIR extends Evm {

    public EvmLpyrIIR() {
        super();
    }

    public EvmLpyrIIR(String filename) {
        super(filename);
    }

    private static final float MIN_FACE_SIZE = 0.3f;
    private static final float F_LOW = 45/60f;
    private static final float F_HIGH = 240/60f;
    private static final Scalar ALPHA = Scalar.all(50);

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
    private List<Mat> windowChannels = new ArrayList<Mat>();
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

        // convert to YUV color space
        frame.convertTo(frameFloat, CvType.CV_32F);
        Imgproc.cvtColor(frameFloat, frameFloat, Imgproc.COLOR_RGB2YUV);

        // TODO apply spatial filter: Laplacian pyramid

        // TODO apply temporal filter: substraction of two IIR lowpass filters

        // TODO amplify
        //Core.multiply(blurred, ALPHA, blurred);

        // TODO rebuild frame from Laplacian pyramid
        //Imgproc.resize(blurred, outputFloat, frame.size());

        // TODO add back to original frame
        //Core.add(frameFloat, outputFloat, outputFloat);

        // convert back to RGBA
        Imgproc.cvtColor(frameFloat, outputFloat, Imgproc.COLOR_YUV2RGB);
        Imgproc.cvtColor(outputFloat, outputFloat, Imgproc.COLOR_RGB2RGBA);
        outputFloat.convertTo(output, CvType.CV_8U);

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

}
