package rover;

import java.util.ArrayList;
import java.util.HashMap;


public class ScanningRover extends Rover {

	private int SCAN_RANGE = 6;
	private int MAX_SPEED = 2;
	private ArrayList<ExplorationSite> exploreSpots;
	private HashMap<Integer,FoundItem> resources;
	private int nextExploreSpot = 0;
	private RoverOntology onto;
	private FoundItem rover;
	
	public ScanningRover(){
		
		super();
		setTeam("hm474");
		
		onto = new RoverOntology();
		
		try {
			//set attributes for this rover
			//speed, scan range, max load
			//has to add up to <= 9
			//Fourth attribute is the collector type
			setAttributes(MAX_SPEED, SCAN_RANGE, 1, 1);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	@Override
	void begin() {
		// TODO Auto-generated method stub
		System.out.println("START SCANNING ROVER!!!");
		exploreSpots = new ArrayList<ExplorationSite>();
		resources = new HashMap<Integer,FoundItem>();
		fillExploreSpots();
		nextExploreSpot = 0;
		rover = new FoundItem(0,0,"Rover",getWorldWidth(),getWorldHeight());
		
		try {
			scan(SCAN_RANGE);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Override
	void end() {
		// TODO Auto-generated method stub
		System.out.println("END SCANNING ROVER!!!");
		cleanUp();
	}

	@Override
	void poll(PollResult pr) {
		// TODO Auto-generated method stub
		switch(pr.getResultType()){
		
			case PollResult.MOVE:
				try {
					scan(SCAN_RANGE);
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				break;
				
			case PollResult.SCAN:
				ScanItem[] items = pr.getScanItems();
				for(int s = 0; s < items.length; s++){
					
					if(items[s].getResourceType() == ScanItem.RESOURCE){
						
						double x = items[s].getxOffset() + rover.getX();
						double y = items[s].getyOffset() + rover.getY();
						int key = FoundItem.getKey(
								FoundItem.getAdjustedMove(x,y,getWorldWidth(),getWorldHeight()));
						
						if(!resources.containsKey(key)){
							resources.put(key,  new FoundItem(x,y,"Resource",
									getWorldWidth(),getWorldHeight()));
							reportResource(resources.get(key));
						}
					}
					
				}
				
				System.out.println("Found " + resources.size() + " resources");
				nextMove();
				break;
		}
		
	}
	
	private void reportResource(FoundItem foundItem) {
		// TODO Auto-generated method stub
		String msg = onto.createCommand(0, new Double(foundItem.getX()).toString(),
				new Double(foundItem.getY()).toString());
		broadCastToTeam(msg);
	}
	
	
	private void nextMove(){
		
		try {
			double[] newEx = exploreSpots.get(nextExploreSpot).toArray();
			exploreSpots.get(nextExploreSpot).setExplored();
			nextExploreSpot();
			move(newEx[0] - rover.getX(),newEx[1] - rover.getY(), MAX_SPEED);
			rover.move(newEx[0] - rover.getX(),newEx[1] - rover.getY());
			//String msg = onto.createCommand(RoverOntology.EXPLORESPOT, new Double(newEx[0]).toString(),
					//new Double(newEx[1]).toString());
			//System.out.println(msg + "|Enter");
			//broadCastToTeam(msg);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	private void fillExploreSpots(){
		
		//System.out.println("Start Filling Explore Spots");
		for(int c = 1; c < this.getWorldHeight()/SCAN_RANGE; c++){
			double circum = 2 * Math.PI * c * SCAN_RANGE;
			double angle = (2*Math.PI)/(circum / SCAN_RANGE);
			//System.out.println("circum " + circum);
			//System.out.println("START CIRCLE");
			for(int s = 0; s <= circum / SCAN_RANGE; s++){
				
				//System.out.println((c*4) * Math.cos(angle * s) + " " +
				//		(c*4) * Math.sin(angle * s));
				exploreSpots.add(new ExplorationSite((c*SCAN_RANGE) * Math.cos(angle * s),
						(c*SCAN_RANGE) * Math.sin(angle * s)));
			}
			//System.out.println("END CIRCLE");
		}
		
		//System.out.println("End Filling Explore Spots");
	}
	
	private void nextExploreSpot() {
		// TODO Auto-generated method stub
		for(int e = nextExploreSpot; e < exploreSpots.size(); e++){
			
			if(!exploreSpots.get(e).isExplored()){
				
				break;
			
			}else{
				
				nextExploreSpot++;
			}
		}
	}
	
	

}
