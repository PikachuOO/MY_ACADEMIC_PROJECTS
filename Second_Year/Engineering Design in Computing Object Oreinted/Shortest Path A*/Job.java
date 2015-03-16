/**
 * Every detail every needed for a job
 * the class store
 * the start point of a job
 * the end point of a job
 * the distance of a job
 * the identity of job
 */

/**
 * @author Jiashu Chen
 */
public class Job {
	private final int startX;
	private final int startY;
	private final int endX;
	private final int endY;
	private final int distance;
	private final int jobID;

	public Job(int startX, int startY, int endX, int endY, int ID) {
		this.startX = startX;
		this.startY = startY;
		this.endX = endX;
		this.endY = endY;
		distance = Math.abs(startX - endX) + Math.abs(startY - endY);
		this.jobID = ID;
	}
	/**
	 * @return value of startX
	 */
	public int getStartX() {
		return startX;
	}
	/**
	 * @return value of startY
	 */
	public int getStartY() {
		return startY;
	}
	/**
	 * @return value of endX
	 */
	public int getEndX() {
		return endX;
	}
	/**
	 * @return value of endY
	 */
	public int getEndY() {
		return endY;
	}
	/**
	 * @return value of distance
	 */
	public int getDistance() {
		return distance;
	}
	/**
	 * @return id of the job
	 */
	public int getJobID() {
		return jobID;
	}

	/**
	 * return the cost from one job to another
	 * from the end of this to start job other
	 * @param other the other job that this is going to
	 * @return cost as interger
	 */
	public int thisJobToAnother(Job other){
		return  Math.abs(this.endX - other.startX) + Math.abs(this.endY - other.startY);
	}

	@Override
	/**
	 * check if this job is equal to another on
	 * this job is equal to the other if their ID are equal
	 * @param other
	 * @return true if the two job are the same
	 */
	public boolean equals(Object obj) {
		if (this == obj) return true;
		if (obj == null) return false;
		if (getClass() != obj.getClass()) return false;

		if (obj.getClass() == Job.class) {
			Job other = (Job) obj;
			return this.jobID == other.jobID;
		}
		return false;
	}
	@Override
	/**
	 * print the current job
	 */
	public String toString() {
		String string = ("JobID("+ jobID +") " + startX + " " + startY + " to " + endX + " " + endY + " distance "+ distance);
		return string;
	}
}
