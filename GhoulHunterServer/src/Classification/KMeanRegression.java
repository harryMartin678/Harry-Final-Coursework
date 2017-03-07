package Classification;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Random;

public class KMeanRegression {
	
	private ArrayList<Dimension> indFeatures;
	
	public KMeanRegression(ArrayList<ArrayList<Integer>> objects,boolean doCorrelationTesting){

		ArrayList<Dimension> thisLayer = getIntialDimensions(objects);
		indFeatures = new ArrayList<Dimension>();
		
		System.out.println("Layer Size: " + thisLayer.size());
		
		if(doCorrelationTesting){
			do{
				
				thisLayer = createLayer(thisLayer,indFeatures);
				System.out.println("Layer Size: " + thisLayer.size());
				
			}while(thisLayer.size() > 0 && thisLayer.size() < 1000);
			
			System.out.println("No of Dimensions: " + indFeatures.size());
			
		}else{
			
			indFeatures = thisLayer;
		}
		
	}
	public KMeanRegression(){
		
	}
	
	private ArrayList<Dimension> getIntialDimensions(ArrayList<ArrayList<Integer>> objects) {
		// TODO Auto-generated method stub
		
		ArrayList<ArrayList<Integer>> featuresList = new ArrayList<ArrayList<Integer>>();
		
		for(int f = 0; f < objects.get(0).size(); f++){
			ArrayList<Integer> list = new ArrayList<Integer>();
			for(int h = 0; h < objects.size(); h++){
				
				list.add(objects.get(h).get(f));
			}
			
			featuresList.add(list);
		}
		
		ArrayList<Dimension> dimensions = new ArrayList<Dimension>();
		
		for(int f = 0; f < featuresList.size(); f++){
			
			dimensions.add(new Dimension(featuresList.get(f),f));
		}
		
		return dimensions;
	}
	
	public ArrayList<Dimension> createLayer(ArrayList<Dimension> lastLayer,
			ArrayList<Dimension> indFeatures){
		
		ArrayList<Dimension> layer = new ArrayList<Dimension>();
		
		ArrayList<int[]> combinations = getCombinations(lastLayer.size());
		
		System.out.println("Combine size: " + combinations.size());
		
		int noValid = 0;;
		
		for(int f = 0; f < combinations.size(); f++){
			
			int[] pair = combinations.get(f);
			Dimension next = new Dimension(lastLayer.get(pair[0]), lastLayer.get(pair[1]));
			
			if(!next.isValid()){
				
				noValid++;
			}
			
			if(next.isValid() && next.getCorrelation()){
				
				layer.add(next);
				lastLayer.get(pair[0]).incrCorrelationCount();
				lastLayer.get(pair[1]).incrCorrelationCount();
			}
			
		}
		
		for(int h = 0; h < lastLayer.size(); h++){
			
			if(lastLayer.get(h).isValid() && lastLayer.get(h).getCorrelationCount() == 0){
				
				indFeatures.add(lastLayer.get(h));
			}
		}
		System.out.println("No Valid: " + noValid);
		
		return layer;
		
	}
	
	public ArrayList<int[]> getCombinations(int size){
		
		ArrayList<int[]> combines = new ArrayList<int[]>(); 
		
		for(int f = 0; f < size; f++){
			for(int s = f; s < size; s++){
				
				if(f == s){
					
					continue;
				}
				
				combines.add(new int[]{f,s});
			}
		}
		
		return combines;
	}
	
	

	public class Dimension{
		
		private double m;
		private double b;
		private double r;
		private double meanY;
		private boolean correlate;
		private ArrayList<Integer> values;
		private int correlationCount;
		private ArrayList<Integer> usedDimensions;
		private boolean isValid;
		
		public Dimension(Dimension one, Dimension two){
			
			if(one.canCompare(two.getUsedDimensions())){
				
				isValid = false;
			
			}else{
			
				usedDimensions = new ArrayList<Integer>();
				
				usedDimensions.addAll(one.getUsedDimensions());
				usedDimensions.addAll(two.getUsedDimensions());
				
				Collections.sort(usedDimensions);
				getFeatureM(one, two);
				calR(one, two);
				
				this.correlate = this.r > 0.3;
				this.correlationCount = 0;
				
				createValues(one,two);
				
				isValid = true;
			}
		}
		
