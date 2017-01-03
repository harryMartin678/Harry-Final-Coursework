import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import javax.sound.sampled.Line;

public class Segment {

	private int rank;
	private Line[] lines;
	
	public Segment(String str,boolean isFirst){
		
		String[] linesStr = str.split("\n");
		
		this.rank = new Integer(linesStr[0]).intValue();
		
		
		int offsetF = 1;
		
		if(isFirst){
			
			offsetF = 0;
		}
		
		this.lines = new Line[linesStr.length-2-offsetF];
		
		for(int l = 1 + offsetF; l < linesStr.length-1; l++){
			
			this.lines[l-1-offsetF] = new Line(linesStr[l],l,this.rank); 
		}
		
		
	}
	
	public int getRank(){
		
		return rank;
	}
	
	public class Line{
		
		private double[] numbers;
		private int lineNo;
		private int rankNo;
		
		public Line(String str,int lineNo,int rankNo){
			
			String[] numStr = str.split(" ");
			numbers = new double[numStr.length];
			this.lineNo = lineNo;
			this.rankNo = rankNo;
			
			for(int n = 0; n < numStr.length; n++){
				
				numbers[n] = new Double(numStr[n]).doubleValue();
			}
		}
		
		public int getLineNo(){
			
			return lineNo;
		}
		
		public int getRankNo(){
			
			return rankNo;
		}
		
		public double getNum(int index){
			if(numbers.length <= index){
				
				for(int n = 0; n < numbers.length; n++){
					
					System.out.println(numbers[n]);
				}
			}
			return numbers[index];
		}
		
		public boolean compareLine(Line line){
			
			for(int n = 0; n < numbers.length; n++){
				
				if(numbers[n] != line.getNum(n)){
					
					System.out.println("Rank: " + line.getRankNo() + " Line No: " + line.getLineNo() +
					 " Number: " + n + " " + numbers[n] + " " + line.getNum(n));
					return true;
				}
			}
			
			return false;
		}

		public int getSize() {
			// TODO Auto-generated method stub
			return numbers.length;
		}
		
		
		public void printLine(){
			
			for(int l = 0; l < numbers.length; l++){
				
				System.out.print(numbers[l] + " ");
			}
			
			System.out.println();
		}
	}

	public boolean compareSeg(Segment seg) {
		// TODO Auto-generated method stub
		for(int n = 0; n < lines.length; n++){
			
			if(this.lines[n].compareLine(seg.getLine(n))){
				
				return true;
			}
		}
		
		return false;
	}

	private Line getLine(int lineNo) {
		// TODO Auto-generated method stub
		return lines[lineNo];
	}

	
	public int getSize() {
		// TODO Auto-generated method stub
		return lines.length;
	}

	public HashMap<Integer, int[]> getLinesCounts(int segNo,int lineNo) {
		// TODO Auto-generated method stub
		HashMap<Integer,int[]> maps = new HashMap<Integer,int[]>();
		
		for(int l = 0; l < lines.length; l++){
			
			maps.put(lineNo + l, new int[]{segNo,l});
		}
		
		return maps;
	}

	public double getIndex(int i, int y) {
		// TODO Auto-generated method stub
		return lines[i].getNum(y);
	}


	public Line[] getLines() {
		// TODO Auto-generated method stub
		return lines;
	}
}
