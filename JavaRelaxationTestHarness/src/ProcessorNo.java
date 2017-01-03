import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;

public class ProcessorNo {

	private int procNo;
	private HashMap<Integer,TestPoint> testPts;
	private Object[] testInOrder;
	
	public ProcessorNo(int procNo){
		
		this.procNo = procNo;
		testPts = new HashMap<Integer,TestPoint>();
	}
	
	public static int GetUniqueNo(int[] pos){
		
		return (int)((0.5 * (pos[0] + pos[1]) * (pos[0] + pos[1] + 1)) + pos[1]);
	}
	
	public int getProcNo(){
		
		return procNo;
	}
	
	public void addRankPoint(RankPoint point){
		
		if(!testPts.containsKey(point.getSize())){
			
			testPts.put(point.getSize(), new TestPoint(point.getSize()));
		}
		
		testPts.get(point.getSize()).checkRankPoint(point);
		
	}
	
	public void calStats(HashMap<Integer,Double> uniTimes){
		
		for(TestPoint pt: testPts.values()){
			
			pt.calStats(procNo, uniTimes.get(pt.getSize()));
		}
	}
	
	public HashMap<Integer,Double> getUniTimes(){
		
		HashMap<Integer,Double> uniTimes = new HashMap<Integer,Double>();
		
		for(TestPoint pt: testPts.values()){
			
			uniTimes.put(pt.getSize(), pt.getMaxTime());
		}
		
		return uniTimes;
	}
	
	public void printRun(){
		
		testInOrder = testPts.values().toArray();
		
		Arrays.sort(testInOrder,new Comparator<Object>(){

			@Override
			public int compare(Object one, Object two) {
				// TODO Auto-generated method stub
				if(((TestPoint)one).getSize() < ((TestPoint)two).getSize()){
					
					return -1;
				}
				
				
				return 0;
			}
			
		});
		
		for(Object test : testInOrder){
			
			System.out.print("No Of Processor: " + procNo + " ");
			((TestPoint)test).printTestPoint();
		}
	}

	public int getNoOfSizes() {
		// TODO Auto-generated method stub
		return testPts.size();
	}

	public int getSizeName(int index) {
		// TODO Auto-generated method stub
		return ((TestPoint)testInOrder[index]).getSize();
	}

	public double getTestStat(int size, int statType) {
		// TODO Auto-generated method stub
		return testPts.get(size).getStat(statType);
	}

	public boolean containsPoint(int size) {
		// TODO Auto-generated method stub
		
		return testPts.containsKey(size);
	}
}
