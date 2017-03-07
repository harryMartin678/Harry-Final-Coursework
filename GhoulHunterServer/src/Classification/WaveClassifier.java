package Classification;

import java.util.ArrayList;

public class WaveClassifier {

	private ArrayList<ArrayList<Integer>> objects;
	private ArrayList<FeatureClass> featureClasses;
	
	public WaveClassifier(ArrayList<ArrayList<Integer>> objects){
		
		featureClasses = new ArrayList<FeatureClass>();
		this.objects = objects;
		
		int averageSteepness = 0;
		
		for(int f = 0; f < objects.get(0).size(); f++){
			
			featureClasses.add(new FeatureClass(objects,f));
			averageSteepness += featureClasses.get(featureClasses.size()-1).getSteepness();
		}
		averageSteepness /= featureClasses.size();
		
		//for(int f = 0; f < featureClasses.size(); f++){
			
			//System.out.println(featureClasses.get(f).getSteepness()
				//	+ " " + featureClasses.get(f).getWaveAvg());
	//	}
		
		double score = Double.MIN_VALUE;
		int scoreNo = 0;
		
		
		for(int f = 0; f < featureClasses.size(); f++){
		
			if(featureClasses.get(f).getScore() > score){
				
				scoreNo = f;
				score = featureClasses.get(f).getScore();
			}
			
		}
		
		int stddev = 0;
		
		for(int f = 0; f < featureClasses.size(); f++){
			
			stddev = (int) Math.pow(featureClasses.get(f).getSteepness() - averageSteepness, 2);
		}
		
		stddev = (int) Math.sqrt(stddev/featureClasses.size());
		
		for(int f = 0; f < featureClasses.size(); f++){
			
			if(featureClasses.get(f).getSteepness() < averageSteepness + stddev){
				
				featureClasses.get(f).setIgnore();
			}
		}
		
		System.out.println(score);
		featureClasses.get(scoreNo).printFeatureClass();
		
		featureClasses.get(5).labelFeatureDensity();
		
		//System.out.println(featureClasses.size());
		
	}
	
	private Object getClasses(ArrayList<Integer> object,int objNo){
		
		ArrayList<Wave> waves = new ArrayList<Wave>();
		
		for(int f = 0; f < object.size(); f++){
			
			if(!featureClasses.get(f).ignore()){
				waves.add(featureClasses.get(f).classify(object.get(f)));
			}
		}
		
		//for(int f = 0; f < featureClasses.size(); f++){
			
			//Wave wave = 
		//}
		
		return new Object(waves,objNo);
	}
	
	public class FeatureClass{
		
		private int[] featureDensity;
		private ArrayList<Wave> waves;
		private double Steepness;
		private double avgWaveDist;
		private double gapSize;
		private int min;
		private boolean ignore;
		
		public FeatureClass(ArrayList<ArrayList<Integer>> objects, int featureNo){
			
			int[] minMax = findMinMax(objects,featureNo);
			min = minMax[1];
			waves = new ArrayList<Wave>();
			//ignore = false;
			
			this.gapSize = (2*((minMax[0] - minMax[1]) / (double)objects.size()));
			
			if(!(minMax[0] == 0 && minMax[1] == 0)){
				featureDensity = new int[(int) (((minMax[0] - minMax[1])/gapSize)+1)];
				//featureDensity = new int[objects.size()];
				
				//System.out.println(minMax[0] + " " + minMax[1]);
				
				for(int b = 0; b < objects.size(); b++){
					
					double index = (objects.get(b).get(featureNo) - minMax[1]) / gapSize;
					featureDensity[(int)index]++;
					//featureDensity[b] = objects.get(b).get(featureNo);
				}
				
				double avg = getAverage(featureDensity);
				double var = getVariance(featureDensity, avg);
				
				//System.out.println("Feature start");
				int next = 0;
				while(next < featureDensity.length){
					
					waves.add(new Wave(featureDensity,next,var));
					next = waves.get(waves.size()-1).getEnd()+1;
					Steepness += waves.get(waves.size()-1).getSteepness();
				}
				
				for(int w = 1; w < waves.size(); w++){
					
					avgWaveDist += waves.get(w).getCenter() - waves.get(w-1).getCenter();
				}
				
				avgWaveDist /= waves.size()/2;
				
				Steepness /= waves.size();
				
			//System.out.println("Feature end");
			}else{
				
				setIgnore();
			}
		}
		
		public void setIgnore() {
			// TODO Auto-generated method stub
			ignore = true;
		}
		
		public boolean ignore(){
			
			return ignore;
		}

		public Wave classify(Integer pos) {
			// TODO Auto-generated method stub
			for(int w = 0; w < waves.size(); w++){
				
				System.out.println(pos + " " + Math.round(((waves.get(w).getStart() * gapSize) + min))
						+ " " + Math.round(((waves.get(w).getEnd() * gapSize) + min))
						+ " " + gapSize + " " + min);
				//System.out.println(pos + " " + waves.get(w).getEnd());
				if(pos <= Math.round(((waves.get(w).getEnd() * gapSize) + min))){
				//if(pos <= waves.get(w).getEnd()){
					//System.out.println(w);
					return waves.get(w);
				}
			}
			System.out.println("Ret null: " + pos + " waves size: " + waves.size());
			return null;
		}

