package Classification;

import java.awt.Color;
import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;

public class TreeFeature {

	public TreeFeature(BufferedImage map){
		
		int[][] mapValues = getMapValues(map);
		new TreeFeature(mapValues,map);
	}
	
	public TreeFeature(int[][] mapValues,BufferedImage map){
		
		//int[][] avgDiff = getAverageDifferenceArray(map);
		
		int[] dist = averageDifference(mapValues);
		
		System.out.println(dist[0] + " " + dist[1]);
		
		GroupList lastGroups = getGroupsInImage(mapValues, dist);
		
		System.out.println("Resolutions: " + lastGroups.resolution
				+ " Actual Resolution: " + map.getHeight());
		
		System.out.println(lastGroups.groups.size() + " groups " + lastGroups.groups.get(0).size()
				+ " arraySize: " + (mapValues.length * mapValues[0].length));
		
		ArrayList<GroupList> groupHeir = new ArrayList<GroupList>();
		groupHeir.add(lastGroups);
		
		int lastGroupSize;
		do{
			
			groupHeir.add(groupGroups(lastGroups));
			lastGroupSize = lastGroups.groups.size();
			lastGroups = groupHeir.get(groupHeir.size()-1);
			
			System.out.println("Number of Groups: " + lastGroups.groupInfo.size() 
							+ " Group Variance: " + lastGroups.groupVariance 
							+ " In Group Average Variance: " + lastGroups.averageInGroupVariance
							+ " Legitimacy: " + lastGroups.legitimacy);
			
		}while(groupHeir.get(groupHeir.size()-1).groups.size() != lastGroupSize);
		
		
		//ArrayList<int[]> pixels = getGroupPixels(130,61,groupHeir);
		ArrayList<ArrayList<int[]>> groupPixels = getPixelsOfGroupLevel(1,groupHeir);
		drawGroups(groupPixels,map);
		System.out.println("Pixels: " + groupPixels.size());
		//printGroupSize(groupHeir,mapValues);
		System.out.println("Group of Groups: " + groupHeir.size());
		
		//ArrayList<PixelGroups> allPixelGroups = assignAllPixel(groupHeir,mapValues.length,
			//	mapValues[0].length);
		
		/*for(int g = 0; g < groupHeir.get(0).groupsNos.size(); g++){
			System.out.print("Group: " + g + " ");
			for(int p = 0; p < groupHeir.get(0).groupsNos.get(g).size(); p++){
				System.out.print(groupHeir.get(0).groupsNos.get(g).get(p) + " ");
			}
			System.out.println();
		}*/
		//drawGroups(groupHeir,map,groupHeir.size()-2);
		
		//int maxColor = (int) (255 * Math.pow(255, 2) * Math.pow(255, 3));
		
		/*for(int g = 0; g < groups.size(); g++){
			
			int groupColor = getGroupColor(map,groups.get(g));
			
			for(int pt = 0; pt < groups.get(g).size(); pt++){
				
				map.setRGB(groups.get(g).get(pt)[1], groups.get(g).get(pt)[0], groupColor);
			}
		}*/
		
		//printGroups(groups);
	}
	
	private ArrayList<ArrayList<int[]>> getPixelsOfGroupLevel(int level, ArrayList<GroupList> groupsHeir){
		
		ArrayList<ArrayList<int[]>> groupPixels = new ArrayList<ArrayList<int[]>>();
		
		for(int g = 0; g < groupsHeir.get(level).groups.size(); g++){
			
			groupPixels.add(getGroupPixels(g,level,groupsHeir));
		}
		
		return groupPixels;
	}
	
	private ArrayList<int[]> getGroupPixels(int groupNo,int level,ArrayList<GroupList> groupsHeir){
		
		//get all pixels 
		if(level == 0){
		
			if(groupsHeir.get(0).groups.size() <= groupNo){
				
				System.err.println("Overflow: " + groupsHeir.get(0).groups.size() + " " + groupNo);
			}
			return groupsHeir.get(0).groups.get(0);
			
		}else{
			
			ArrayList<Integer> groups = groupsHeir.get(level).groupsNos.get(groupNo);
			ArrayList<int[]> pixels = new ArrayList<int[]>();
			
			for(int g = 0; g < groups.size(); g++){
				
				pixels.addAll(getGroupPixels(groups.get(g),level-1,groupsHeir));
			}
			
			return pixels;
		}
	}
	
