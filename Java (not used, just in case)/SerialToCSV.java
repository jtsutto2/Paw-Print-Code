import com.fazecast.jSerialComm.SerialPort;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.Scanner;

public class SerialToCSV {

    public static void main(String[] args) {
        // Open the first available serial port
        SerialPort comPort = SerialPort.getCommPorts()[0];
        comPort.openPort();
        comPort.setComPortParameters(9600, 8, 1, SerialPort.NO_PARITY);

        try {
            // Prepare the scanner for reading serial data
            Scanner scanner = new Scanner(comPort.getInputStream());
            // FileWriter to append data to the CSV file
            FileWriter fileWriter = new FileWriter("data_log.csv", true);
            // Use PrintWriter for easy writing and flushing
            PrintWriter printWriter = new PrintWriter(fileWriter);

            while (scanner.hasNextLine()) {
                System.out.println("Trying");
                try {
                    String line = scanner.nextLine();
                    System.out.println("Received: " + line); // Debug print
                    // Write the received line to the CSV file
                    printWriter.println(line);
                    // Ensure data is written by flushing the writer
                    printWriter.flush();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            // Cleanup
            scanner.close();
            printWriter.close();
            fileWriter.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (comPort.isOpen()) {
                comPort.closePort();
            }
        }
    }
}