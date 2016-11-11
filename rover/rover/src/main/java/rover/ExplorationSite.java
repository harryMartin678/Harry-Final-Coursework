package rover;

public class ExplorationSite{
	
	public double x;
	public double y;
	public boolean explore;
	
	public ExplorationSite(double x, double y){
		
		explore = false;
		this.x = x;
		this.y = y;
	}
	
	public boolean isInPos(double x, double y) {
		// TODO Auto-generated method stub
		return this.x == x && this.y == y;
	}

	public void setExplored(){
		
		explore = true;
	}
	
	public double[] toArray(){
		
		return new double[]{x,y};
	}
	
	public boolean isExplored(){
		
		return explore;
	}
	
	public double getX(){
		
		return x;
	}
	
	public double getY(){
		
		return y;
	}
	
	public void setXY(double x, double y){
		
		this.x = x;
		this.y = y;
	}
}