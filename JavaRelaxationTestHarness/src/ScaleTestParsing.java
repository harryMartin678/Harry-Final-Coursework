import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;

public class ScaleTestParsing {

	
	public ScaleTestParsing() throws IOException{
		
		BufferedReader reader = new BufferedReader(new FileReader(new File("ScaleResults.txt")));
		
		HashMap<Integer,ProcessorNo> procs = new HashMap<Integer,ProcessorNo>();
		//StringBuilder sb = new StringBuilder();
		
		String line;
		while((line = reader.readLine()) != null){
			
			//sb.append(line + "\n");
			RankPoint rank = new RankPoint(line);
			
			if(!procs.containsKey(rank.getProcNo())){
				
				procs.put(rank.getProcNo(), new ProcessorNo(rank.getProcNo()));
			
			}
			procs.get(rank.getProcNo()).addRankPoint(rank);
		
		}
		
		HashMap<Integer,Double> uniTimes = procs.get(1).getUniTimes();
		
		
		Object[] sortedProcs = procs.values().toArray();
		Arrays.sort(sortedProcs,new Comparator<Object>(){

			@Override
			public int compare(Object one, Object two) {
				// TODO Auto-generated method stub
				if(((ProcessorNo)one).getProcNo() > ((ProcessorNo)two).getProcNo()){
					
					return 0;
				
				}
				
				return -1;
			}
			
			
		});
		for(Object proc : sortedProcs){
			
			((ProcessorNo)proc).calStats(uniTimes);
			((ProcessorNo)proc).printRun();
		}
		
		new ScaleGraph("Speed Up","Speed Up","Number of Processors",procs,TestPoint.SPEEDUP,new int[]{});
		new ScaleGraph("Efficiency","Efficiency","Number of Processors",procs,TestPoint.EFFICIENCY,new int[]{});
		new ScaleGraph("Karp-Flatt","Karp-Flatt","Number of Processors",procs,TestPoint.KARPFLATT,new int[]{});
		new ScaleGraph("Karp-Flatt","Karp-Flatt","Number of Processors",procs,TestPoint.KARPFLATT,new int[]{50,100});
		
		//System.out.println(sb.toString());
	}
	
	public static void main(String[] args) {
		
		try {
			new ScaleTestParsing();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