	private ArrayList<PixelGroups> assignAllPixel(ArrayList<GroupList> groupHeir,int xSize,
			int ySize){
		
		ArrayList<PixelGroups> allPixels = new ArrayList<PixelGroups>();
		
		for(int x = 1; x < xSize-1; x++){
			for(int y = 0; y < ySize; y++){
				
				allPixels.add(assignPixel(new int[]{x,y},groupHeir));
			}
		}
		
		return allPixels;
		
	}
	
	private PixelGroups assignPixel(int[] pixel,ArrayList<GroupList> groupHeir){
		
		PixelGroups pixelGroups = new PixelGroups(pixel);
		int groupNo = -1;
		
		for(int g = 0; g < groupHeir.get(0).groups.size(); g++){
			for(int p = 0; p < groupHeir.get(0).groups.get(g).size(); p++){
				
				int[] check = groupHeir.get(0).groups.get(g).get(p);
				if(check[0] == pixel[0] && check[1] == pixel[1]){
					
					groupNo = g;
					break;
				}
			}
		}
		
		if(groupNo == -1){
			
			System.err.println("Error: Couldn't find pixel: {" + pixel[0] + "," + pixel[1] + "}");
		
		}
		
		/*for(int h = 1; h < groupHeir.size(); h++){
			for(int g = 0; g < groupHeir.get(h).groupsNos.size(); g++){
				for(int p = 0; p < groupHeir.get(h).groupsNos.get(g).size(); p++){
					
					if(groupHeir.get(h).groupsNos.get(g).get(p) == groupNo){
						
						groupNo = g;
						PixelGroup pg = new PixelGroup(h,groupNo);
						pixelGroups.groups.put(pg.hashCode(), pg);
					}
					
				}
			}
		}*/
		
		return pixelGroups;
		
	}

	
	private void drawGroups(ArrayList<GroupList> groupsHeir, BufferedImage map,int level){
		
		ArrayList<ArrayList<int[]>> pixelGroups = new ArrayList<ArrayList<int[]>>();
		
		int totalPixels = 0;
		for(int l = 0; l < groupsHeir.get(level).groupInfo.size(); l++){
			
			pixelGroups.add(getPixelsInGroup(groupsHeir,l,level-1));
			totalPixels += pixelGroups.get(l).size();
		}
		
		System.out.println("Resolution: " + Math.sqrt(totalPixels));
		
		for(int g = 0; g < pixelGroups.size(); g++){
			for(int p = 0; p < pixelGroups.get(g).size(); p++){
				
				map.setRGB(pixelGroups.get(g).get(p)[1], pixelGroups.get(g).get(p)[0], 
						Color.YELLOW.getRGB());
			}
		}
		
	}
	
