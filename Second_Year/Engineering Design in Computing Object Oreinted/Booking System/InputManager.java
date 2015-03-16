/**
 * the class that interpret the input
 */
import java.util.Scanner;
import java.lang.Integer;
import java.util.ArrayList;

/**
 * @author Jiashu Chen
 * @date Apr 16, 2013
 */
public class InputManager {
	/**
	 * Class Fields
	 */
	private BookingSystem myBookingSystem;

	// Regular expressions used to interpret input command.
	/**
	 * ROOM creating room command
	 * BOOK make new booking command
	 * CHANGE change booking command
	 * DELETE delete booking command
	 * PRINT print bookings for a room command
	 */
	private static final String ROOM = "^Room [0-9]+ [a-zA-Z0-9]+";
	private static final String BOOK = "^Book [a-zA-Z]+ [0-9]+ [0-9]+ [a-zA-Z]{3} [0-9]+ [0-9]{1,2} [0-9]+ [a-zA-Z0-9]+";
	private static final String CHANGE = "^Change [a-zA-Z]+ [a-zA-Z0-9]+ [0-9]+ [a-zA-Z]{3} [0-9]+ [0-9]{1,2} [0-9]+ [a-zA-Z]{3} [0-9]+ [0-9]{1,2} [0-9]+ [a-zA-Z0-9]+";
	private static final String DELETE = "^Delete [a-zA-Z]+ [a-zA-Z0-9]+ [0-9]+ [a-zA-Z]{3} [0-9]+ [0-9]{1,2}";
	private static final String PRINT = "^Print [a-zA-Z0-9]+";


	/**
	 * Creates a new instance of the 'InputManager' class to interpret user input.
	 * @param void
	 * @return Request Object
	 */
	public InputManager(){
		this.myBookingSystem = new BookingSystem();
	}
	/**
	 * change the string month to integer month
	 * @param Month
	 * @return month in integer form
	 */
	public int StringMonthToIntMonth (String Month){
		if (Month.equals("Jan")) return 1;
		else if (Month.equals("Feb")) return 2;
		else if (Month.equals("Mar")) return 3;
		else if (Month.equals("Apr")) return 4;
		else if (Month.equals("May")) return 5;
		else if (Month.equals("Jun")) return 6;
		else if (Month.equals("Jul")) return 7;
		else if (Month.equals("Aug")) return 8;
		else if (Month.equals("Sep")) return 9;
		else if (Month.equals("Oct")) return 10;
		else if (Month.equals("Nov")) return 11;
		else if (Month.equals("Dec")) return 12;
		else return 0;
	}


	/**
	 * Launches the command interpreter function
	 * which converts scanned user input into formal
	 * system commands.
	 * Matches using regular expressions.
	 * @param void
	 * @return void
	 */
	public void commandInterpreter(Scanner input){

		Scanner scanIn = input;
		String scannedLine = scanIn.nextLine();
		Boolean finish = false;

		while(!scannedLine.contains("#") && finish == false){
			//system terminate when # is entered
			// Determining input command.
            // all assignment is finished
			if(scannedLine.matches(ROOM)){

				String[] substringArray = scannedLine.split(" ");

				//allocated parameters from the input
				int capacity = Integer.parseInt(substringArray[1]);
				String roomName = substringArray[2];
				if(capacity > 0){
					myBookingSystem.setRoom(roomName, capacity);
				}
				//else {
					//System.out.println("Capacity of Room must be greater than 0");
				//}

			}else if(scannedLine.matches(BOOK )){
				String[] substringArray = scannedLine.split(" ");

				//allocated parameters from the input
				String user = substringArray[1];
				int numPeople = Integer.parseInt(substringArray[2]);
				int numWeeks = Integer.parseInt(substringArray[3]);
				String StringMonth = substringArray[4];
				int date = Integer.parseInt(substringArray[5]);
				int timeStart = Integer.parseInt(substringArray[6]);
				int duration = Integer.parseInt(substringArray[7]);
				String sessionName = substringArray[8];

				//construct the reservation object
				int IntMonth = this.StringMonthToIntMonth(StringMonth);
				Reservation newR = new Reservation (user,sessionName, timeStart, duration, date, IntMonth, null, numPeople);
				if(!myBookingSystem.addReservation(newR, numWeeks, null)){
					System.out.println("Booking rejected");
					//System.out.println("Booking Failed ERROR <Time Clash/Capacity>");
				}
			}else if(scannedLine.matches(CHANGE )) {

				String[] substringArray = scannedLine.split(" ");

				//allocated parameters from the input
				String user = substringArray[1];
				String roomName = substringArray[2];
				int numWeeks = Integer.parseInt(substringArray[3]);
				String StringMonthOld = substringArray[4];
				int dateOld = Integer.parseInt(substringArray[5]);
				int timeStartOld = Integer.parseInt(substringArray[6]);
				int numPeople = Integer.parseInt(substringArray[7]);
				String StringMonthNew = substringArray[8];
				int dateNew = Integer.parseInt(substringArray[9]);
				int timeStartNew = Integer.parseInt(substringArray[10]);
				int duration = Integer.parseInt(substringArray[11]);
				String sessionName = substringArray[12];

				int IntMonthOld = this.StringMonthToIntMonth(StringMonthOld);
				int IntMonthNew = this.StringMonthToIntMonth(StringMonthNew);
				Reservation old = new Reservation (user,null, timeStartOld, 0, dateOld, IntMonthOld, roomName, 0);
				Reservation newR = new Reservation (user,sessionName, timeStartNew, duration, dateNew, IntMonthNew, roomName, numPeople);
				myBookingSystem.changeReservation(old, newR, numWeeks);
			}else if(scannedLine.matches(DELETE)){
				String[] substringArray = scannedLine.split(" ");
				//allocated parameters from the input
				String user = substringArray[1];
				String roomName = substringArray[2];
				int numWeeks = Integer.parseInt(substringArray[3]);
				String StringMonth = substringArray[4];
				int date = Integer.parseInt(substringArray[5]);
				int timeStart = Integer.parseInt(substringArray[6]);
				int IntMonth = this.StringMonthToIntMonth(StringMonth);

				Reservation old = new Reservation (user,null, timeStart, 0, date, IntMonth, roomName, 0);
				ArrayList<Reservation> deleted = myBookingSystem.deleteReservation(old, roomName, numWeeks);
				if(!deleted.isEmpty()){
					System.out.println("Reservations deleted");
				}else {
					System.out.println("Deletion rejected");
				}
			}else if(scannedLine.matches(PRINT )) {
				String[] substringArray = scannedLine.split(" ");
				String roomName = substringArray[1];
				myBookingSystem.printReservationOfARoom(roomName);
			}
			if(scanIn.hasNextLine()){
				scannedLine = scanIn.nextLine();
			}else{
				finish = true;
			}
		}

		// Close the scanner.
		scanIn.close();
	}

}

