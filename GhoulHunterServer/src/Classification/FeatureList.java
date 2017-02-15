package Classification;

import java.awt.GridLayout;
import java.awt.image.BufferedImage;
import java.awt.image.ColorModel;
import java.awt.image.DataBufferByte;
import java.awt.image.WritableRaster;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;

import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import Util.Location;

public class FeatureList {
	
	private final int IMAGE_RESOLUTION = 800;
	
	public FeatureList() throws MalformedURLException, IOException{
		
		System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
		//51.3758° N, 2.3599° W
		BufferedImage map = getMapImage(new Location(51.3758,-2.3699),18);
		
		int[][] contastImage = getAverageDifferenceArray(map);
		//extendConstast(contastImage,map);
		BufferedImage mapCanny = deepCopy(map);
		//TreeFeature feature = new TreeFeature(map);
		//HeirarchicalFeature feature = new HeirarchicalFeature(map);
		FixedTreeFeature fixedTree = new FixedTreeFeature(map);
		WaveClassifier classifier = new WaveClassifier(fixedTree.getObjects());
		//FlattenedFeature flattenFeature = new FlattenedFeature(fixedTree);
		//contastImage = new int[map.getHeight()][map.getWidth()];
		
		//printArray(contastImage);
		ArrayList<int[]> highLights = highChangePoints(contastImage);
		//System.out.println(highLights.size());
		//drawOnHighLightsMap(map,highLights,16776960);
		
		BufferedImage mapToConvert = Image.toBufferedImageOfType(map,BufferedImage.TYPE_3BYTE_BGR);
		/*JFrame frame = new JFrame();
		frame.getContentPane().add(new JLabel(new ImageIcon(fromUrl)));
		
		frame.setSize(fromUrl.getWidth(), fromUrl.getHeight());
		
		frame.setVisible(true);*/
		Mat img = new Mat(mapToConvert.getHeight(), mapToConvert.getWidth(), CvType.CV_8UC4);
		img.put(0, 0, ((DataBufferByte)mapToConvert.getRaster().getDataBuffer()).getData());
		Mat edges = new Mat();
		Imgproc.Canny(img, edges, 100, 300);
		//Imgproc.HoughLines(img, edges, 1.0, Math.PI/180, 100);
		//System.out.println(edges.size());
		ArrayList<int[]> cannyHighLights = getEdgeLocations(edges);
		//drawOnHighLightsMap(mapCanny,cannyHighLights,16776960);
		JFrame frame = new JFrame();
		
		frame.setSize(1400,800);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setTitle("Image");
		frame.getContentPane().setLayout(new GridLayout(1,2));
		frame.getContentPane().add(new JLabel(new ImageIcon(map)));
		frame.getContentPane().add(new JLabel(new ImageIcon(mapCanny)));
		
		frame.setVisible(true);
	}
	
	private void extendConstast(int[][] avgDiff, BufferedImage map){
		
		for(int x = 0; x < map.getHeight(); x++){
			for(int y = 0; y < map.getWidth(); y++){
			
				map.setRGB(x, y, map.getRGB(x, y) - avgDiff[x][y]);
			}
		}
	}
	
	public static BufferedImage deepCopy(BufferedImage bi) {
		ColorModel cm = bi.getColorModel();
		boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
		WritableRaster raster = bi.copyData(null);
		return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
	}
	
	private ArrayList<int[]> getEdgeLocations(Mat edges){
		
		ArrayList<int[]> locs = new ArrayList<int[]>();
		
		for(int x = 0; x < edges.height(); x++){
			for(int y = 0; y < edges.width(); y++){
				
				if(edges.get(y, x)[0] > 0){
					
					locs.add(new int[]{x,y});
				}
			}
		}
		
		return locs;
	}
	
	private void drawOnHighLightsMap(BufferedImage map, ArrayList<int[]> highLights,
			int highLightColor){
		
		//int highLightColor = 16776960;
		int hightSize = 1;
		//int[] lc = new int[]{0,1,0,-1,1,0,-1,0,-1,-1,1,-1,1,1,-1,1};
		for(int h = 0; h < highLights.size(); h++){
			
			for(int x = -hightSize; x < hightSize; x++){
				for(int y = -hightSize; y < hightSize; y++){
					
					if(highLights.get(h)[0] + x >= 0 && highLights.get(h)[0] + x < map.getHeight() 
							&& highLights.get(h)[1] + y >= 0 && highLights.get(h)[1] + y < map.getWidth()){
						map.setRGB(highLights.get(h)[0] + x, highLights.get(h)[1] + y, highLightColor);
					}
				}
			}
		}
		
		/*for(int x = 0; x < map.getHeight(); x++){
			for(int y = 0; y < map.getWidth(); y++){
				
				map.setRGB(x,y, highLightColor);
			}
		}*/
	}
	
	private BufferedImage getMapImage(Location center, int zoomLevel) throws MalformedURLException, IOException{
		
		BufferedImage image =  ImageIO.read(new URL(getImageURL(center,zoomLevel)));
		
		return image.getSubimage(0, 0, image.getHeight()-100, image.getWidth()-100);
	}
	
	
	private String getImageURL(Location center,int zoomLevel){
		
		return "http://maps.googleapis.com/maps/api/staticmap?center="+new Double(center.getLat()).toString()
				+","+new Double(center.getLon()).toString() + "&size=" + IMAGE_RESOLUTION + "x"
						+ IMAGE_RESOLUTION + "&maptype=satellite&sensor=true&zoom=" + 
				new Integer(zoomLevel).toString();
	}
	
	private void printArray(int[][] array){
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[x].length; y++){
				
				System.out.print(array[x][y]+",");
			}
			System.out.println();
		}
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
	
	
	public ArrayList<int[]> highChangePoints(int[][] avgDist){
		
		int cutoff =  findTopPercent(avgDist);
		
		ArrayList<int[]> highLights = new ArrayList<int[]>();
		
		for(int x = 0; x < avgDist.length; x++){
			for(int y = 0; y < avgDist[x].length; y++){
				
				if(avgDist[x][y] >= cutoff){
					
					highLights.add(new int[]{x,y});
				}
			}
		}
		
		
		return highLights;
	}
	
	private int findTopPercent(int[][] array){
		
		double mean = 0.0;
		double stdDev = 0.0;
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[x].length; y++){
				
				mean += array[x][y];
			}
			
		}
		
		mean /= array.length * array[0].length;
		
		for(int x = 0; x < array.length; x++){
			for(int y = 0; y < array[0].length; y++){
				
				stdDev += Math.pow(mean - array[x][y], 2);
			}
		}
		
		stdDev /= array.length * array[0].length;
		
		stdDev = Math.pow(stdDev, 0.5);
		
		return (int) (mean + stdDev);
	}
	
	
	
	
	public static void main(String[] args) throws MalformedURLException, IOException {
		
		new FeatureList();
	}

}
