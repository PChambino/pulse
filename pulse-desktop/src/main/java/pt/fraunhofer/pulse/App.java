package pt.fraunhofer.pulse;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.IMediaViewer;
import com.xuggle.mediatool.ToolFactory;
import java.awt.image.BufferedImage;
import javax.swing.JFrame;

public class App {

    static {
        System.loadLibrary("opencv_java");
    }

    public static void main(String[] args) {
        IMediaReader reader = ToolFactory.makeReader(
                "../../vidmagSIGGRAPH2012/face_source_timecode.wmv"
//                "../../vidmagSIGGRAPH2012/face2_source.mp4"
//                "../../vidmagSIGGRAPH2012/baby2_source.mp4"
                );
        reader.setBufferedImageTypeToGenerate(BufferedImage.TYPE_3BYTE_BGR);
        
        // original viewer
        reader.addListener(ToolFactory.makeViewer(IMediaViewer.Mode.FAST_VIDEO_ONLY, false, JFrame.EXIT_ON_CLOSE));
        
        // evm
        reader.addListener(new EvmMediaToolAdapter(
//                new EvmGdownIdeal
//                new EvmGdownIIR
                new EvmLpyrIIR
                        ("res/lbpcascade_frontalface.xml")));

        // evm viewer
        reader.addListener(ToolFactory.makeViewer(IMediaViewer.Mode.FAST_VIDEO_ONLY, false, JFrame.EXIT_ON_CLOSE));

        // writer
//        reader.addListener(ToolFactory.makeWriter("out.mp4", reader));

        while (reader.readPacket() == null) {}
        System.exit(0);
    }
    
}