		public Dimension(ArrayList<Integer> values,int dimension){
			
			
			this.usedDimensions = new ArrayList<Integer>();
			this.usedDimensions.add(dimension);
			this.correlationCount = 0;
			this.values = values;
		}
		
		public ArrayList<Integer> getUsedDimensions(){
			
			return usedDimensions;
		}
		
		public boolean isValid(){
			
			return isValid;
		}
		
		public boolean getCorrelation(){
			
			return correlate;
		}
		
		public void incrCorrelationCount(){
			
			this.correlationCount++;
		}
		
		public int getCorrelationCount(){
			
			return this.correlationCount;
		}
		
		public boolean canCompare(ArrayList<Integer> dimensions){
			
			boolean containsOne = false;
			
			int index1 = 0;
			int index2 = 0;
			
			int exit = 0;
			
			while(exit < 2){
				
				if(dimensions.get(index1) < usedDimensions.get(index2)){
					
					if(index1 + 1 == dimensions.size()){
						
						exit ++;
					
					}else{
						
						index1++;
					}
				
				}else if(dimensions.get(index1) > usedDimensions.get(index2)){
					
					if(index2 + 1 == usedDimensions.size()){
						
						exit ++;
					
					}else{
						
						index2++;
					}
				}else{
					
					containsOne = true;
					break;
				}
			}
			
			return containsOne;
			
		}
		
		private void createValues(Dimension one, Dimension two){
			
			values = new ArrayList<Integer>();
			
			for(int s = 0; s < one.size(); s++){
				
				values.add((int) ((one.get(s) * m) + two.get(s)));
			}
		}
		
		private void calR(Dimension one, Dimension two){
			
			double var = 0.0;
			
			for(int t = 0; t < two.size(); t++){
				
				var += Math.pow(two.get(t) - meanY,2); 
			}
			
			double varLine = 0.0;
			
			for(int l = 0; l < one.size(); l++){
				
				varLine += Math.pow(two.get(l) - ((m * one.get(l)) + b), 2);
			}
			
			//System.out.println("Combine: " + varLine + " " + var);
			
			this.r = 1 - (varLine / var);
		}
		
		private int size() {
			// TODO Auto-generated method stub
			return values.size();
		}

		private void getFeatureM(Dimension one,Dimension two){
			
			double meanX = getMean(one,false);
			double meanY = getMean(two,false);
			
			double meanSqX = getMean(one, true);
			double meanXY = getTwoMean(one, two);
			
			m = ((meanX * meanY) - meanXY) / (Math.pow(meanX, 2) - meanSqX);
			
			b = meanY - (m * meanX);
			
			this.meanY = meanY;
			
		}
		
		private double getTwoMean(Dimension one,Dimension two){
			
			double avg = 0.0;
			
			for(int f = 0; f < one.size(); f++){
				
				avg += one.get(f) * two.get(f);
			}
			
			return avg / one.size();
		}
		
		public double get(int index){
			
			return values.get(index);
		}

		private double getMean(Dimension one,boolean square) {
			// TODO Auto-generated method stub
			double avg = 0.0;
			
			for(int f = 0; f < one.size(); f++){
				
				if(square){
					
					avg += Math.pow(one.get(f),2);
					
				}else{
					
					avg += one.get(f);
				}
				
			}
			return avg / one.size();
		}

		public double getDistance(int index, int classCenter) {
			// TODO Auto-generated method stub
			return Math.abs(values.get(index) - values.get(classCenter));
		}
	}
	
