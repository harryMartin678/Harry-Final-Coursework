package Util;

public class Vector {

	private Location start;
	private Location direction;
	private double maxT;
	
	public Vector(Location one, Location two){
		
		this.start = one;
		direction = minusLocations(two, one);
		
		maxT = minusLocations(one,two).getLat() / direction.getLat();
		
		
	}
	
	private Location minusLocations(Location one, Location two){
		
		return new Location(one.getLat() - two.getLat(),one.getLon() - two.getLon());
	}
	
	public double intersects(Vector vec){
		
		double latT = (vec.getStartLat() - start.getLat()) / direction.getLat();
		double lonT = (vec.getStartLon() - start.getLon()) / direction.getLon();
		
		if(latT == lonT && latT >= 0 && latT <= maxT){
			
			return latT;
		
		}else{
			
			return -1.0;
		}
	}

	private double getStartLon() {
		// TODO Auto-generated method stub
		return start.getLon();
	}

	private double getStartLat() {
		// TODO Auto-generated method stub
		return start.getLat();
	}

	public Location getLocation(double t) {
		// TODO Auto-generated method stub
		return new Location(start.getLat() + direction.getLat()*t,
				start.getLon() + direction.getLon()*t);
	}

}
