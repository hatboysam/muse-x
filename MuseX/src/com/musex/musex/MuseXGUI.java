package com.musex.musex;

import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import javax.swing.*;
import javax.swing.text.DefaultCaret;
import java.awt.event.*;

/**
 * Author: samstern
 * Date: 4/23/14
 */
public class MuseXGUI implements SerialPortEventListener {
    public static final String CLASS_NAME = MuseXGUI.class.getSimpleName();

    private JButton leftButton;
    private JButton rightButton;
    private JCheckBox beat1CheckBox;
    private JCheckBox beat2CheckBox;
    private JCheckBox beat3CheckBox;
    private JCheckBox beat4CheckBox;
    private JButton calibrateButton;
    private JButton OKButton;
    private JButton playButton;
    private JPanel formPanel;
    private JScrollPane scrollPane;
    private JTextArea textArea;

    private ArduinoConnection ac;

    public MuseXGUI(JFrame frame) {
        // Open frame
        frame.setContentPane(formPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);

        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent windowEvent) {
                super.windowClosed(windowEvent);
                ac.close();
            }
        });

        // Autoscroll
        DefaultCaret caret = (DefaultCaret) textArea.getCaret();
        caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);

        // Connect to Arduino
        ac = new ArduinoConnection();
        boolean connected = ac.connectToBoard();
        if (connected){
            // Begin listening for events
            writeOutput("Arduino connected.");
            ac.addListener(this);
        } else {
            // Never gonna happen
            writeOutput("Failed to connect Arduino.");
        }

        // Set up buttons
        buttonSetup();

    }

    public void buttonSetup() {
        // Left and right
        leftButton.addActionListener(new StringSender("l", ac));
        rightButton.addActionListener(new StringSender("r", ac));

        // Start and stop
        playButton.addActionListener(new StringSender("o", ac));
    }

    public void writeOutput(String msg) {
        String currentText = textArea.getText();
        textArea.setText(currentText + "\n" + msg);
    }

    public static void main(String[] args) {
        // Setup the frame
        JFrame frame = new JFrame(CLASS_NAME);
        MuseXGUI museXGUI = new MuseXGUI(frame);
    }

    @Override
    public void serialEvent(SerialPortEvent serialPortEvent) {
        if (serialPortEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
            String inLine = ac.readLine();
            if (inLine != null) {
                writeOutput(inLine);
            }
        }
    }

    private static class StringSender implements ActionListener {
        private String string;
        private ArduinoConnection ac;

        private StringSender(String string, ArduinoConnection ac) {
            this.string = string;
            this.ac = ac;
        }

        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            ac.sendString(string);
        }
    }
}
