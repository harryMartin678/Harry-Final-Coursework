package Util;

public class Location {

	private double latitude;
	private double longitude;
	
	public Location(){
		
		
	}
	
	public Location(double latitude,double longitude){
		
		this.latitude = latitude;
		this.longitude = longitude;
	}
	
	public void setLat(double lat){
		
		this.latitude = lat;
	}
	
	public void setLong(double lon){
		
		this.longitude = lon;
	}
	
	public double getLat(){
		
		return this.latitude;
	}
	
	public double getLon(){
		
		return this.longitude;
	}

	public void print() {
		// TODO Auto-generated method stub
		System.out.println("{" + latitude + "," + longitude + "}");
	}
}
