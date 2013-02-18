package pt.fraunhofer.pulse;

import com.xuggle.mediatool.IMediaReader;
import com.xuggle.mediatool.IMediaViewer;
import com.xuggle.mediatool.ToolFactory;
import java.awt.image.BufferedImage;
import javax.swing.JFrame;

public class App {

    public static void main(String[] args) {
        IMediaReader reader = ToolFactory.makeReader("../../vidmagSIGGRAPH2012/face_source_timecode.wmv");
        reader.setBufferedImageTypeToGenerate(BufferedImage.TYPE_3BYTE_BGR);
        reader.addListener(new EvmAdapter());
//        reader.addListener(ToolFactory.makeWriter("out.wmv", reader));
        reader.addListener(ToolFactory.makeViewer(IMediaViewer.Mode.FAST_VIDEO_ONLY, false, JFrame.EXIT_ON_CLOSE));
        while (reader.readPacket() == null) {}
        System.exit(0);
    }
    
}