	public ArrayList<ArrayList<Integer>> classify(ArrayList<Integer> selection){
		
		System.out.println("Classify");
		
		ArrayList<ArrayList<Integer>> classes = new ArrayList<ArrayList<Integer>>();
		
		ArrayList<ArrayList<Double>> means = new ArrayList<ArrayList<Double>>();
		
		double difference = 1.5;
		
		for(int s = 0; s < selection.size(); s++){
			
			means.add(new ArrayList<Double>());
			for(int i = 0; i < indFeatures.size(); i++){
				
				means.get(s).add(indFeatures.get(i).get(selection.get(s)));
			}
			
		}
		
		for(int c = 0; c < selection.size(); c++){
			
			classes.add(new ArrayList<Integer>());
		}
		
		while(difference > 1.0){
			
			
			for(int c = 0; c < selection.size(); c++){
				
				classes.get(c).clear();
			}
			for(int h = 0; h < indFeatures.get(0).size(); h++){
				
				classes.get(classifyObject(h,means)).add(h);
			}
			
			difference = 0.0;
			for(int s = 0; s < means.size(); s++){
				
				for(int m = 0; m < means.get(s).size(); m++){
					double avg = 0.0;
					for(int i = 0; i < classes.get(s).size(); i++){
						
						avg += indFeatures.get(m).get(classes.get(s).get(i));
					}
					avg /= classes.get(s).size();
					difference += Math.abs(means.get(s).get(m) - avg);
					means.get(s).set(m, avg);
				}
			}
			
			System.out.println(difference);
		}
		
		return classes;
	}
	
	private int classifyObject(int index,ArrayList<ArrayList<Double>> means){
		
		double[] distances = new double[means.size()];
		
		for(int i = 0; i < indFeatures.size(); i++){
			for(int s = 0; s < means.size(); s++){
				
				distances[s] += Math.abs(indFeatures.get(i).get(index) - means.get(s).get(i));
			}
		}
		
		double minDist = Double.MAX_VALUE;
		int minIndex = 0;
		
		for(int d = 0; d < distances.length; d++){
			
			if(distances[d] < minDist){
				
				minDist = distances[d];
				minIndex = d;
			}
		}
		
		return minIndex;
	}
	
	/*public static boolean canCompare(ArrayList<Integer> dimensions,
			ArrayList<Integer> usedDimensions){
		
		boolean containsOne = false;
		
		int index1 = 0;
		int index2 = 0;
		
		int exit = 0;
		
		while(exit < 2){
			
			if(dimensions.get(index1) < usedDimensions.get(index2)){
				
				if(index1 + 1 == dimensions.size()){
					
					exit ++;
				
				}else{
					
					index1++;
				}
			
			}else if(dimensions.get(index1) > usedDimensions.get(index2)){
				
				if(index2 + 1 == usedDimensions.size()){
					
					exit ++;
				
				}else{
					
					index2++;
				}
			}else{
				
				containsOne = true;
				break;
			}
		}
		
		return containsOne;
		
	}*/
	
	/*public static void main(String[] args) {
		
		ArrayList<Integer> dimensions = new ArrayList<Integer>();
		dimensions.add(1);
		dimensions.add(2);
		dimensions.add(7);
		
		ArrayList<Integer> usedDimensions = new ArrayList<Integer>();
		usedDimensions.add(4);
		usedDimensions.add(5);
		usedDimensions.add(6);
		usedDimensions.add(7);
		
		boolean compare = canCompare(dimensions, usedDimensions);
		
		System.out.println("Compare: " + compare);
	}*/
	
	/*public static void main(String[] args) {
		
		KMeanRegression inst = new KMeanRegression();
		Random ran = new Random();
		ArrayList<Integer> oneVal = new ArrayList<Integer>();
		oneVal.add(ran.nextInt(10));
		oneVal.add(ran.nextInt(10));

		Dimension one = inst.new Dimension(oneVal);
		ArrayList<Integer> twoVal = new ArrayList<Integer>();
		twoVal.add(ran.nextInt(10));
		twoVal.add(ran.nextInt(10));
	
		Dimension two = inst.new Dimension(twoVal);
		
		Dimension combine = inst.new Dimension(one,two);
		
	}*/
	
	
}
