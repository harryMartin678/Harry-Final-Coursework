package rover;

import java.util.ArrayList;
import java.util.HashMap;

import org.agentscape.config.Arguments;

import rover.Ontology.Argument;
import rover.Ontology.MethodCall;

public class CollectingRover extends Rover {

	private HashMap<Integer,FoundItem> resources;
	private int MAX_SPEED = 5;
	private RoverOntology onto;
	private FoundItem rover;
	private boolean collectNextTurn;
	private boolean depositNextTurn;
	private boolean roverHasEnded;
	
	public CollectingRover() {
		// TODO Auto-generated constructor stub
		super();
		setTeam("hm474");
		
		try {
			//set attributes for this rover
			//speed, scan range, max load
			//has to add up to <= 9
			//Fourth attribute is the collector type
			setAttributes(MAX_SPEED, 0, 3, 1);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	void begin() {
		// TODO Auto-generated method stub
		System.out.println("START COLLECTING ROVER!!!");
		resources = new HashMap<Integer,FoundItem>();
		onto = new RoverOntology();
		rover = new FoundItem(0, 0, "Rover", getWorldWidth(), getWorldHeight());
		roverHasEnded = false;
		nextMove();
		
		new Thread(new Runnable(){

			@Override
			public void run() {
				// TODO Auto-generated method stub
				while(!roverHasEnded){
					
					retrieveMessages();
					
					for(String msg : messages){
						
						parseMessage(msg);
					}
					
					try {
						Thread.sleep(100);
					} catch (InterruptedException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
			
			private void recieveResourceLoc(ArrayList<Argument> args){
				
				double x = args.get(0).toDouble();
				double y = args.get(1).toDouble();
				int key = FoundItem.getKey(new double[]{x,y});
				
				System.out.println(x + " " + y + " Collecting Rover");
				
				if(!resources.containsKey(key)){
					
					resources.put(key, new FoundItem(x,y,"Resource",getWorldWidth(),getWorldHeight()));
				}
				
				
			}
			
			private void parseMessage(String msg){
				
				MethodCall method = onto.analysisCommand(msg);
				if(method.getMethodName() == "recieveResourceLoc"){
					
					recieveResourceLoc(method.getArgs());
				}
			}
			
		}).start();
	}

	@Override
	void end() {
		// TODO Auto-generated method stub
		System.out.println("END COLLECTING ROVER!!!");
		cleanUp();
		roverHasEnded = true;
	}

	@Override
	void poll(PollResult pr) {
		// TODO Auto-generated method stub
		switch(pr.getResultType()){
		case PollResult.MOVE:
			if(collectNextTurn){
				
				try {
					collect();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					//System.out.println("CANT MOVE");
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
				
			}
			break;
		
		case PollResult.COLLECT:
			
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
			
		case PollResult.DEPOSIT:
			//System.out.println("FINISHED DEPOSIT!!!");
			nextMove();
			break;
		
		}
		
		
	}
	
	private FoundItem findNearestResource(){
		
		FoundItem current = null;
		for(int t = 0; t < resources.size(); t++){
			
			if(resources.get(t).isResource() && (
					current == null || current.getDistance() > resources.get(t).getDistance())){
				
				current = resources.get(t);
			}
		}
		
		return current;
	}
	
	private void nextMove(){
		
		FoundItem nearestRes = findNearestResource();
		
		//System.out.println((nearestRes == null) + " CollectingRover");
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
				Thread.sleep(200);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			
			nextMove();
		}
		
		
	}

}
