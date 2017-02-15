package Classification;

import java.awt.image.BufferedImage;
import java.util.ArrayList;
import java.util.HashMap;

import Classification.TreeFeature.GroupInfo;
import Classification.TreeFeature.GroupList;
import Classification.TreeFeature.PixelGroup;

public class HeirarchicalFeature {

	
	public HeirarchicalFeature(BufferedImage map){
		
		int[][] mapValues = getMapValues(map);
		getGroupHeir(mapValues);
		
		ArrayList<GroupList> groups = getGroupHeir(mapValues);
		
		int maxLeg = Integer.MAX_VALUE;
		int maxLegIndex = 0;
		
		for(int g = 0; g < groups.size(); g++){
			
			System.out.println("HeirarchicalFeature: " + groups.get(g).groups.size() + " "
					+ groups.get(g).legitimacy);
			
			if(groups.get(g).legitimacy < maxLeg){
				
				maxLeg = groups.get(g).legitimacy;
				maxLegIndex = g;
			}
		}
		
		drawGroup(groups.get(groups.size()-2),map);
		
	}
	
	private void drawGroup(GroupList groups,BufferedImage map){
		
		for(int g = 0; g < groups.groups.size(); g++){
			for(int l = 0; l < groups.groups.get(g).size(); l++){
				int[] loc = groups.groups.get(g).get(l);
				if(isCorner(loc, groups.groupLocs)){
					
					map.setRGB(loc[1], loc[0], 16711805);
				}
			}
		}
		
	}
	
	private boolean isCorner(int[] pixel, int[][] groupLocs){
		
		boolean isCorner = false;
		
		int[] lc = new int[]{0,1,0,-1,1,0,-1,0,1,1,-1,-1,-1,1,1,-1};
		
		for(int l = 0; l < lc.length; l+=2){
			
			if(pixel[0] - lc[l] >= 0 && pixel[0] - lc[l] < groupLocs.length
					&& pixel[1] - lc[l+1] >= 0 && pixel[1] - lc[l+1] < groupLocs[0].length){
				isCorner = isCorner || 
						groupLocs[pixel[0] - lc[l]][pixel[1] - lc[l+1]] != groupLocs[pixel[0]][pixel[1]];
			}
			
			if(isCorner){
				
				break;
			}
					
		}
		
		return isCorner;
	}
	
	private ArrayList<GroupList> getGroupHeir(int[][] mapValues){

		ArrayList<GroupList> groups = new ArrayList<GroupList>();
		
		int[] stat = averageDifference(mapValues);
		
		double level = 10;
		GroupList thisGroup = getGroupsInImage(mapValues, stat,level);
		groups.add(thisGroup);
		GroupList lastGroup;
		
		do{
			
			level /= 2;
			
			if(level < 0.5){
				
				level = 0.5;
			}
			
			lastGroup = thisGroup;
			stat = averageDifference(lastGroup.groupLocs);
			thisGroup = getGroupsInImage(lastGroup.groupLocs, stat,level);
			groups.add(thisGroup);
			
	
		}while(lastGroup.groups.size() != thisGroup.groups.size() && !(thisGroup.groups.size() < 20));
		
		
		
		return groups;
		
	}
	
	private GroupList getGroupsInImage(int[][] avgDiff,int[] stat,double level){
		
		int[][] assigned = new int[avgDiff.length][avgDiff[0].length];
		
		ArrayList<ArrayList<int[]>> groups = new ArrayList<ArrayList<int[]>>();
		ArrayList<GroupInfo> groupColor = new ArrayList<GroupInfo>();
		
		for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				if(assigned[x][y] == 0 && avgDiff[x][y] != -1){
					
					groups.add(getGroup(assigned,avgDiff,stat,x,y,groups.size()+1,level));
					if(groups.get(groups.size()-1).size() == 0){
						
						continue;
					}
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
	
	private int[] averageDifference(int[][] avgDiff){
		
		long avg = 0;
		long stddev = 0;
		
		HashMap<Integer,Boolean> hasBeenSeen = new HashMap<Integer,Boolean>();
		
		for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				if(!hasBeenSeen.containsKey(avgDiff[x][y])){
					
					hasBeenSeen.put(avgDiff[x][y], true);
					avg += avgDiff[x][y];
				}
			}
		}
		
		//avg = (avg / (avgDiff.length * avgDiff[0].length));
		avg /= hasBeenSeen.size();
		
		/*for(int x = 0; x < avgDiff.length; x++){
			for(int y = 0; y < avgDiff[x].length; y++){
				
				stddev += Math.pow(avgDiff[x][y] - avg, 2);
			}
		}
		
		stddev = (long) Math.sqrt(stddev/(avgDiff.length * avgDiff[0].length));*/
		
		for(Integer value : hasBeenSeen.keySet()){
			
			stddev += Math.pow(value - avg, 2);
		}
		
		stddev = (long) Math.sqrt(stddev/hasBeenSeen.size());
		
		return new int[]{(int)avg ,(int) stddev};
	}
	
	private int getGroupColor(int[][] map,ArrayList<int[]> group){
		
		long avgColor = 0;
		
		for(int g = 0; g < group.size(); g++){
			
			avgColor += map[group.get(g)[0]][group.get(g)[1]];
		}
		
		return (int)(avgColor/group.size());
	}
	
	private ArrayList<int[]> getGroup(int[][] assigned, int[][] avgDiff, int[] stat,
			int x, int y,int groupNo,double level){
		
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
						&& Math.abs(avgDiff[x][y] - avgDiff[lx][ly]) < (stat[1]/level)){

				group.add(new int[]{lx,ly,(lx * assigned[0].length) + ly});
				assigned[lx][ly] = groupNo;
				//runningAvg += runningAvg + (avgDiff[lx][ly] - runningAvg)/group.size();
				bx = lx;
				by = ly;
				c = 0;
				
			}else{
				
				lx = bx + choices[c*2];
				ly = by + choices[(c*2)+1];
				
				c++;
			}
		}
		
		if(group.size() == 0){
			
			return group;
		}
		
		long average = 0;
		for(int g = 0; g < group.size(); g++){
			
			int[] pos = group.get(g);
			average += avgDiff[pos[0]][pos[1]];
		}
		
		average /= group.size();
		
		for(int g = 0; g < group.size(); g++){
			
			int[] pos = group.get(g);
			assigned[pos[0]][pos[1]] = (int)average;
		}
		
		return group;
	}
	
	public static int[][] getMapValues(BufferedImage map) {
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
			if(this.groupInfo.size() > 0){
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
}
