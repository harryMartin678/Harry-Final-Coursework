import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;

import javax.imageio.ImageIO;

import Util.Location;
import Util.Vector;

//AIzaSyBi4zuPk75Av1GHNv8TFrPAhAQqGM-uwj0
//http://maps.googleapis.com/maps/api/staticmap?center=48.167432,10.533072&size=400x400&sensor=true&zoom=7
public class MapParser {
	
	private BufferedImage img;
	private Location center;
	private Location[] corners;
	private Vector[] mapVec;
	private int zoomLevel;
	//private double pixelSize;
	private final static double LAT_IN_METRES = 110574.61087757687;
	private final static double lONG_IN_METRES = 111302.61697430261;
	public static final int IMAGE_RESOLUTION = 500;
	
	public MapParser(Location center, int zoomLevel) throws MalformedURLException, IOException{
		
		this.center = center;
		this.zoomLevel = zoomLevel;
		img = getMapImage(center, zoomLevel);
		//pixelSize = pixelsInMetres(center.getLat(),zoomLevel,imageRes);
		//51.3758,2.3599
		corners = getCorners(IMAGE_RESOLUTION,center,zoomLevel);
		mapVec = createVectors(corners);
		
		for(int c = 0; c < corners.length; c++){
			
			corners[c].print();
		}
	}
	
	public static double pixelsInMetres(double latitude,int zoomLevel,int imageRes){
		
		return (Math.cos(latitude * Math.PI/180) * 2 * Math.PI * 6378137)
				/ (imageRes * Math.pow(2,zoomLevel));
	}
	
//	private void locationType(Location loc){
//		
//		double latDist = (center.getLat() - loc.getLat()) * (pixelSize * LAT_IN_METRES);
//		double lonDist = (center.getLon() - loc.getLon()) * (pixelSize * lONG_IN_METRES);
//		
//		double pixelX = (img.getWidth()/2) - latDist;
//		double pixelY = (img.getHeight()/2) - lonDist;
//	}
	
	public static Location[] getCorners(int imageRes,
			Location center,int zoomLevel){
		
		Location[] corners = new Location[4];
		
		int noOfPixelsHor = imageRes/2;
		int noOfPixelsVer = imageRes/2;
		
		double pixelSize = pixelsInMetres(center.getLat(),zoomLevel,imageRes);
		
		double lat = noOfPixelsHor * (pixelSize / LAT_IN_METRES);
		double lon = noOfPixelsVer * (pixelSize / lONG_IN_METRES);
		
		
		int[] lc = new int[]{1,1,-1,1,1,-1,-1,-1};
		
		for(int l = 0; l < lc.length; l+=2){
			
			corners[l/2] = new Location(center.getLat() + (lat * lc[l]),center.getLon() + (lon * lc[l+1]));
		}
		
		return corners;
	}
	
	
	private BufferedImage getMapImage(Location center, int zoomLevel) throws MalformedURLException, IOException{
		
		return ImageIO.read(new URL(getImageURL(center,zoomLevel)));
	}
	
	
	private String getImageURL(Location center,int zoomLevel){
		
		return "http://maps.googleapis.com/maps/api/staticmap?center="+new Double(center.getLat()).toString()
				+","+new Double(center.getLon()).toString() + "&size=" + IMAGE_RESOLUTION + "x"
						+ IMAGE_RESOLUTION + "&sensor=true&zoom=" + 
				new Integer(zoomLevel).toString();
	}
	
	public Location[] intersection(Vector[] boxVec){
		
		ArrayList<Location> intersections = new ArrayList<Location>();
		
		for(int v = 0; v < boxVec.length; v++){
			for(int m = 0; m < mapVec.length; m++){
				
				double t = mapVec[m].intersects(boxVec[v]);
				
				if(t != -1.0){
					
					intersections.add(mapVec[m].getLocation(t));
				}
			}
		}
		
		return null;
		
	}
	
	
	public static Vector[] createVectors(Location[] locations){
		
		Vector[] vectors = new Vector[locations.length];
		
		for(int v = 1; v < vectors.length; v++){
			
			vectors[v] = new Vector(locations[v-1], locations[v]);
		}
		
		vectors[0] = new Vector(locations[locations.length-1],locations[0]);
		
		return vectors;
	}
	
	
	public static void main(String[] args) {
		
		try {
			new MapParser(new Location(51.3758,-2.3599),13);
		} catch (MalformedURLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
}
