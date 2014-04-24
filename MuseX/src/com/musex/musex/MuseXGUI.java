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
    private JButton upPickButton;
    private JButton downPickButton;

    private ArduinoConnection ac;

    public MuseXGUI(JFrame frame) {
        // Open frame
        frame.setContentPane(formPanel);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.pack();
        frame.setVisible(true);

        // Make sure Arduino connection closes on window exit
        frame.addWindowListener(new WindowAdapter() {
            @Override
            public void windowClosed(WindowEvent windowEvent) {
                super.windowClosed(windowEvent);
                ac.close();
            }
        });

        // Set up auto-scroll for output pane
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

        // Checkbox start position
        beat1CheckBox.setSelected(true);
        beat2CheckBox.setSelected(true);
        beat3CheckBox.setSelected(true);
        beat4CheckBox.setSelected(false);

        // Set up buttons
        buttonSetup();
    }

    public void buttonSetup() {
        // Left and right
        leftButton.addActionListener(new StringSender("l", ac));
        rightButton.addActionListener(new StringSender("r", ac));

        // Start and stop
        playButton.addActionListener(new StringSender("o", ac));

        // Calibration
        calibrateButton.addActionListener(new StringSender("c", ac));
        OKButton.addActionListener(new StringSender("k", ac));
        upPickButton.addActionListener(new StringSender("u", ac));
        downPickButton.addActionListener(new StringSender("d", ac));

        // Beat selection
        beat1CheckBox.addItemListener(new CheckboxStringSender("b11", "b10", ac));
        beat2CheckBox.addItemListener(new CheckboxStringSender("b21", "b20", ac));
        beat3CheckBox.addItemListener(new CheckboxStringSender("b31", "b30", ac));
        beat4CheckBox.addItemListener(new CheckboxStringSender("b41", "b40", ac));
    }

    public void writeOutput(String msg) {
        // Write a string to the output panel
        String currentText = textArea.getText();
        textArea.setText(currentText + "\n" + msg);
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

    public static void main(String[] args) {
        // Load the library
        // TODO

        // Setup the frame
        JFrame frame = new JFrame(CLASS_NAME);
        MuseXGUI museXGUI = new MuseXGUI(frame);
    }

    /**
     * Button Listener that sends a String to Arduino.
     */
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

    private static class CheckboxStringSender implements ItemListener {

        private String sChecked;
        private String sUnchecked;
        private ArduinoConnection ac;

        private CheckboxStringSender(String sChecked, String sUnchecked, ArduinoConnection ac) {
            this.sChecked = sChecked;
            this.sUnchecked = sUnchecked;
            this.ac = ac;
        }

        @Override
        public void itemStateChanged(ItemEvent itemEvent) {
            if (itemEvent.getStateChange() == ItemEvent.SELECTED) {
                ac.sendString(sChecked);
            } else if(itemEvent.getStateChange() == ItemEvent.DESELECTED) {
                ac.sendString(sUnchecked);
            }
        }
    }
}
