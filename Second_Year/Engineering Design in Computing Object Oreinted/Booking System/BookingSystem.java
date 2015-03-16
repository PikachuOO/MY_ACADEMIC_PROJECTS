/**
 *the class contains an ArrayList of rooms,
 *which is used to arrange bookings for each rooms
 *it's act as a communicator between the request and rooms
 */

import java.util.ArrayList;
import java.util.Iterator;
/**
 *
 * @author Jiashu Chen
 * @date Apr 16, 2013
 */

public class BookingSystem {
	/**
	 * Class Field
	 */
	private ArrayList<Room> allRooms;
	/**
	 * Create a new instance of 'BookingSYstem'
	 */
	public BookingSystem() {
		allRooms = new ArrayList<Room>();
	}
	/**
	 * Set up a new Room
	 * @param roomName name of room
	 * @param capacity capacity of room
	 */
	public void setRoom(String roomName, int capacity) {
		Room newRoom = new Room(roomName, capacity);
		allRooms.add(newRoom);
		//System.out.println("Room "+roomName+" is set up, with capacity"+capacity);
	}
	/**
	 * add reservation to a appropriate room
	 * @param newR new reservation
	 * @param weeks number of weeks repeated
	 * @param roomName if roomName is null, then find a appropriate room automatically
	 * add reservation to a appropriate room
	 * @return return true if addition is successful
	 */
	public Boolean addReservation (Reservation newR, int weeks, String roomBooked){
		if(this.allRooms.isEmpty()){
			//System.out.println("Booking Failed ERROR<NO ROOM>");
			return false;
		}
		Iterator<Room> itr = allRooms.iterator();
	    Room temp = allRooms.get(0);
	    Boolean addSuccess = false;
	    //add without a specified room name
	    if(roomBooked == null){
	    	  if(temp.getRoomCapacity() >= newR.getNumPeople()){
	    		  addSuccess = temp.addReservation(newR, weeks);
	    	  }
	    	  while (itr.hasNext() && addSuccess == false){
		    	  temp = itr.next();
		    	  if(temp.getRoomCapacity() >= newR.getNumPeople()){
		    		  addSuccess = temp.addReservation(newR, weeks);
		    	  }
		      }
	     //add with a specified room name
	      }else {
	    	  if(temp.getRoomName().equals(roomBooked)){
	    		  addSuccess = temp.addReservation(newR, weeks);
	    	  }
	    	  while (itr.hasNext() && addSuccess == false){
		    	  if(temp.getRoomName().equals(roomBooked)){
		    		  addSuccess = temp.addReservation(newR, weeks);
		    	  }
		    	  temp = itr.next();
		      }
	      }
	      return addSuccess;
	}
	/**
	 * delete already existing reservation
	 * @param old old reservation
	 * @param roomName room deleting from
	 * @param room deleting
	 * @return a list of reservation deleted
	 */
	public ArrayList<Reservation> deleteReservation(Reservation old, String roomName, int weeks){
		//there has to be rooms
	    //System.out.println("Try to delete reservation for" +weeks);

		if(this.allRooms.isEmpty()) return null;
		ArrayList<Reservation> deleted = new ArrayList<Reservation>();
		Iterator<Room> itr = allRooms.iterator();
	    Room temp = allRooms.get(0);
	    Reservation tempRes = old;
	    Reservation tempRet = null;
	    int i;
	    //get the right room
	    while (itr.hasNext() && !roomName.equals(temp.getRoomName()) ){
	    	  temp = itr.next();
	    }
	    if(temp.getRoomName().equals(roomName)){
	    	for (i=0; i < weeks; i++){
	    		if(temp.checkCanDelete(tempRes)){
	    			tempRet = temp.deleteParticularReservation(tempRes);
		    		deleted.add(tempRet);
	    		}
	    		tempRes.getReservationNextWeek(tempRes);
			}
		}
		return deleted;
	}
	/**
	 * change reservation from a old one to a new one
	 * by deleting the old ones and add the new reservation in
	 * @param old existing reservation
	 * @param newR new changes
	 * @param weeks number off weeks
	 */
	public void changeReservation (Reservation old, Reservation newR, int weeks){
		if(this.allRooms.isEmpty()) return;
		ArrayList<Reservation> deleted = this.deleteReservation(old, old.getRoomName(), weeks);
		Reservation temp = null;
		int i;
		//can't change reservation that doesn't exist
		if (!deleted.isEmpty()){
			//restore the room if addition of new reservation makes clashes
			if(!this.addReservation(newR, weeks, null)){
				for(i=0; i < weeks; i++){
					 Iterator<Reservation> itr = deleted.iterator();
					 temp = deleted.get(0);
					 while(itr.hasNext()){
						 this.addReservation(temp, 1, temp.getRoomName());
						 temp = itr.next();
					 }
				}
			}
		}else{
			System.out.println("Change Rejected");
		}
	}
	/**
	 * print all reservations stored in a room
	 * @param roomName
	 */
	public void printReservationOfARoom (String roomName){
		Iterator<Room> itr = allRooms.iterator();
		if(this.allRooms.isEmpty()){
			//System.out.println("Print Faild Error <No Room>");
			return;
		}
	    Room temp = allRooms.get(0);
	    //get the right room
	    while (itr.hasNext() && !roomName.equals(temp.getRoomName()) ){
	    	  temp = itr.next();
	    }
	    if(temp.getRoomName().equals(roomName)){
			temp.printReservationInRoom();
		}
	}
}
