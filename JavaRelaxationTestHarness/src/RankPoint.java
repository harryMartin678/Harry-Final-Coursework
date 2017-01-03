
public class RankPoint {

	private int procNo;
	private int size;
	private double time;
	
	public RankPoint(String line){
		
		String[] rank = line.split(" ");
		
		procNo = new Integer(rank[0].split(":")[1]);
		size = new Integer(rank[1].split(":")[1]);
		time = new Double(rank[3].split(":")[1]);
		
	}
	
	public int getSize(){
		
		return size;
	}
	
	public int getProcNo(){
		
		return procNo;
	}
	
	public double getTime(){
		
		return time;
	}
}
