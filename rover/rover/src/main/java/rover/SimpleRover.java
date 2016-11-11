package rover;

import java.util.ArrayList;
import java.util.Random;

import rover.Ontology.Argument;
import rover.Ontology.MethodCall;

//java -jar lib/boot.jar ./RoverWorld.xml
//java -jar lib/config.jar
public class SimpleRover extends Rover {

	//2 x speed / maxspeed = e per second
	//2 x scanradius / maxscanradius = e per scan

	private ArrayList<FoundItem> items;
	private FoundItem rover;
	private boolean collectNextTurn;
	private boolean depositNextTurn;
	private final int SCAN_RANGE = 4;
	private ArrayList<ExplorationSite> exploreSpots;
	private int nextExploreSpot = 0;
	//private RoverOntology onto;
	
	public SimpleRover(){
		
		super();
		setTeam("hm474");
		
		try {
			//set attributes for this rover
			//speed, scan range, max load
			//has to add up to <= 9
			//Fourth attribute is the collector type
			setAttributes(4, SCAN_RANGE, 1, 1);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	@Override
	void begin() {
		// TODO Auto-generated method stub
		
		items = new ArrayList<FoundItem>();
		rover = new FoundItem(0, 0, "rover",getWorldWidth(),getWorldHeight());
		depositNextTurn = false;
		collectNextTurn = false;
		nextExploreSpot = 0;
		getLog().info("Begin SimpleRover...");
		exploreSpots = new ArrayList<ExplorationSite>();
		fillExploreSpots();
		//onto = new RoverOntology();
		
		try {
			scan(4);
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
//		new Thread(new Runnable(){
//
//			@Override
//			public void run() {
//				// TODO Auto-generated method stub
//				while(true){
//					
//					retrieveMessages();
//					
//					for(String msg: messages){
//						
//						parseMessage(msg);
//					}
//					
//					try {
//						Thread.sleep(250);
//					} catch (InterruptedException e) {
//						// TODO Auto-generated catch block
//						e.printStackTrace();
//					}
//				}
//			}
//			
//			
//		}).start();
		
		
	}
	
//	private void parseMessage(String msg) {
//		// TODO Auto-generated method stub
//		MethodCall mc = onto.analysisCommand(msg);
//		
//		if(mc.getMethodName().equals("notifyOfExploreSpot")){
//			
//			notifyOfExploreSpot(mc.getArgs());
//		}
//	}
	
	private void fillExploreSpots(){
		
		//System.out.println("Start Filling Explore Spots");
		for(int c = 1; c < this.getWorldHeight()/4; c++){
			double circum = 2 * Math.PI * c * 4;
			double angle = (2*Math.PI)/(circum / 4);
			//System.out.println("circum " + circum);
			//System.out.println("START CIRCLE");
			for(int s = 0; s <= circum / 4; s++){
				
				//System.out.println((c*4) * Math.cos(angle * s) + " " +
				//		(c*4) * Math.sin(angle * s));
				exploreSpots.add(new ExplorationSite((c*4) * Math.cos(angle * s),
						(c*4) * Math.sin(angle * s)));
			}
			//System.out.println("END CIRCLE");
		}
		
		//System.out.println("End Filling Explore Spots");
	}

	@Override
	void end() {
		// TODO Auto-generated method stub
		getLog().info("End SimpleRover...");
		cleanUp();
	}

	@Override
	void poll(PollResult pr) {
		// TODO Auto-generated method stub
		switch(pr.getResultType()){
		
		case PollResult.DEPOSIT:
			//System.out.println("FINISHED DEPOSIT!!!");
			nextMove();
			break;
		
		case PollResult.COLLECT:
			
			//System.out.println("FINISHED COLLECT!!!");
			try {
				double[] pos = FoundItem.getAdjustedMove(-rover.getX(),-rover.getY(),getWorldWidth(),getWorldHeight());
				move(pos[0],pos[1],4);
				rover.move(-rover.getX(),-rover.getY());
				depositNextTurn = true;
			} catch (Exception e2) {
				// TODO Auto-generated catch block
				e2.printStackTrace();
			}
			
			
			break;
		
		case PollResult.MOVE:
			
			if(collectNextTurn){
				
				try {
					collect();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					//System.out.println("CANT MOVE");
					removeFoundItem();
					nextMove();
				}
				
				collectNextTurn = false;
			
			}else if(depositNextTurn){
			
				try {
					deposit();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				depositNextTurn = false;
				
			}else{
				try {
					scan(SCAN_RANGE);
				} catch (Exception e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
			break;
		
		case PollResult.SCAN:
			
			//System.out.println("FINISHED SCAN!!!");
			ScanItem[] items = pr.getScanItems();
			for(int s = 0; s < items.length; s++){
				
				if(items[s].getResourceType() == ScanItem.RESOURCE){
					
					addFoundItem(items[s].getxOffset(), items[s].getyOffset(),
							"resource");
				}
			}
			nextMove();
			
			break;
		}
	}
	
	private void printTrail(double[] end){
		
		double dx = (end[0]-rover.getX()) / 100;
		double dy = (end[1]-rover.getY()) /100;
		System.out.println("Start: " + end[0] + " " + end[1]);
		for(double x = rover.getX(); x < end[0]; x+= dx){
			for(double y = rover.getY(); y < end[1]; y+= dy){
				
				System.out.println(x + " " + y);
			}
		}
		System.out.println("End");
	}
	
	private double distance(double[] one, double[] two){
		
		return Math.sqrt(Math.pow(one[0] - two[0], 2) + Math.pow(one[1]-two[1], 2));
	}
	private void removeFoundItem() {
		// TODO Auto-generated method stub
		
		for(int t = 0; t < items.size(); t++){
			
			if(items.get(t).getX() == rover.getX() &&
					items.get(t).getY() == rover.getY()){
				
				items.remove(t);
				break;
			}
		}
	}
	private FoundItem findNearestResource(){
		
		FoundItem current = null;
		for(int t = 0; t < items.size(); t++){
			
			if(items.get(t).isResource() && (
					current == null || current.getDistance() > items.get(t).getDistance())){
				
				current = items.get(t);
			}
		}
		
		return current;
	}
	
	private void commPos(){
		
		this.broadCastToTeam("");
	}
	
	private void notifyOfExploreSpot(ArrayList<Argument> args){
		
		double x = args.get(0).toDouble();
		double y = args.get(1).toDouble();
		
		for(int e = 0; e < exploreSpots.size(); e++){
			
			if(exploreSpots.get(e).isInPos(x,y)){
				
				exploreSpots.get(e).setExplored();
				break;
			}
		}
	}
	
	private void nextMove(){
		
		FoundItem nearestRes = findNearestResource();
		
		if(nearestRes != null){
			
	
			try {
				double[] moveTo = FoundItem.getAdjustedMove(nearestRes.getX() - rover.getX(),
						nearestRes.getY() - rover.getY(),getWorldWidth(),getWorldHeight());
				move(moveTo[0],moveTo[1],4);
				rover.move(nearestRes.getX()- rover.getX(), nearestRes.getY()- rover.getY());
				collectNextTurn = true;
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
		}else{
		
			try {
				double[] newEx = exploreSpots.get(nextExploreSpot).toArray();
				exploreSpots.get(nextExploreSpot).setExplored();
				nextExploreSpot();
				move(newEx[0] - rover.getX(),newEx[1] - rover.getY(), 4);
				rover.move(newEx[0] - rover.getX(),newEx[1] - rover.getY());
				//String msg = onto.createCommand(RoverOntology.EXPLORESPOT, new Double(newEx[0]).toString(),
					//	new Double(newEx[1]).toString());
				//System.out.println(msg + "|Enter");
				//broadCastToTeam(msg);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
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
	
	private void addFoundItem(double offsetX,double offsetY,String name){
		
		items.add(new FoundItem(offsetX + rover.getX(),offsetY + rover.getY(),name,
				getWorldWidth(),getWorldHeight()));
	}
	
//	private double[] findNextExploreSpot(){
//		
//		ExplorationSite closest = null;
//		double closestDis = Double.MAX_VALUE;
//		
//		System.out.println(exploreSpots.size() + " SimpleRover");
//		for(int e = 0; e < exploreSpots.size(); e++){
//			
//			double newDis;
//			if((newDis = distance(rover.toArray(),exploreSpots.get(e).toArray())) < closestDis){
//				
//				closestDis = newDis;
//				closest = exploreSpots.get(e);
//			}
//		}
//		
//		return closest.toArray();
//	}
	

}
