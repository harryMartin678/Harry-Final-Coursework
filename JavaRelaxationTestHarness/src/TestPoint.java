
public class TestPoint {

	private int size;
	private double maxTime;
	private double kf;
	private double eff;
	private double speedup;
	
	public static final int SPEEDUP = 0;
	public static final int EFFICIENCY = 1;
	public static final int KARPFLATT = 2;
	
	public TestPoint(int size){
		
		this.size = size;
		maxTime = -1.0;
	}
	
	public int getSize(){
		
		return size;
	}

	public void checkRankPoint(RankPoint point) {
		// TODO Auto-generated method stub
		if(point.getTime() > maxTime){
			
			maxTime = point.getTime();
		}
	}
	
	private void karpflatt(double threads){

		kf =  ((1.0/speedup) - (1.0/threads)) / (1.0 - (1.0/threads));
	}

	private void Efficiency(double threads){

		eff = speedup / threads;
	}

	private void SpeedUp(double uni){

		speedup =  uni / this.maxTime;
	}
	
	public double getMaxTime(){
		
		return maxTime;
	}
	
	public void calStats(int noOfProcs, double uniTime){
		
		SpeedUp(uniTime);
		Efficiency(noOfProcs);
		karpflatt(noOfProcs);
		
	}

	public void printTestPoint() {
		// TODO Auto-generated method stub
		System.out.println("Size: " + size + " Time: " + maxTime + " Speedup: " + speedup
				+ " Efficieny: " + eff + " Karp-Flatt: " + kf);
	}

	public double getStat(int statType) {
		// TODO Auto-generated method stub
		if(statType == SPEEDUP){
			
			return speedup;
		
		}else if(statType == KARPFLATT){
			
			return kf;
		
		}else if(statType == EFFICIENCY){
			
			return eff;
		}
		
		return -1.0;
	}
	
	
}