		public double getWaveAvg() {
			// TODO Auto-generated method stub
			return avgWaveDist;
		}

		public double getFeatureSize() {
			// TODO Auto-generated method stub
			return featureDensity.length;
		}

		public double getSteepness(){
			
			return Steepness;
		}
		
		public double getScore(){
			
			return Steepness * avgWaveDist;
		}
		
		public void labelFeatureDensity(){
			
			int currentWave = 0;
			for(int f = 0; f < featureDensity.length && currentWave < waves.size(); f++){
				
				if(f == waves.get(currentWave).getStart()){
					
					System.out.print("S:");
				
				}else if(f == waves.get(currentWave).getEnd()){
					
					System.out.print("E(" + waves.get(currentWave).getAmplitude()+"):");
					currentWave++;
				
				}else if(f == waves.get(currentWave).getCenter()){
					
					System.out.print("C:");
				}
				
				System.out.print(featureDensity[f] + " ");
			}
			System.out.println();
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
	
	public ArrayList<ArrayList<Integer>> classify(ArrayList<Integer> selection) {
		// TODO Auto-generated method stub
		Object[] objs = new Object[objects.size()];

		for(int c = 0; c < objs.length; c++){
			
			objs[c] = getClasses(objects.get(c),c);
		}

		SuperClass[] superClasses = new SuperClass[selection.size()];
		
		for(int s = 0; s < selection.size(); s++){
			
			superClasses[s] = new SuperClass(objs[selection.get(s)]);
		}
		
		for(int b = 0; b < objs.length; b++){
			int maxScore = Integer.MIN_VALUE;
			int choice = 0;
			//System.out.println("Start!!!");
			for(int r = 0; r < superClasses.length; r++){
				
				int score = superClasses[r].scoreObj(objs[b]);
				//System.out.println(score + " " + maxScore);
				if(score > maxScore){
					
					maxScore = score;
					choice = r;
				}
			}
			//System.out.println("End!!!");
			superClasses[choice].addObj(objs[b]);
		}
		
		ArrayList<ArrayList<Integer>> superClassChoices = new ArrayList<ArrayList<Integer>>();
		
		for(int s = 0; s < superClasses.length; s++){
			
			System.out.println(superClasses[s].getSize());
			superClassChoices.add(superClasses[s].getBackObjsNos());
		}
		
		return superClassChoices;
		
	}
	
	public class SuperClass{
		
		private Object repres;
		private ArrayList<Object> objects;
		
		public SuperClass(Object repres){
			
			this.repres = repres;
			objects = new ArrayList<Object>();
			
		}
		
		public ArrayList<Integer> getBackObjsNos(){
			
			ArrayList<Integer> objNos = new ArrayList<Integer>();
			
			for(int o = 0; o < objects.size(); o++){
				
				objNos.add(objects.get(o).getObjNo());
			}
			
			return objNos;
		}
		
		public int getSize() {
			// TODO Auto-generated method stub
			return objects.size();
		}

		public void addObj(Object obj) {
			// TODO Auto-generated method stub
			objects.add(obj);
		}

		public int scoreObj(Object obj){
			
			return repres.compare(obj);
		}
	}
	
	public class Object{
		
		private ArrayList<Wave> classes;
		private int objNo;
		
		public Object(ArrayList<Wave> classes,int objNo){
			
			this.classes = classes;
			this.objNo = objNo;
		}

		public Integer getObjNo() {
			// TODO Auto-generated method stub
			return objNo;
		}

		public int compare(Object obj) {
			// TODO Auto-generated method stub
			int score = 0;
			
			for(int c = 0; c < classes.size(); c++){
				
				if(obj.classes.get(c).center == this.classes.get(c).center
						&& !featureClasses.get(c).ignore()){
					
					score ++;
				}
			}
			
			return score;
		}
	}
	
	public class Wave{
		
		private int start;
		private int amplitude; 
		private int end;
		private int center;
		private int average;
		private double steepness;
		
		public Wave(int[] featureDensity,int start,double var){
			
			this.start = start;
			createWave(featureDensity,var);
			steepness = (amplitude - average);
			
		}
		
		public int getCenter() {
			// TODO Auto-generated method stub
			return center;
		}

		public int getStart() {
			// TODO Auto-generated method stub
			return start;
		}
		
		public double getSteepness(){
			
			return steepness;
		}

		private void createWave(int[] featureDensity,double var){
			
			boolean prePeak = true;
			double diff = 0;
			
			int f = start;
			while(true){
				
				if(f >= featureDensity.length){
				
					this.end = f-1;
					break;
					
				}else if(prePeak && diff > var){
					
					prePeak = false;
					this.center = f;
				
				}else if(diff > var){
					
					this.end = f;
					break;
				}
				
				this.amplitude = Math.max(this.amplitude, featureDensity[f]);
				average += featureDensity[f];
				f++;
				if(f < featureDensity.length){
					diff = Math.abs(featureDensity[f] - featureDensity[f-1]);
				}
			}
			average /= featureDensity.length;
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
