/**
 * Class store information about reservation
 * and also it does date arrangement
 * due to the spec, the input doesn't contain a year,
 * therefore the system will always assume that the year
 * is 2013, and it is not a leap year.
 */
import java.util.Calendar;
import java.util.GregorianCalendar;
/**
 * @author Jiashu Chen
 * Apr 16, 2013
 */
public class DayDealer {

	/**
	 * Class Fields
	 */
	private int timeStart;
	private int duration;
	private int timeEnd;
	private int date;
	private int month;

	/**
	 *
	 * @param timeStart
	 * @param duration
	 * @param date
	 * @param month
	 */

	/**
	 * year is assumed to be 2013
	 */
	private final int year = 2013;
	private final int fullHour = 0;
	/**
	 * Create a new instance of 'DayDealer'
	 * @param timeStart
	 * @param duration
	 * @param date
	 * @param month
	 */
	public DayDealer(int timeStart, int duration, int date, int month) {
		this.timeStart = timeStart;
		this.duration = duration;
		this.timeEnd = (timeStart + duration)%24;
		this.date = date;
		this.month = month;
	}

	/**
	 * find whether a date is booked or not
	 * Calendar class month range from 0-11 for JAN to DEC
	 * @param other the new booking that doesn't exist
	 * @return true if the new booking clashes
	 */
	public boolean timeClash(DayDealer other){
		Calendar start1 = new GregorianCalendar(this.year, this.getMonth()-1,this.getDate(), this.getTimeStart(), fullHour);
		Calendar end1 = new GregorianCalendar(this.year, this.getMonth()-1,this.getDate(), this.getTimeEnd(), fullHour);
		Calendar start2 = new GregorianCalendar(other.year, other.getMonth()-1, other.getDate(), other.getTimeStart(), fullHour);
		Calendar end2 = new GregorianCalendar(other.year, other.getMonth()-1, other.getDate(), other.getTimeEnd(), fullHour);

		//return 0 if equal
		//return < 0 if this before other
		//return > 0 if this is after other
		int s1tos2 = start1.compareTo(start2);
		int s1toe2 = start1.compareTo(end2);
		int e1tos2 = end1.compareTo(start2);

		if (s1tos2 < 0 && e1tos2 <= 0) {return false;}
		if (s1tos2 > 0 && s1toe2 >= 0) {return false;}
		return true;
	}
//  BAD METHOD USED BEFORE LOOKING INTO JAVA API
//	/**
//	 * this method compare if the starting time clash
//	 * @param day
//	 * @return true if starting time clashses
//	 */
//	public Boolean timeClash(DayDealer day){
//		ArrayList<Integer> thisList = new ArrayList<Integer>();
//
//		int i;
//		for (i = this.getTimeStart(); i < this.getTimeEnd() ;i++){
//			thisList.add(i);
//		}
//
//		for (i = day.getTimeStart(); i < day.getTimeEnd() ;i++){
//			if(this.thisStartTimecontains(i, thisList)){
//				return true;
//			}
//		}
//		return false;
//	}
//	/**
//	 * this method can only be access within the class by startTimeClash
//	 * @param i the number looking for
//	 * @param thisList list of start time
//	 * @return true if starting time clashes
//	 */
//	private boolean thisStartTimecontains(int i, ArrayList<Integer> thisList){
//		int index;
//	    for(index = 0; index < thisList.size(); index++) {
//	    	if(i == thisList.get(index)) return true;
//	    }
//	    return false;
//	}
//	/**
//	* check if two dates are equal
//	* @param day
//	* @return if two date is equal
//	*/
//	public Boolean isEqualDate (DayDealer day){
//		if(this.getMonth() == day.getMonth()
//				&& this.getDate() == day.getDate()){
//				return true;
//				}
//		return false;
//	}

	/**
	* if two Daydealer are equal that is starting time and end time all equal
	* @param day
	* @return true if two object are equal
	*/
	public Boolean isEqual (DayDealer day){
		if(this.getMonth() == day.getMonth()
				&& this.getDate() == day.getDate()
				&& this.getTimeStart() == day.getTimeStart()
				&& this.getTimeEnd() == day.getTimeEnd()){
				return true;
				}
		return false;
	}
	/**
	 * input a date, and it will out put the date of next week
	 * calender class month range from 0-11 for JAN to DEC
	 * @param oldDate
	 * @return newDate
	 */
	public DayDealer getDateNextWeek (DayDealer oldDate){
		int day = oldDate.getDate();
		int month = oldDate.getMonth()-1;
		int startTime = oldDate.getTimeStart();
		Calendar calendar =Calendar.getInstance();
		calendar.set(year, month, day, oldDate.getTimeStart(), 0);
		calendar.add(Calendar.DATE, 7);
		day = calendar.get(Calendar.DAY_OF_MONTH);
		month = calendar.get(Calendar.MONTH) + 1;
		DayDealer newDate = new DayDealer(startTime, oldDate.getDuration(), day, month);
		return newDate;
	}


	/**
	 * a series of methods that return information about this date class
	 * @return required field
	 */
	public int getTimeStart() {
		return timeStart;
	}
	public int getDuration() {
		return duration;
	}
	public int getTimeEnd() {
		return timeEnd;
	}
	public int getDate() {
		return date;
	}
	public int getMonth() {
		return month;
	}
}