	private void printGroupSize(ArrayList<GroupList> groupsHeir,int[][] mapValue){
		
		ArrayList<ArrayList<Integer>> sizes = new ArrayList<ArrayList<Integer>>();
		sizes.add(new ArrayList<Integer>());
		for(int x = 0; x < mapValue.length; x++){
			for(int y = 0; y < mapValue[x].length; y++){
				
				sizes.get(0).add(1);
			}
		}
		/*
		for(int g = 1; g < groupsHeir.size(); g++){
			
			sizes.add(new ArrayList<Integer>());
			int[][] groupLocs = groupsHeir.get(g-1).groupLocs;
			for(int i = 0; i < groupsHeir.get(g).groups.size(); i++){
				
				sizes.get(g).add(sizes.get(g-1).get(
						(groupsHeir.get(g).groups.get(
								groupsHeir.get(g).groupInfo.get(i).groupNo).get(i)[0] * groupLocs.length) + 
							groupsHeir.get(g).groups.get(
									groupsHeir.get(g).groupInfo.get(i).groupNo).get(i)[1]));
			}
			
		}*/
		
		
		int[][] groupLoc = mapValue;
		for(int g = 0; g < groupsHeir.size(); g++){
			System.out.println("Level: " + g + " " + sizes.get(g).size() + " " + 
						groupsHeir.get(g).groupInfo.size());
			sizes.add(new ArrayList<Integer>());
			for(int p = 0; p < groupsHeir.get(g).groups.size(); p++){
				int total = 0;
				for(int i = 0; i < groupsHeir.get(g).groups.get(p).size(); i++){
					
					int[] pos = groupsHeir.get(g).groups.get(p).get(i);
					int index = (pos[0] * groupsHeir.get(g).groupLocs[0].length) + pos[1];
					//System.out.println(pos[0] + " " + pos[1] + " " + (groupLoc.length-2) + " " 
						//	+ groupLoc[pos[1]].length + " " + sizes.get(g).size()
							//+ " " + (groupsHeir.get(g).groupLocs[0].length * groupsHeir.get(g).groupLocs[0].length) + " " + index);
					total += sizes.get(g).get(index);
				}
				
				sizes.get(g+1).add(total);
			}
			
			groupLoc = groupsHeir.get(g).groupLocs;
		}
		
		int[] totalSizes = new int[sizes.size()];
		
		for(int t = 0; t < sizes.size(); t++){
			for(int s = 0; s < sizes.get(t).size(); s++){
				
				totalSizes[t] += sizes.get(t).get(s);
			}
			
		}
		
		for(int t = 0; t < totalSizes.length; t++){
			
			System.out.println(totalSizes[t]);
		}
		
	}
	
	private void drawGroups(ArrayList<ArrayList<int[]>> groups,BufferedImage map){
		
		int total = 0;
		for(int g = 0; g < groups.size(); g++){
			total += groups.get(g).size();
			for(int p = 0; p < groups.get(g).size(); p++){
				
				int[] pos = groups.get(g).get(p);
				map.setRGB(pos[1], pos[0], 16711805);
			}
		}
		
		System.out.println("Group Pixel total: " + total 
				+ " Actual value: " + (map.getHeight() * map.getWidth()));
		
	}
	
	private ArrayList<int[]> getPixelsInGroup(ArrayList<GroupList> groupsHeir,int selection,int level){
		
		ArrayList<int[]> pixels = new ArrayList<int[]>();
		
		//base case first group
		if(level == 0){
			
			//return getOnlyEdges(groupsHeir.get(0).groups.get(selection),groupsHeir.get(0).groupLocs);
			return groupsHeir.get(0).groups.get(selection);
			
		//recursive group of groups
		}else{
			
			ArrayList<int[]> nextGroups = groupsHeir.get(level).groups.get(selection);
			
			for(int g = 0; g < nextGroups.size(); g++){
				
				int nextSelection = groupsHeir.get(level).groupInfo.get(g).groupNo;
				pixels.addAll(getPixelsInGroup(groupsHeir,nextSelection,level-1));
			}
			
		}
		
		return pixels;
		
		
	}
	
	private ArrayList<int[]> getOnlyEdges(ArrayList<int[]> pixels,int[][] groupLocs){
		
		int groupNo = groupLocs[pixels.get(0)[0]][pixels.get(0)[1]];
		for(int r = 0; r < pixels.size(); r++){
			
			int[] loc = pixels.get(r);
			boolean isEdge = isOtherGroup(loc,1, 0, groupLocs, groupNo)
							|| isOtherGroup(loc,-1, 0, groupLocs, groupNo)
							|| isOtherGroup(loc,0, 1, groupLocs, groupNo)
							|| isOtherGroup(loc,0, -1, groupLocs, groupNo)
							|| isOtherGroup(loc,1, 1, groupLocs, groupNo)
							|| isOtherGroup(loc,1, -1, groupLocs, groupNo)
							|| isOtherGroup(loc,-1, -1, groupLocs, groupNo)
							|| isOtherGroup(loc,-1, 1, groupLocs, groupNo);
			
			if(!isEdge){
				
				pixels.remove(r);
				r--;
			}
							
		}
		
		
		return pixels;
		
	}
	
