import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

public class TestHarness {

	
	public TestHarness(int no) throws IOException{
		
		File file = new File("Correctness" + no + ".txt");
		BufferedReader reader = new BufferedReader(new FileReader(file));
		
		//String text = "";
		String line;
		StringBuilder text = new StringBuilder();
		while((line = reader.readLine()) != null){
			
			//text = text + line + "\n";
			text.append(line + "\n");
		}
		
		
		String[] runStr = text.toString().split("END");
		
		Run[] runs = new Run[runStr.length-1];
		
		for(int r = 0; r < runs.length; r++){

			runs[r] = new Run(runStr[r]);
		}
		
		int currentArraySize = -1;
		int currentProcessors = -1;
		Run compare = null;
		int testNo = 1;
	
		for(int r = 0; r < runs.length; r++){
			
			if(runs[r].getArraySize() != currentArraySize){
				
				System.out.println("Data with Arraysize: " + runs[r].getArraySize());
				compare = runs[r];
				currentArraySize = runs[r].getArraySize();
				currentProcessors = runs[r].getProcessors();
				continue;
			}
			
			if(compare.isDifferent(runs[r])){
				
				System.out.println("Test " + testNo +"(" + runs[r].getProcessors() + ")" + ": " + "Failed");
			
			}else{
				
				System.out.println("Test " + testNo + "(" + runs[r].getProcessors() + ")" + ": " + "Succeeded");
			}
			
			testNo++;
		}
		
		
	}
	
	
	
	public static void main(String[] args) throws IOException {
		
		for(int t = 1; t < 5; t++){
			new TestHarness(t);
			System.out.println("/////////////////////");
		}
	}
}
