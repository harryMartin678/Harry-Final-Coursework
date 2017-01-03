import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;


public class Run {

	private Segment[] segs;
	private int arraySize;
	private int processors;
	private ArrayList<Segment.Line> lines;
	
	public Run(String text){
		
		
		String[] processorsStr = text.split("PROCESSORS:");
		String[] runSize = processorsStr[1].split("SIZE:");
		arraySize = new Integer(runSize[1].substring(0,runSize[1].length()-1)).intValue();
		processors = new Integer(runSize[0].substring(0,runSize[0].length()-1)).intValue();
		String[] segStr = processorsStr[0].split("Rank:");

		segs = new Segment[segStr.length-1];
		
		for(int s = 1; s < segStr.length; s++){

			segs[s-1] = new Segment(segStr[s],s==1);
		}
		
		Arrays.sort(segs,new Comparator<Segment>(){

			@Override
			public int compare(Segment s1, Segment s2) {
				// TODO Auto-generated method stub

				if(s1.getRank() < s2.getRank()){
					
					return -1;
				
				}else{
					
					return 0;
				}
			}

			
			
		});
		
		lines = new ArrayList<Segment.Line>();
		
		for(int s = 0; s < segs.length; s++){
			
			
			for(int l = 0; l < segs[s].getSize(); l++){
				
				lines.add(segs[s].getLines()[l]);
			}
		}

	}
	
	public int getArraySize(){
		
		return arraySize;
	}
	
	public int getProcessors(){
		
		return processors;
	}
	
	public void printRun(int range,int cx,int cy){
		
		for(int x = Math.max(cx - range,0); x < Math.min(cx + range,lines.size()); x++){
			for(int y = Math.max(cy - range,0); y < Math.min(cy + range,lines.get(x).getSize()); y++){
				
				if(x == cx && y == cy){
					
					System.out.print(" |" + lines.get(x).getNum(y) + "|");
				}else{
					
					System.out.print(lines.get(x).getNum(y) + " ");
				}
				
			}
			System.out.println();
		}
	}
	
	public double getValue(int x, int y){
		
		return lines.get(x).getNum(y);
	}
	
	public boolean isDifferent(Run other){
		
		for(int x = 0; x < lines.size(); x++){
			for(int y = 0; y < lines.get(x).getSize(); y++){
				
				if(other.getValue(x, y) != this.getValue(x, y)){
					
					return true;
				}
			}
		}
		
		return false;
	}
	
	/*public boolean isDifferent(Run run){
		
		for(int s = 0; s < segs.length; s++){
			
			if(segs[s].compareSeg(run.getSeg(s))){
				
				return true;
			}
		}
		
		return false;
	}*/

	private Segment getSeg(int segNo) {
		// TODO Auto-generated method stub
		return segs[segNo];
	}
}
