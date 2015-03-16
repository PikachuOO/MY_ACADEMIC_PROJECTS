/**
 * This store a list of jobs, and also generate
 * have other cost calculating methods
 * e.g. cost from job1 to job2
 * 32 jobs at maximum
 */
import java.util.ArrayList;
import java.util.BitSet;
import java.util.Collections;
import java.util.Comparator;
import java.util.PriorityQueue;

/**
 * @author Jiashu Chen
 */
public class JobGraph {
	private static final int size = 32;
	private ArrayList<Job> jobs;
	private State best;
	/**
	 * The constructor construct the number of jobs
	 */
	public JobGraph() {
		jobs = new ArrayList<Job>(size);
	    best = null;
	}
	/**
	 * add job to the job graph
	 * @param add the job being added
	 * @return true if successful
	 */
	public boolean addJob(Job add) {
		if(add.getJobID() > 31) {
			return false;
		}
		jobs.add(add.getJobID(), add);
		return true;
	}

	/**
	 * returns the job list
	 * @return list of job
	 */
	public ArrayList<Job> getJobs() {
		return jobs;
	}
	@Override
	public String toString() {
		StringBuffer string = new StringBuffer();
		for(Job job : jobs) {
			string.append(job.toString()+ "\n");
		}
		return string.toString();
	}
	/**
	 * Perform A star search on the current graph
	 */
	public void pathGenerator(){
		if(this.jobs.size() == 0) return;
		//the origin point
		int heuristic = 0;
		int graphSize = this.jobs.size();
		int explored = 0;
		int functionalCost = 0;

		Job origin = new Job(0,0,0,0,0);

		ArrayList<State> tracking = new ArrayList<State>();
		PriorityQueue<State> stateQueue = new PriorityQueue<State>(1,stateCompare);

		BitSet temp = new BitSet(graphSize);
		temp.clear();
        State originState = new State(origin, functionalCost, heuristic, null, temp);
        stateQueue.add(originState);

		while(this.best == null){
            State expandable = stateQueue.poll();
            tracking.add(expandable);
            explored++;
            //check for ending;
            if(expandable.getVisited().cardinality() == graphSize){
            	this.best = expandable;
            }else {
                for(Job looking : this.jobs){
                    if( (!expandable.getVisited().get(this.jobs.indexOf(looking)))){
                    	functionalCost = expandable.getFunctionalCost() + expandable.getCurr().thisJobToAnother(looking)
                    			+looking.getDistance();
                        BitSet update = (BitSet) expandable.getVisited().clone();
                        update.set(looking.getJobID());
                        heuristic = heuristicCal(update, looking);
                        State newState = new State(looking, functionalCost, heuristic, expandable, update);
                        stateQueue.add(newState);
                    }
                }
                expandable.setVisitedToNull();
            }
		}

		ArrayList<Job> route = generatePath(tracking);
		String string = pathToString(route, explored);
		System.out.println(string);
	}
	/**
	 * Compare two states by their total weight
	 */
	Comparator<State> stateCompare = new Comparator<State>() {
		@Override
		public int compare(State state1, State state2) {
			if(state1.getTotalCost() < state2.getTotalCost()) return -1;
			if(state1.getTotalCost() > state2.getTotalCost()) return 1;
			return 0;
		}
	};
	/**
	 * generate a path form the states looked by a star search
	 * @param track List of states that the A star search used
	 * @return list of jobs that the route goes through
	 */
	private ArrayList<Job> generatePath(ArrayList<State> track){
		ArrayList<Job> route = new ArrayList<Job>();
		if(this.best == null) {
			System.out.println("No PATH");
		}
		State current = track.get(track.size()-1);
		route.add(0, current.getCurr());
		while(current.getParents() != null){
			current = current.getParents();
			route.add(0, current.getCurr());
		}
		return route;
	}
	/**
	 * turn the path generated into a string
	 * @param route list job should travel
	 * @param n number of nodes explored
	 * @return path in a string
	 */
	private String pathToString(ArrayList<Job> route, int n) {
		StringBuffer string = new StringBuffer();
		string.append(n + " nodes explored\ncost = "+ this.best.getFunctionalCost());
		route.remove(0);
		Job prev = new Job(0,0,0,0,0);
		for(Job current : route) {
			if(prev.getEndX() != current.getEndY() && prev.getEndY() != current.getStartY()){
			string.append("\nMove from "+prev.getEndX() +" "+ prev.getEndY() + " to "+ current.getStartX() +" "+ current.getStartY());
			}
			string.append("\nCarry from "+current.getStartX() +" "+ current.getStartY() + " to "+ current.getEndX() +" "+ current.getEndY());
			prev = current;
		}
		return string.toString();
	}
	/**
	 * calculating the heuristic
	 * @param check jobs that already looked
	 * @param job the job generate from
	 * @return value of the heuristic
	 */
	private int heuristicCal(BitSet check, Job job){
		if(check.cardinality() == check.size()) return 0;
		if(check.cardinality() == check.size()-1) return job.getDistance();
		ArrayList<Integer> heuristic = new ArrayList<Integer>();
		int restJob = 0;
		int value = 0;
//		System.out.println(check.toString());
		for(Job curr : this.jobs){
			if(!(check.get(curr.getJobID())))	{
			  heuristic.add((int) job.thisJobToAnother(curr));
			  restJob += curr.getDistance();
			}
		}
		if(!heuristic.isEmpty()){
	        value = Collections.min(heuristic);
		}
		return value+restJob;
	}
}
