/**the room class contain all important property
 * of rooms, such as it's name and capacity also
 * it contains an ArrayList of reservations
 */
import java.util.ArrayList;
import java.util.Iterator;

/**
 * @author Jiashu Chen
 * @date Apr 16, 2013
 */
public class Room {

	/**
	 * Class Fields
	 */
	private String roomName;
	private int roomCapacity;
	private ArrayList<Reservation> reservationList;

	/**
	 * Create a new instance of 'Room'
	 * @param newName as string
	 * @param capacity as number
	 */
	public Room(String newName, int capacity) {
		roomName = newName;
		roomCapacity = capacity;
		reservationList = new ArrayList<Reservation>();
	}
	/**
	 * add a new reservation to the list
	 * @param newbooking
	 * @return return true if addition is successful
	 */
	public Boolean addReservation(Reservation newbooking, int weeks) {
		int i;
		Reservation temp = new Reservation (newbooking.getUser(),
				newbooking.getSessionName(), newbooking.getDate().getTimeStart(),
				newbooking.getDate().getDuration(),newbooking.getDate().getDate(),
				newbooking.getDate().getMonth(), newbooking.getRoomName(), newbooking.getNumPeople());
		//check for clash
		for (i=0; i < weeks; i++){
			if(this.reservationClashRoom(temp)){
				return false;
			}
			temp.getReservationNextWeek(temp);
		}
		temp = newbooking;
		//add if no clash
		for (i=0; i < weeks; i++){
			Reservation added = new Reservation (temp.getUser(),
					temp.getSessionName(), temp.getDate().getTimeStart(),
					temp.getDate().getDuration(),temp.getDate().getDate(),
					temp.getDate().getMonth(), this.roomName, temp.getNumPeople());
			temp.getReservationNextWeek(temp);
			this.reservationList.add(added);
		}
		System.out.println("Room " + this.getRoomName() + " assigned");
		return true;
	}
	/**
	 * if a reservation clash with reservations that already in room
	 * @param booking
	 * @return true if clashes
	 */
	private Boolean reservationClashRoom(Reservation booking){
		//if this.reservationList is empty, no need to check
		if(this.reservationList.isEmpty()) return false;
		Iterator<Reservation> itr = this.reservationList.iterator();
	    Reservation temp = this.reservationList.get(0);
	    if (booking.reservationClash(temp)) return true;
	    while (itr.hasNext()){
	    	  temp = itr.next();
	    	  if (booking.reservationClash(temp)) return true;
	    }
		return false;
	}
	/**
	 * do the comparison for deletion command
	 * @param booking
	 * @return true if can delete
	 */
	private Boolean checkUserDate (Reservation resOne, Reservation resTwo){
		if(resOne.getUser().equals(resTwo.getUser())
    			&& resOne.getDate().getDate() == resTwo.getDate().getDate()
    			&& resOne.getDate().getMonth() == resTwo.getDate().getMonth()
    			&& resOne.getDate().getTimeStart() == resTwo.getDate().getTimeStart()){
			return true;
		}
		return false;
	}
	/**
	 * check if a reservation can be deleted
	 * @param booking
	 * @return return false if can't delete
	 */
	public Boolean checkCanDelete (Reservation booking){
		if(this.reservationList.isEmpty()) return false;
		Iterator<Reservation> itr = this.reservationList.iterator();
	    Reservation temp = this.reservationList.get(0);

	    if(this.checkUserDate(temp, booking)) return true;
	    while(itr.hasNext()){
		    if(this.checkUserDate(temp, booking)) return true;
	    	temp = itr.next();
	    }
	    return false;
	}
	/**
	 * delete reservation a specific
	 * @param booking
	 * @return the reservation deleted, null is return when there is nothing being deleted
	 */
	public Reservation deleteParticularReservation (Reservation booking){
		Reservation deleted = null;
		Iterator<Reservation> itr = this.reservationList.iterator();
	    Reservation temp = this.reservationList.get(0);
	    Boolean stop = false;

	    if(this.checkUserDate(temp, booking)) {
    		this.reservationList.remove(temp);
	    }
	    while(itr.hasNext() && stop == false){
	    	if(this.checkUserDate(temp, booking)) {
	    		this.reservationList.remove(temp);
	    		stop = true;
	    	}
	    	if(stop == false){
	    		temp = itr.next();
	    	}
	    }
		return deleted;
	}

	/**
	 * print all reservation store in the room
	 */
	public void printReservationInRoom (){
		//System.out.println("-----------------------------------------------");
		String StringMonth;
		System.out.println(this.getRoomName());
		for(Reservation temp : this.reservationList){
			 StringMonth = this.IntMonthToStringMonth(temp.getDate().getMonth());
			 System.out.println(temp.getUser()+" "+StringMonth+" "+temp.getDate().getDate()+" "+ temp.getDate().getTimeStart()+" "+temp.getDate().getDuration()+" "+temp.getSessionName());
		}
		//System.out.println("-----------------------------------------------");

	}

	/**
	 * change the integer month to string month
	 * @param Month
	 * @return month in integer form
	 */
	public String IntMonthToStringMonth (int Month){
		if (Month == 1) return "Jan";
		else if (Month == 2) return "Feb";
		else if (Month == 3) return "Mar";
		else if (Month == 4) return "Apr";
		else if (Month == 5) return "May";
		else if (Month == 6) return "Jun";
		else if (Month == 7) return "Jul";
		else if (Month == 8) return "Aug";
		else if (Month == 9) return "Sep";
		else if (Month == 10) return "Oct";
		else if (Month == 11) return "Nov";
		else if (Month == 12) return "Dec";
		else return null;
	}


	/**
	 * series of get functions
	 */
	/**
	 * getRoomName
	 * @return name of room
	 */
	public String getRoomName() {
		return roomName;
	}
	/**
	 * getRoomCapacity
	 * @return the capacity of room
	 */
	public int getRoomCapacity() {
		return roomCapacity;
	}
	/**
	 * getReservationList();
	 * @return a list of reservation that booked to the room
	 */
	public ArrayList<Reservation> getReservationList() {
		return reservationList;
	}
}
