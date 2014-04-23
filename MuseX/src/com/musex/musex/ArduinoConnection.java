package com.musex.musex;

import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;

import java.util.Enumeration;

/**
 * Manage connection to Arduino microcontroller.
 *
 * Adapted from http://www.drdobbs.com/jvm/control-an-arduino-from-java/240163864
 */
public class ArduinoConnection implements SerialPortEventListener {

    private static final String MAC_PORT_NAME_MOD = "/dev/tty.usbmodem";
    private static final String MAC_PORT_NAME_SER = "/dev/tty.usbserial";
    private static final String WIN_PORT_NAME = "COM3";
    private static final String[] PORT_NAMES = new String[]{
            MAC_PORT_NAME_MOD,
            MAC_PORT_NAME_SER,
            WIN_PORT_NAME
    };

    private static final String APP_NAME = "MuseX";
    private static final int TIME_OUT = 1000;
    private static final int DATA_RATE = 9600;

    private SerialPort serialPort;

    public ArduinoConnection() {
        // Nothing to initialize
    }

    public boolean connectToBoard() {
        try {
            CommPortIdentifier portId = null;
            Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

            while ((portId == null) && (portEnum.hasMoreElements())) {
                CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
                for (String portName : PORT_NAMES) {
                    // System.out.println("Found: " + currPortId.getName());
                    if (currPortId.getName().equals(portName) || currPortId.getName().startsWith(portName)) {
                        System.out.println("Attempting to Open");
                        // Open serial port
                        serialPort = (SerialPort) currPortId.open(APP_NAME, TIME_OUT);
                        portId = currPortId;
                        System.out.println( "Connected on port" + currPortId.getName() );
                    }
                }
            }

            if (portId == null || serialPort == null) {
                System.out.println("Error: Could not connect to Arduino");
                return false;
            }

            serialPort.setSerialPortParams(DATA_RATE, SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);

            // add event listeners
            serialPort.addEventListener(this);
            serialPort.notifyOnDataAvailable(true);

            // Give the Arduino some time
            try {
                Thread.sleep(2000);
            } catch (InterruptedException ie) {
                // Do Nothing
            }

            return true;
        } catch (Exception e) {
            e.printStackTrace();
        }

        // Fail
        return false;
    }

    public synchronized void close() {
        if (serialPort != null) {
            serialPort.removeEventListener();
            serialPort.close();
        }
    }

    @Override
    public void serialEvent(SerialPortEvent serialPortEvent) {
        // TODO: Implement
    }

    public static void main(String[] args) {
        // TODO: Use or move

        // Open and Connect
        ArduinoConnection ac = new ArduinoConnection();
        ac.connectToBoard();

        // Close the connection
        ac.close();
    }
}