	private boolean isOtherGroup(int[] loc, int dx,int dy, int[][] groupLocs,int groupNo){
		
		return loc[0] + dx >= 0 && loc[0] + dx < groupLocs.length && loc[1] + dy >= 0 &&
				loc[1] + dy < groupLocs[0].length && groupLocs[loc[0] + dx][loc[1] + dy] != groupNo;
	}
	
	//private ArrayList<int[]> getPixelsInGroup()
	
	/*private ArrayList<ArrayList<Integer>> groupGroups(GroupList lastGroups){
		
		boolean[] assigned = new boolean[lastGroups.groups.size()];
		
		
		
		ArrayList<ArrayList<Integer>> groupsOfGroups = new ArrayList<ArrayList<Integer>>(); 
		for(int g = 0; g < lastGroups.groups.size(); g++){
			
			groupsOfGroups.add(groupNeighbours(lastGroups.groupLocs, lastGroups.groups.get(g),
					g+1, assigned, lastGroups));
		}
		
		return groupsOfGroups;
	}
	
	private ArrayList<Integer> groupNeighbours(int[][] groupLocs,ArrayList<int[]> group,
			int groupNo,boolean[] assigned,GroupList groups){
		
		ArrayList<Integer> neighbours = getGroupNeighbours(groupLocs,
				group, groupNo);
		ArrayList<Integer> inGroup = new ArrayList<Integer>();
		assigned[groupNo-1] = true;
		
		for(int a = 0; a < assigned.length; a++){

			if(!assigned[a]){
				System.out.println(assigned[a] + " " + a);
			}
		}
		
		for(int t = 0; t < neighbours.size(); t++){
			
			if(!assigned[neighbours.get(t)+1]){
				
				assigned[neighbours.get(t)+1] = true;
				inGroup.add(neighbours.get(t));
				inGroup.addAll(groupNeighbours(groupLocs,groups.groups.get(neighbours.get(t)),
						neighbours.get(t),assigned,groups));
			}
		}
		
		return inGroup;
	}*/
	
	private GroupList groupGroups(GroupList lastGroups){
		
		int[][] groupGroups = createGroupArray(lastGroups.groupLocs,lastGroups.groupInfo);
		
		/*for(int x = 0; x < groupGroups.length; x++){
			for(int y = 0; y < groupGroups[x].length; y++){
				
				System.out.print(groupGroups[x][y] + " ");
			}
			System.out.println();
		}*/
		int[] dist = averageDifference(groupGroups);
		return getGroupsInImage(groupGroups, dist);
	}

	private int[][] createGroupArray(int[][] groupLocs,ArrayList<GroupInfo> group){
		
		HashMap<Integer,int[]> groupPos = new HashMap<Integer,int[]>();
		
		for(int x = 0; x < groupLocs.length; x++){
			for(int y = 0; y < groupLocs[x].length; y++){
				
				if(groupLocs[x][y] == 0){
					
					continue;
				}
				
				if(groupPos.containsKey(groupLocs[x][y])){
					
					groupPos.get(groupLocs[x][y])[0] = groupPos.get(groupLocs[x][y])[0] 
							+ (x - groupPos.get(groupLocs[x][y])[0])/2;
					groupPos.get(groupLocs[x][y])[1] = groupPos.get(groupLocs[x][y])[1] 
							+ (y - groupPos.get(groupLocs[x][y])[1])/2;
				
				}else{
					
					groupPos.put(groupLocs[x][y], new int[]{x,y});
				}
			}
		}
		
		ArrayList<Integer> groups = new ArrayList<Integer>(groupPos.keySet());
		Collections.sort(groups,new Comparator<Integer>(){

			@Override
			public int compare(Integer oneKey, Integer twoKey) {
				// TODO Auto-generated method stub
				int[] one = groupPos.get(oneKey);
				int[] two = groupPos.get(twoKey);
				
				if(one[0] > two[0]){
					
					return -1;
				
				}else if(one[0] == two[0] && one[1] > two[1]){
					
					return -1;
				}
				
				return 0;
			}
			
		});
		
		int size = (int) Math.sqrt(group.size());
		int offset = 1;
		
		while((size * size) < groups.size()){
			
			size = (int) Math.sqrt(group.size() + offset);
			offset+=2;
		}
		int[][] groupArray = new int[size][size];
		
		
		
		for(int g = 0; g < groups.size(); g++){
			
			//System.out.println(groups.get(g) + " GroupNo");
			groupArray[g/size][g%size] = group.get(groups.get(g)-1).average;
		}
		
		for(int r = groups.size(); r < (size*size); r++){
			
			groupArray[r/size][r%size] = -1;
		}
		
		return groupArray;
	}
	
