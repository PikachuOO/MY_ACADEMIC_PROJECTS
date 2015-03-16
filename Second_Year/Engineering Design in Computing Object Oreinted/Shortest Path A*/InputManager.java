/**
 * The system take at maximum 32 jobs
 * with index 0-31 by the order entered
 */

/**
 * @author Jiashu Chen
 */
import java.util.Scanner;
import java.lang.Integer;

public class InputManager {
	/**
	 * Class Fields
	 */

	// Regular expression used to interpret input command.
	/**
	 * Job creating job command
	 */
	private static final String Job = "^Job [0-9]+ [0-9]+ to [0-9]+ [0-9]+";
	private static int ID;

	/**
	 * Creates a new instance of the 'InputManager' class to interpret user input.
	 */
	public InputManager(){
		ID = 0;
	}

	/**
	 * Launches the command interpreter function
	 * which converts scanned user input into formal
	 * system commands.
	 * Matches using regular expressions.
	 * @param input scanner object that scan specifies the source of input
	 */
	public void commandInterpreter(Scanner input, JobGraph graph){
		Scanner scanIn = input;
		String scannedLine = scanIn.nextLine();
		boolean done = false;
		while(!scannedLine.contains("#")
				&& !done){
			//system terminate when # is entered
			// Determining input command.
			if(scannedLine.matches(Job)){
				String[] substringArray = scannedLine.split(" ");
				//allocated parameters from the input
				int startX = Integer.parseInt(substringArray[1]);
				int startY = Integer.parseInt(substringArray[2]);
				int endX = Integer.parseInt(substringArray[4]);
				int endY = Integer.parseInt(substringArray[5]);

				Job newjob = new Job(startX, startY, endX, endY, ID++);
				graph.addJob(newjob);
			}
			if(scanIn.hasNextLine()){
				scannedLine = scanIn.nextLine();
			}else {
				done = true;
			}
		}

		// Close the scanner.
		scanIn.close();
	}

}