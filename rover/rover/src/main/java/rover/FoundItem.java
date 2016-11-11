package rover;

public class FoundItem{
	
	private double x;
	private double y;
	private String name;
	private int worldWidth;
	private int worldHeight;
	
	public FoundItem(double x,double y,String name,
			int worldWidth,int worldHeight){
		
		this.x = x;
		this.y = y;
		//offsetToWorldSize();
		this.name = name;
		this.worldHeight = worldHeight;
		this.worldWidth = worldWidth;
	}
	
	public double[] toArray() {
		// TODO Auto-generated method stub
		return new double[]{x,y};
	}

	public double getX() {
		// TODO Auto-generated method stub
		return x;
	}
	
	public double getY(){
		
		return y;
	}
	
	private void offsetToWorldSize(){
		
		x = x % (double)worldWidth;
		y = y % (double)worldHeight;
		
	}

	public double getDistance() {
		// TODO Auto-generated method stub
		return Math.abs(x) + Math.abs(y);
	}
	

	public void move(double dx,double dy){
		
		x += dx;
		y += dy;
		//offsetToWorldSize();
	}
	
	
	public boolean isResource(){
		
		return name == "resource";
	}
	
	public static double[] getAdjustedMove(double mx, double my,int worldWidth, int worldHeight) {
		// TODO Auto-generated method stub

		double[] minPt = null;
		double minDis = Double.MAX_VALUE;
		for(int h = -1; h <=1; h++){
			for(int w = -1; w<=1; w++){
				
				double[] next = new double[]{mx + (worldHeight * h),
						my + (worldWidth * w)};
				
				double newDis;
				if((newDis = size(next[0],next[1])) < minDis){
					
					minPt = next;
					minDis = newDis;
				}
				
			}
		}
		return minPt;
		
	}
	
	private static double size(double x, double y){
		
		return Math.abs(x) + Math.abs(y);
	}
	
	public static int getKey(double[] pos){
		
		return GetUniqueNo(ToIntArray(pos));
	}
	
	private static int[] ToIntArray(double[] doubleArray) {
		// TODO Auto-generated method stub
		return new int[]{(int)doubleArray[0],(int)doubleArray[1]};
	}
	private static int GetUniqueNo(int[] pos){
		
		return (int)((0.5 * (pos[0] + pos[1]) * (pos[0] + pos[1] + 1)) + pos[1]);
	}
}