	/*private int[][] createGroupArray(int[][] groupLocs,ArrayList<GroupInfo> group){
		
		HashMap<Integer,int[]> groupPos = new HashMap<Integer,int[]>();
		
		for(int x = 0; x < groupLocs.length; x++){
			for(int y = 0; y < groupLocs[x].length; y++){
				
				if(groupPos.containsKey(groupLocs[x][y])){
					
					groupPos.get(groupLocs[x][y])[0] = groupPos.get(groupLocs[x][y])[0] 
							+ (x - groupPos.get(groupLocs[x][y])[0])/2;
					groupPos.get(groupLocs[x][y])[1] = groupPos.get(groupLocs[x][y])[1] 
							+ (y - groupPos.get(groupLocs[x][y])[1])/2;
				
				}else{
					
					groupPos.put(groupLocs[x][y], new int[]{x,y});
				}
			}
		}
		
		double sizeRatio = groupPos.size()/(double)(groupLocs.length * groupLocs[0].length);
		
		int xSize = (int) (groupLocs.length * Math.pow(sizeRatio,0.5))+1;
		int[][] groupArray = new int[xSize][xSize];
		
		System.out.println("Group Array: " + group.size() + " " + (groupArray.length * groupArray[0].length));
		
		for(int g = 0; g < groupPos.size(); g++){
			
			groupArray[g/xSize][g%xSize] = group.get(g).average;
		}	
	
		return groupArray;
	}*/
	 
	
	private ArrayList<Integer> getGroupNeighbours(int[][] groupLocs,ArrayList<int[]> group,
			int groupNo){
		
		HashMap<Integer,Boolean> nearGroups = new HashMap<Integer,Boolean>();
		
		int[] lc = new int[]{0,1,0,-1,1,0,-1,0,1,1,-1,-1,1,-1,-1,1};
		for(int g = 0; g < group.size(); g++){
			
			for(int l = 0; l < lc.length; l+=2){
				int x = group.get(g)[0] + lc[l];
				int y = group.get(g)[1] + lc[l+1];
				
				if(x >= 1 && x < groupLocs.length-1
						&& y >= 0 && y < groupLocs[0].length
							&& groupLocs[x][y] != groupNo  && !nearGroups.containsKey(groupLocs[x][y])){
					
					nearGroups.put(groupLocs[x][y], true);
				}
			}
		}
		
		return new ArrayList<Integer>(nearGroups.keySet());
	}

	private int[][] getMapValues(BufferedImage map) {
		// TODO Auto-generated method stub
		int[][] mapValues = new int[map.getHeight()][map.getWidth()];
		
		for(int x = 0; x < map.getHeight(); x++){
			for(int y = 0; y < map.getWidth(); y++){
				
				mapValues[x][y] = map.getRGB(y, x);
			}
		}
		
		return mapValues;
	}
	
	private int getVariance(int average,int[][] map, ArrayList<int[]> group){
		
		long stddev = 0;
		
		for(int g = 0; g < group.size(); g++){
			
			stddev += Math.pow(map[group.get(g)[0]][group.get(g)[1]] - average,2);
		}
		
		return (int)(Math.pow(stddev/group.size(),0.5));
	}

	private int getGroupColor(int[][] map,ArrayList<int[]> group){
		
		long avgColor = 0;
		
		for(int g = 0; g < group.size(); g++){
			
			avgColor += map[group.get(g)[0]][group.get(g)[1]];
		}
		
		return (int)(avgColor/group.size());
	}
	
	private void printGroups(ArrayList<ArrayList<int[]>> groups){
		
		for(int g = 0; g < groups.size(); g++){
			System.out.println("Group " + g + ":");
			for(int p = 0; p < groups.get(g).size(); p++){
				
				System.out.println(groups.get(g).get(p)[0] + " " + groups.get(g).get(p)[1]);
			}
			
		}
	}
	
