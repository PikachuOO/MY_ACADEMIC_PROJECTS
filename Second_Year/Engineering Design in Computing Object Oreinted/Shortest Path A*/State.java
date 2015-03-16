/**
 * Store the state of the A*Search
 */
import java.util.BitSet;

public class State{
	//Fields
	private int totalCost;
	private int functionalCost;
	private int heuristicCost;
	private Job curr;
	private State parents;
	private BitSet visited;
	/**
	 * 
	 * @param current current job
	 * @param functionalCost total function cost so far
	 * @param heuristicCost heuristic cost
	 * @param visited the closed set so far for this state
	 */
	public State(Job current, int functionalCost, int heuristicCost, State parents, BitSet visited) {
	    this.curr = current;
	    this.totalCost = heuristicCost + functionalCost;
	    this.functionalCost = functionalCost;
	    this.heuristicCost = heuristicCost;
	    this.parents = parents;
	    this.visited = visited;
	 }

	/**
	 * @return return the current job node
	 */
	public Job getCurr() {
		return curr;
	}
	/**
	 * @return return the current functional cost
	 */
	public int getFunctionalCost() {
		return functionalCost;
	}
	/**
	 * @return return the heuristic Cost
	 */
	public int getHeuristicCost() {
		return heuristicCost;
	}
	/**
	 * @return return total cost
	 */
	public int getTotalCost() {
		return totalCost;
	}
	/**
	 * @return the route
	 */
	public State getParents() {
		return parents;
	}
	
	/**
	 * @return visited bit set
	 */
	public BitSet getVisited() {
		return visited;
	}
	/**
	 * change the current visited in this state to null
	 */
	public void setVisitedToNull(){
		this.visited = null;
	}
	@Override
	public String toString() {
		StringBuffer string = new StringBuffer();
		string.append("After Going through "+ this.visited.cardinality() + " jobs, with cost " +this.functionalCost+"\n");
		string.append(this.curr.toString()+"\n");
		if(this.parents != null){
			string.append("with parents " + this.parents.getCurr().toString()+"\n\n");
		}else {
			string.append("these are parents\n\n");
		}
		return string.toString();
	}
}
