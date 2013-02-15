package pt.fraunhofer.pulse;

import java.awt.Canvas;
import java.awt.HeadlessException;
import java.awt.image.BufferStrategy;
import javax.swing.JFrame;

public class App extends JFrame {

    static { System.loadLibrary("opencv_java"); }

    public static void main(String[] args) {
        App app = new App();
        app.setVisible(true);
        app.start();
    }

    private AppCanvas canvas;

    private App() throws HeadlessException {
        setTitle("Pulse");
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setResizable(false);
        setIgnoreRepaint(true);

        canvas = new AppCanvas();
        getContentPane().add(canvas);

        pack();
        setLocationRelativeTo(null);
    }

    private void load(String filename) {
        // TODO
    }

    private void start() {

    }

    private static class AppCanvas extends Canvas {

        private BufferStrategy strategy;

        public AppCanvas() {
            setIgnoreRepaint(true);
            setSize(500, 500);
        }

        public void start() {
            createBufferStrategy(3);
            strategy = getBufferStrategy();
            
            loop();
        }

        private void loop() {
            
        }

    }

}
