package pt.fraunhofer.pulse;

import java.awt.HeadlessException;
import javax.swing.JFrame;

public class App extends JFrame {

    static { System.loadLibrary("opencv_java"); }

    public static void main(String[] args) {
        App app = new App();
        app.setVisible(true);
    }

    public App() throws HeadlessException {
    }

}
