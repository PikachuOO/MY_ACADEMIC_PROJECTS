/**reservation class store user who booked the reservation
 * session name
 * date
 * time
 * month
 * duration
 */

/**
 * @author Jiashu Chen
 * @date Apr 16, 2013
 */

public class Reservation {

	/**
	 * Class Field
	 */
	private String User;
	private String sessionName;
	private DayDealer date;
	private String roomName;
	private int numPeople;
	private final int dayLimit = 24;
	/**
	 * Create a new instance of 'Reservation'
	 * @param UserName
	 * @param SessionName
	 * @param timeIn
	 * @param duration
	 * @param date
	 * @param month
	 * @param roomName
	 * @param People
	 */
	public Reservation(String UserName, String SessionName, int timeIn, int duration, int date, int month, String roomName, int People) {
		this.User = UserName;
		this.sessionName = SessionName;
		this.date = new DayDealer(timeIn, duration, date, month);
		this.roomName = roomName;
		this.numPeople = People;
	}
	/**
	 * check if two reservation whether they are the same
	 * @param res
	 * @return true for equal
	 */
	public Boolean compareSame (Reservation res){
		if (this.User.equals(res.User)
				&& this.sessionName.equals(res.sessionName)
				&& this.date.isEqual(res.date) == true
				&& this.roomName.equals(res.roomName)
				&& this.numPeople == res.numPeople
				){
			return true;
		}
		return false;
	}
	/**
	 * compare if two reservation will clashes
	 * @param other
	 * @return true if clashes
	 */
	public Boolean reservationClash(Reservation other) {
		if (this.date.timeClash(other.date)){
			return true;
		}
		return false;
	}
	/**
	 * replace the input reservation's date with the date next week
	 * @param res
	 * @return same reservation with replaced date
	 */
	public Reservation getReservationNextWeek(Reservation res) {
		res.date = res.date.getDateNextWeek(res.date);
		return res;
	}
	/**
	 * check if a reservation is valid
	 * @return true if valid
	 */
	public Boolean checkValid (){
		int overtime;
		overtime = this.getDate().getTimeStart() + this.getDate().getDuration();
		if (overtime > dayLimit){
			//System.out.println("start time + duration = end time = "+ overtime + "which doesn't exist (24hr), or go over a day");
			return false;
		}
		return true;
	}

	/**
	 * a series of getFunction which return the data
	 * about this reservation
	 */

	/**
	 * getUser
	 * @return name of User
	 */
	public String getUser() {
		return User;
	}
	/**
	 * getSessionName
	 * @return name of the meeting session
	 */
	public String getSessionName() {
		return sessionName;
	}
	/**
	 * getDate
	 * @return date object of the reservation
	 */
	public DayDealer getDate() {
		return date;
	}
	/**
	 * getRoomName
	 * @return the room that the reservation is scheduled in
	 */
	public String getRoomName() {
		return roomName;
	}
	/**
	 * getNumPeople
	 * @return the number of people in the meeting
	 */
	public int getNumPeople() {
		return numPeople;
	}
}