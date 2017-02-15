package Classification;

import java.util.ArrayList;

public class WaveClassifier {

	public WaveClassifier(ArrayList<ArrayList<Integer>> objects){
		
		ArrayList<FeatureClass> featureClasses = new ArrayList<FeatureClass>();
		
		for(int f = 0; f < objects.get(0).size(); f++){
			
			featureClasses.add(new FeatureClass(objects,f));
		}
		
		
		for(int f = 0; f < featureClasses.size(); f++){
			
			System.out.println(featureClasses.get(f).getWaveSize());
		}
		
		//System.out.println(featureClasses.size());
		
	}
	
	public class FeatureClass{
		
		private int[] featureDensity;
		private ArrayList<Wave> waves;
		
		public FeatureClass(ArrayList<ArrayList<Integer>> objects, int featureNo){
			
			int[] minMax = findMinMax(objects,featureNo);
			
			waves = new ArrayList<Wave>();
			
			double gapSize = (2*((minMax[0] - minMax[1]) / (double)objects.size()));
			
			featureDensity = new int[(int) (((minMax[0] - minMax[1])/gapSize)+1)];
			
			for(int b = 0; b < objects.size(); b++){
				
				double index = (objects.get(b).get(featureNo) - minMax[1]) / gapSize;
				
				featureDensity[(int)index]++;
			}
			
			double avg = getAverage(featureDensity);
			double var = getVariance(featureDensity, avg);
			
			int next = 0;
			while(next < featureDensity.length){
				
				waves.add(new Wave(featureDensity,next,var));
				next = waves.get(waves.size()-1).getEnd()+1;
			}
		}
		
		private double getVariance(int[] featureDensity,double avg){
			
			double var = 0.0;
			
			for(int f = 0; f < featureDensity.length; f++){
				
				var += Math.pow(featureDensity[f]-avg, 2);
			}
			
			var = Math.sqrt(var/featureDensity.length);
			
			return var;
		}
		
		private double getAverage(int[] featureDensity){
			
			double avg = 0.0;
			
			for(int f = 0; f < featureDensity.length; f++){
				
				avg += featureDensity[f];
			}
			
			return avg/featureDensity.length;
		}
		
		public void printFeatureClass(){
			
			for(int f = 0; f < featureDensity.length; f++){
				
				System.out.print(featureDensity[f] + " ");
			}
			System.out.println();
		}
		
		public int getWaveSize(){
			
			return waves.size();
		}

		private int[] findMinMax(ArrayList<ArrayList<Integer>> objects, int featureNo) {
			// TODO Auto-generated method stub
			int max = Integer.MIN_VALUE;
			int min = Integer.MAX_VALUE;
			
			for(int b = 0; b < objects.size(); b++){
				
				max = Math.max(objects.get(b).get(featureNo) , max);
				min = Math.min(objects.get(b).get(featureNo) , min);
			}
			
			return new int[]{max,min};
		}
	}
	
	public class Wave{
		
		private int start;
		private int amplitude; 
		private int end;
		private int center;
		
		public Wave(int[] featureDensity,int start,double var){
			
			this.start = start;
			createWave(featureDensity,var);
			
		}
		
		private void createWave(int[] featureDensity,double var){
			
			boolean prePeak = true;
			double diff = 0;
			
			int f = 0;
			while(true){
				
				
				if(f >= featureDensity.length){
				
					this.end = f;
					break;
					
				}else if(prePeak && diff > var){
					
					prePeak = false;
					this.center = f;
				
				}else if(diff > var){
					
					this.end = f;
					break;
				}
				
				this.amplitude = Math.max(this.amplitude, featureDensity[f]);
				f++;
			}
		}
		
		public int getEnd(){
			
			return end;
		}
		
		public int getAmplitude(){
			
			return amplitude;
		}
		
		public int getSize(){
			
			return end - start;
		}
	}
	
}
