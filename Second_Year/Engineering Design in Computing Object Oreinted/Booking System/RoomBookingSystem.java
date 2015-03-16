/**
 * the main class of the system
 * the system terminate when # is entered
 */
import java.util.Scanner;
import java.io.FileNotFoundException;
import java.io.FileReader;

/**
 * @author Jiashu Chen
 * @date Apr 16, 2013
 */
public class RoomBookingSystem {

	/**
	 * @param args
	 * @param arg[0] is the input
	 */
	public static void main(String[] args) {
		InputManager inputScanner = new InputManager();
		  try
	      {
	          Scanner sc = new Scanner(new FileReader(args[0]));
	          // arg[0] is the first command line argument
	          inputScanner.commandInterpreter(sc);
	      }
	      catch (FileNotFoundException e) {}
	}

}
