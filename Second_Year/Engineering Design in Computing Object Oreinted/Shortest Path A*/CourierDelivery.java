/**
 * Main class
 * This class is the overall
 *
 */

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.Scanner;

/**
 * @author Jiashu Chen
 */
public class CourierDelivery {
	/**
	 * @param args
	 * arg[0] is the first command line argument
	 */
	public static void main(String[] args) {
		InputManager inputScanner = new InputManager();
		JobGraph graph = new JobGraph();
		  try
	      {
	          Scanner sc = new Scanner(new FileReader(args[0]));
	          inputScanner.commandInterpreter(sc, graph);
	      }
	      catch (FileNotFoundException e) {}
		  //test graph input
		  //System.out.println(graph.toString());
		  graph.pathGenerator();
	}

}
