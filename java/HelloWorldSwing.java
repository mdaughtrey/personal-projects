//package start;
import javax.swing.*;

public class HelloWorldSwing
{
    private static void createAndShowGui()
    {
        JFrame frame = new JFrame("HelloWorldSwing");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        JLabel label = new JLabel("Hello you bastards");
        frame.getContentPane().add(label);

        frame.pack();
        frame.setVisible(true);
    }
    public static void main(String[] args)
    {
        javax.swing.SwingUtilities.invokeLater(new Runnable()
        {
            public void run()
            {
                createAndShowGui();
            }
        });
    }
}
