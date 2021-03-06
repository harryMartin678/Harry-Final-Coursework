package Classification;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;

public class FlattenedFeature {

	private ArrayList<Integer> features;
	
	public FlattenedFeature(FixedTreeFeature tree){
		
		features = new ArrayList<Integer>();
		
		features.addAll(getLevelsFeatures(tree.getColorLevels(),0));
		features.addAll(getLevelsFeatures(tree.getColorLevels(),1));
		features.addAll(getLevelsFeatures(tree.getColorLevels(),2));
		features.addAll(getLevelsFeatures(tree.getCannyLevels(),0));
		features.addAll(getLevelsFeatures(tree.getDevLevels(),0));
		
		System.out.println("No of features: " + features.size());
	}
	
	public ArrayList<Integer> getLevelsFeatures(ArrayList<Block[][]> levels,int index){
		
		ArrayList<Integer> features = new ArrayList<Integer>();
		
		for(int l = 1; l < levels.size(); l++){
			
			features.addAll(getFlattenFeatures(levels.get(l),index));
		}
		
		return features;
	}
	
	public ArrayList<Integer> getFlattenFeatures(Block[][] blocks,int index){
		
		HashMap<Integer,Integer> featuresMap = new HashMap<Integer,Integer>();
		
		for(int x = 0; x < blocks.length; x++){
			for(int y = 0; y < blocks[x].length; y++){
					
				if(blocks[x][y] == null){
					
					System.out.println("Block is null");
					continue;
				}
				//features.add(blocks[x][y].getWord());
				if(!featuresMap.containsKey(blocks[x][y].getWord())){
					
					featuresMap.put(blocks[x][y].getWord(), 1);
					
				}else{
					
					featuresMap.replace(blocks[x][y].getWord(), 
							featuresMap.get(blocks[x][y].getWord())+1);
				}
			}
		}
		
		ArrayList<Integer> features = new ArrayList<Integer>();
		
		for(Integer no : featuresMap.values()){
			
			features.add(no);
		}
		
		//Collections.sort(features);
		
		return features;
	}
}
