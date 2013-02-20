package pt.fraunhofer.pulse;

import com.xuggle.mediatool.MediaToolAdapter;
import com.xuggle.mediatool.event.IVideoPictureEvent;
import com.xuggle.xuggler.IVideoPicture;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class EvmAdapter extends MediaToolAdapter {

    static {
        System.loadLibrary("opencv_java");
    }

    private Evm evm = new Evm("res/lbpcascade_frontalface.xml");
    private boolean initialize = true;

    private byte[] picData;
    private Mat picDataMat;
    private Mat frame;
    private byte[] frameData;
    private int frameRows;
    private int frameCols;
    private int frameChannels;
    private int[] imgData;

    @Override
    public void onVideoPicture(IVideoPictureEvent event) {
        IVideoPicture p = event.getPicture();
        if (initialize) {
            picData = new byte[p.getSize()];
            picDataMat = new Mat(p.getHeight() + p.getHeight() / 2, p.getWidth(), CvType.CV_8UC1);
            frame = new Mat();
        }

        p.get(0, picData, 0, picData.length);
        picDataMat.put(0, 0, picData);
        Imgproc.cvtColor(picDataMat, frame, Imgproc.COLOR_YUV420p2RGBA, 4);

        if (initialize) {
            frameRows = frame.rows();
            frameCols = frame.cols();
            frameChannels = frame.channels();
            frameData = new byte[frameCols * frameRows * frameChannels];
            imgData = new int[frameCols * frameRows];
            evm.start(frameCols, frameRows);
            initialize = false;
        }

        Mat result = evm.onFrame(frame);

        result.get(0, 0, frameData);
        for (int y = 0; y < frameRows; y++) {
            for (int x = 0; x < frameCols; x++) {
                int index = (x + y * frameCols) * frameChannels;
                imgData[x + y * frameCols] = (0xff & frameData[index + 3]) << 24 |
                                             (0xff & frameData[index + 2]) << 16 |
                                             (0xff & frameData[index + 1]) << 8  |
                                             (0xff & frameData[index + 0]);
            }
        }
        event.getImage().setRGB(0, 0, frameCols, frameRows, imgData, 0, frameCols);

        super.onVideoPicture(event);
    }

}