	private int[] averageDifference(int[][] avgDiff){
		
		long avg = 0;
		long stddev = 0;
		
		for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				avg += avgDiff[x][y];
			}
		}
		
		avg = (avg / (avgDiff.length * avgDiff[0].length));
		
		for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				stddev += Math.pow(avgDiff[x][y] - avg, 2);
			}
		}
		
		stddev = (long) Math.sqrt(stddev/(avgDiff.length * avgDiff[0].length));
		
		System.out.println(avg + " " + stddev);
		
		return new int[]{(int)avg ,(int) stddev};
	}
	
	private GroupList getGroupsInImage(int[][] avgDiff,int[] stat){
		
		int[][] assigned = new int[avgDiff.length][avgDiff[0].length];
		
		ArrayList<ArrayList<int[]>> groups = new ArrayList<ArrayList<int[]>>();
		ArrayList<GroupInfo> groupColor = new ArrayList<GroupInfo>();
		
		for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				if(assigned[x][y] == 0 && avgDiff[x][y] != -1){
					
					groups.add(getGroup(assigned,avgDiff,stat,x,y,groups.size()+1));
					int avg = getGroupColor(avgDiff,groups.get(groups.size()-1));
					groupColor.add(new GroupInfo(groups.size()-1,avg,getVariance(
							avg,avgDiff,groups.get(groups.size()-1))));
				}
			}
		}
		
		/*for(int x = 0; x < assigned.length; x++){
			for(int y = 0; y < assigned[x].length; y++){
				
				System.out.print(assigned[x][y] + " ");
			}
			System.out.println();
		}*/
		
		
		return new GroupList(groups,assigned,groupColor);
	}
	
	private ArrayList<int[]> getGroup(int[][] assigned, int[][] avgDiff, int[] stat,
			int x, int y,int groupNo){
		
		ArrayList<int[]> group = new ArrayList<int[]>();
		
		int[] choices = new int[]{0,1,0,-1,1,0,-1,0};
		
		int c = 0;
		int lx = x;
		int ly = y;
		int bx = lx;
		int by = ly;
		
		while(c < 4){
			
			if(!(lx < 0 || lx >= assigned.length || ly < 0 || ly >= assigned[0].length) 
					&& assigned[lx][ly] == 0 && avgDiff[lx][ly] != -1
						&& Math.abs(avgDiff[x][y] - avgDiff[lx][ly]) < (stat[1]/5)){

				group.add(new int[]{lx,ly,(lx * assigned[0].length) + ly});
				assigned[lx][ly] = groupNo;
				bx = lx;
				by = ly;
				c = 0;
				
			}else{
				
				lx = bx + choices[c*2];
				ly = by + choices[(c*2)+1];
				
				c++;
			}
		}
		
		return group;
	}
	
	private int[][] getAverageDifferenceArray(BufferedImage map){
		
		int[][] avgDist = new int[map.getHeight()][map.getWidth()];
		
		for(int x = 1; x < map.getHeight()-1; x++){
			for(int y = 1; y < map.getWidth()-1; y++){
				
				avgDist[x][y] = Math.abs(map.getRGB(x-1, y) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x, y-1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x-1, y-1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x-1, y+1) - map.getRGB(x, y));
				avgDist[x][y] += Math.abs(map.getRGB(x+1, y-1) - map.getRGB(x, y));
				
				avgDist[x][y] /= 8;
			}
		}
		
		return avgDist;
		
	}
	
	public class GroupList{
		
		
		public ArrayList<ArrayList<int[]>> groups;
		public ArrayList<ArrayList<Integer>> groupsNos;
		public ArrayList<GroupInfo> groupInfo;
		public int[][] groupLocs;
		public int groupVariance;
		public int averageInGroupVariance;
		public int legitimacy;
		public int resolution;//debug
		
		public GroupList(ArrayList<ArrayList<int[]>> groups, int[][] groupLocs,
				ArrayList<GroupInfo> groupInfo) {
			// TODO Auto-generated constructor stub
			//System.out.println("Block size: " + groupLocs.length);
			this.groups = groups;
			this.groupLocs = groupLocs;
			this.groupInfo = groupInfo;
			createGroupNos();
			calGroupVar();
			calAvgInGroupVar();
			
			if(averageInGroupVariance != 0){
				legitimacy = groupVariance / averageInGroupVariance;
			}
			
			for(int a = 0; a < groups.size(); a++){
				
				resolution += groups.get(a).size();
			}
			
			resolution = (int) Math.sqrt(resolution);
			
		}
		
		private void createGroupNos(){
			
			groupsNos = new ArrayList<ArrayList<Integer>>();
			for(int g = 0; g < groups.size(); g++){
				groupsNos.add(new ArrayList<Integer>());
				//System.out.print("GroupNo: ");
				for(int p = 0; p < groups.get(g).size(); p++){
					
					int[] pos = groups.get(g).get(p);
					/*int groupNo = (pos[0] * groupLocs[0].length) + pos[1];
					if(groupNo >= (groupLocs[0].length * groupLocs.length)){
						
						System.err.println("Error greater than size: " + groupNo + " " + groupLocs[0].length
								+ " " + groupLocs.length);
					}*/
					
					if(pos[2] == -1){
						
						System.err.println("-1 getting though");
					}
					groupsNos.get(g).add(pos[2]);
					//System.out.print("{" + pos[0] +"," + pos[1] +"," +groupsNos.get(g).get(p) +"}");
				}
				//System.out.println();
			}
		}
		
		private void calGroupVar(){
			
			long avg = 0;
			
			for(int g = 0; g < groupInfo.size(); g++){
				
				avg += groupInfo.get(g).average;
			}
			
			avg /= groupInfo.size();
			
			long stddev = 0;
			
			for(int s = 0; s < groupInfo.size(); s++){
				
				stddev += Math.pow(groupInfo.get(s).average - avg, 2);
			}
			
			this.groupVariance = (int)Math.sqrt(stddev/groupInfo.size());
		}
		
		private void calAvgInGroupVar(){
			
			long avg = 0;
			
			for(int g = 0; g < groupInfo.size(); g++){
				
				avg += groupInfo.get(g).variance;
			}
			
			this.averageInGroupVariance = (int)(avg/groupInfo.size());
		}
		
		
		
		/*public int[][] getSinglarGroupLocs(){
			
			int maxY = -1;
			int minY = Integer.MAX_VALUE;
			int maxX = -1;
			int minX = Integer.MAX_VALUE;
			
			for(int g = 0; g < groups.size(); g++){
				
				maxX = Math.max(groups.get(g).get(0)[0],maxX);
				minX = Math.min(groups.get(g).get(0)[0],minX);
				maxY = Math.max(groups.get(g).get(0)[1],maxY);
				minY = Math.min(groups.get(g).get(0)[1],minY);
			}
		}*/
		
		
	}
	
	public class PixelGroups{
		
		public int[] pixel;
		public HashMap<Integer,PixelGroup> groups;
		
		public PixelGroups(int[] pixel){
			
			this.pixel = pixel;
			groups = new HashMap<Integer,PixelGroup>();
		}

		public void printGroups() {
			// TODO Auto-generated method stub
			
			for(PixelGroup pg : groups.values()){
				
				pg.printPixelGroup();
			}
		}
	}
	
	public class PixelGroup{
		
		public int level;
		public int groupNo;
		
		public PixelGroup(int level,int groupNo){
			
			this.level = level;
			this.groupNo = groupNo;
		}

		public void printPixelGroup() {
			// TODO Auto-generated method stub
			System.out.println("{" + level + "," + groupNo + "}");
		}
		
		@Override
		public int hashCode() {
			// TODO Auto-generated method stub
			return (int)((0.5 * (level + groupNo) * (level + groupNo + 1)) + groupNo);
		}
	}
	
	
	public class GroupInfo{
		
		public int average;
		public int variance;
		public int groupNo;
		
		public GroupInfo(int groupNo,int average, int variance){
			
			this.average = average;
			this.variance = variance;
			this.groupNo = groupNo;
		}
	}
	
}
